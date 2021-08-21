#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <HackRfController.hpp>
#include <QHackRfButton.hpp>
#include <GpsSdrSim.hpp>
#include <QScrollBar>
#include <filesystem>
#include <loguru.hpp>

MainWindow::MainWindow (HackRfController* controller, QWidget* parent) :
    QMainWindow (parent), ui (new Ui::MainWindow), hackRfController (controller)
{
    ui->setupUi (this);

    setWindowTitle ("GpsSimulator");

    // Setup on-screen log
    qout = new QDebugStream (std::cout, ui->logButton);
    qerr = new QDebugStream (std::cerr, ui->logButton, true);

    // Start GPS Receiver
    ui->gpsReceiver->startReceiver ();

    // Register callbacks
    connect (ui->logButton, &QLogButton::released, this, &MainWindow::toggleFullScreenLog);
    connect (ui->startHackRfButton, &QPushButton::released, this, &MainWindow::startHackRfPressed);
    connect (ui->gpsSimButton, &QGpsSimButton::generatePressed, this, &MainWindow::generateGpsSim);
    connect (ui->gainSlider, &QSlider::sliderMoved, ui->startHackRfButton, &QHackRfButton::setDbGain);
    connect (ui->durationSlider, &QSlider::sliderMoved, ui->gpsSimButton, &QGpsSimButton::setDuration);
    connect (hackRfController, &HackRfController::startedTransmit, this, &MainWindow::setTransmitting);
    connect (hackRfController, &HackRfController::stoppedTransmit, this, &MainWindow::setStopped);
}

MainWindow::~MainWindow ()
{
    delete qout;
    delete qerr;
    delete ui;
}

void MainWindow::toggleFullScreenLog ()
{
    static bool isFullScreen = false;
    if (!isFullScreen)
    {
        ui->gridLayout->removeWidget (ui->logButton);
        ui->stackedWidget->addWidget (ui->logButton);
        ui->stackedWidget->setCurrentIndex (1);
        ui->logButton->show ();
        isFullScreen = true;
    } else
    {
        ui->stackedWidget->removeWidget (ui->logButton);
        ui->gridLayout->addWidget (ui->logButton, 1, 1);
        ui->stackedWidget->setCurrentIndex (0);
        ui->logButton->show ();
        isFullScreen = false;
    }
    ui->logButton->scrollToEnd ();
}

void MainWindow::startHackRfPressed ()
{
    if (hackRfController != nullptr && hackRfController->isTransmitting ())
    {
        hackRfController->stopTransfer ();
    } else
    {
        hackRfController->setGain (ui->startHackRfButton->getDbGain ());
        try
        {
            hackRfController->startTransfer ();
        } catch (const std::invalid_argument& e)
        {
            ui->startHackRfButton->setWarning (e.what ());
            return;
        } catch (const std::runtime_error& e)
        {
            ui->startHackRfButton->setError (e.what ());
            return;
        }
    }
}

void MainWindow::setTransmitting ()
{
    ui->gpsReceiver->transmitStarted ();
    ui->startHackRfButton->setOk ("Transmitting...");
}

void MainWindow::setStopped ()
{
    ui->startHackRfButton->setOff ("");
}

void MainWindow::generateGpsSim ()
{
    std::string homeDir = getenv ("HOME");
    std::thread generator_thread (&GpsSdrSim::generateGpsSimulation,
                                  homeDir + "/gps-sdr-sim/gps-sdr-sim",
                                  homeDir + "/brdc_file",
                                  ui->gpsSimButton->getLatitude (),
                                  ui->gpsSimButton->getLongitude (),
                                  ui->gpsSimButton->getHeight (),
                                  ui->gpsSimButton->getDuration (),
                                  homeDir + "/gpssim.bin");
    generator_thread.detach ();
}
