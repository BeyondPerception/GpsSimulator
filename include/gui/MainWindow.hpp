#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QKeyEvent>

#include "HackRfController.hpp"
#include "QDebugStream.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow (HackRfController* controller, QWidget* parent = nullptr);

    ~MainWindow () override;

private slots:

    void toggleFullScreenLog ();

    void startHackRfPressed ();

    void generateGpsSim ();

private:
    Ui::MainWindow* ui;

    QDebugStream* qout;
    QDebugStream* qerr;

    HackRfController* hackRfController;
};

#endif //MAINWINDOW_HPP
