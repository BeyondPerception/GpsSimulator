#include <QApplication>
#include <unistd.h>
#include <getopt.h>
#include <loguru.hpp>

#include "MainWindow.hpp"
#include "ControlServer.hpp"

int main (int argc, char* argv[])
{
    // Configure loguru
    loguru::g_colorlogtostderr = false;
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_file = false;
    loguru::g_preamble_thread = false;
    loguru::g_preamble_date = false;

    char* gpsSimFile = nullptr;

    struct option long_options[] = {{ "simfile", required_argument, nullptr, 'f' },
                                    { nullptr, 0,                   nullptr, 0 }};
    int c;
    while ((c = getopt_long (argc, argv, "f:", long_options, nullptr)) != -1)
    {
        if (c == 'f')
        {
            gpsSimFile = optarg;
            break;
        }
    }

    std::string homeDir = getenv ("HOME");
    HackRfController* hackRfController;
    if (gpsSimFile == nullptr)
    {
        hackRfController = new HackRfController (homeDir + "/gpssim.bin");
    } else
    {
        hackRfController = new HackRfController (gpsSimFile);
    }

    // Setup server
    ControlServer server (hackRfController);
    server.start ();

    // Setup gui
    QCoreApplication::setApplicationName ("GPSSimulator");
    QApplication::setFont (QFont ("Ubuntu Mono"));
    QApplication a (argc, argv);
    MainWindow window (hackRfController);
    window.show ();
    return QApplication::exec ();
}
