#include <QApplication>
#include "MainWindow.hpp"
#include <loguru.hpp>

int main (int argc, char* argv[])
{
    // Configure loguru
    loguru::g_colorlogtostderr = false;
    loguru::g_preamble_uptime = false;
    loguru::g_preamble_file = false;
    loguru::g_preamble_thread = false;
    loguru::g_preamble_date = false;

    QApplication a (argc, argv);
    MainWindow window;
    window.show ();
    return QApplication::exec ();
}
