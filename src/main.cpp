#include <QApplication>
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

    std::string homeDir = getenv ("HOME");
    auto* hackRfController = new HackRfController (homeDir + "/gpssim.bin");

    // Setup server
    ControlServer server (hackRfController);
    server.start ();

    // Setup gui
//    QCoreApplication::setApplicationName ("GPSSimulator");
//    QApplication::setFont (QFont ("Ubuntu Mono"));
//    QApplication a (argc, argv);
//    MainWindow window (hackRfController);
//    window.show ();
//    return QApplication::exec ();
    system ("read");
}
