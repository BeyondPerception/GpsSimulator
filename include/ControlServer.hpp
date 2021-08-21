#ifndef CONTROL_SERVER_HPP
#define CONTROL_SERVER_HPP

#include <sys/socket.h>
#include "HackRfController.hpp"

class ControlServer
{
public:
    explicit ControlServer (HackRfController* controller, uint16_t port = 1111);

    ~ControlServer ();

    void start ();

    void stop ();

private:
    int port;

    int socketFd;

    HackRfController* hackRfController;

    std::atomic<bool> serverRunning;

    std::thread socketThread;

    void sendMessage (struct sockaddr_in* clientAddr, socklen_t len, const char* message) const;

    void control_task ();

    void sendError (sockaddr_in* clientAddr, socklen_t len, const char* message) const;

    void send (sockaddr_in* clientAddr, socklen_t len, const char* message) const;

    void sendConfirm (sockaddr_in* clientAddr, socklen_t len) const;
};

#endif //CONTROL_SERVER_HPP
