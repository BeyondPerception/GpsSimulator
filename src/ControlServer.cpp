#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <loguru.hpp>
#include <sstream>
#include <iostream>
#include "ControlServer.hpp"

ControlServer::ControlServer (HackRfController* controller, uint16_t port) :
    hackRfController (controller),
    port (port),
    serverRunning (false)
{
    socketFd = socket (AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (socketFd < 0)
    {
        throw std::runtime_error ("socket() failed: " + std::string (strerror (errno)));
    }
}

ControlServer::~ControlServer ()
{
    std::cout << "Stopping Control Server.\n";
    if (serverRunning)
    {
        stop ();
    }
}

void ControlServer::start ()
{
    struct sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons (port);

    if (bind (socketFd, (const struct sockaddr*) &servaddr, sizeof (servaddr)) < 0)
    {
        throw std::runtime_error ("bind() failed: " + std::string (strerror (errno)));
    }

    serverRunning = true;
    socketThread = std::thread (&ControlServer::control_task, this);
    socketThread.detach ();
}

void ControlServer::stop ()
{
    serverRunning = false;
    close (socketFd);
}

void ControlServer::send (struct sockaddr_in* clientAddr, socklen_t len, const char* message) const
{
    if (sendto (socketFd, message, strlen (message), MSG_CONFIRM,
                (const struct sockaddr*) clientAddr, len) < 0)
    {
        LOG_F (INFO, "sendto() failed: %s", strerror (errno));
    }
}

void ControlServer::sendConfirm (struct sockaddr_in* clientAddr, socklen_t len) const
{
    send (clientAddr, len, "OK\n");
}

void ControlServer::sendMessage (struct sockaddr_in* clientAddr, socklen_t len, const char* message) const
{
    send (clientAddr, len, "OK\n");
    send (clientAddr, len, message);
}

void ControlServer::sendError (struct sockaddr_in* clientAddr, socklen_t len, const char* message) const
{
    send (clientAddr, len, "ERROR\n");
    send (clientAddr, len, message);
}

void ControlServer::control_task ()
{
    struct sockaddr_in clientaddr{};
    while (serverRunning)
    {
        char buffer[64];
        socklen_t len = sizeof (sockaddr_in);
        ssize_t n = recvfrom (socketFd, buffer, 64, MSG_WAITALL, (struct sockaddr*) &clientaddr, &len);
        if (n < 0)
        {
            LOG_F (INFO, "recvfrom() failed: %s", strerror (errno));
            continue;
        }
        std::string line (buffer, n);
        std::istringstream iss (line);

        std::string command;
        iss >> command;

        if (command == "start")
        {
            try
            {
                hackRfController->startTransfer ();
            } catch (const std::exception& e)
            {
                sendError (&clientaddr, len, e.what ());
                continue;
            }
            sendConfirm (&clientaddr, len);
        } else if (command == "stop")
        {
            hackRfController->stopTransfer ();
            sendConfirm (&clientaddr, len);
        } else if (command == "info")
        {
            std::string infoMessage = "transmitting: ";
            infoMessage += (hackRfController->isTransmitting () ? "true\n" : "false\n");
            sendMessage (&clientaddr, len, infoMessage.c_str ());
        } else if (command == "set")
        {
            std::string variable, value;
            iss >> variable >> value;
            if (variable == "gain")
            {
                uint8_t gain;
                try
                {
                    gain = std::stoul (value);
                    if (gain > 47)
                    {
                        throw std::exception ();
                    }
                    hackRfController->setGain (gain);
                } catch (...)
                {
                    sendError (&clientaddr, len, "Gain must be a number between 0 and 47.\n");
                    continue;
                }
                sendConfirm (&clientaddr, len);
            }
        } else if (command == "help")
        {
            sendMessage (&clientaddr, len,
                         "start: Start transmitting\n"
                         "stop: Stop transmitting\n"
                         "set <variable> <value>: Available vars: gain\n"
                         "info: Print info about current status\n");
        }
    }
}
