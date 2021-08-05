#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <HackRfController.hpp>
#include <QHackRfButton.hpp>
#include <GpsSdrSim.hpp>
#include <QScrollBar>

MainWindow::MainWindow (QWidget* parent) :
    QMainWindow (parent), ui (new Ui::MainWindow), hackRfController (nullptr)
{
    ui->setupUi (this);

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
}

void MainWindow::startHackRfPressed ()
{
    if (hackRfController != nullptr && hackRfController->isTransmitting ())
    {
        hackRfController->stopTransfer ();
        delete hackRfController;
        ui->startHackRfButton->setOff ("");
    } else
    {
        try
        {
            std::string homeDir = getenv ("HOME");
            hackRfController = new HackRfController ((homeDir + "/gpssim.bin").c_str (),
                                                     ui->startHackRfButton->getDbGain ());
        } catch (const std::invalid_argument& e)
        {
            ui->startHackRfButton->setWarning (e.what ());
            return;
        } catch (const std::runtime_error& e)
        {
            ui->startHackRfButton->setError (e.what ());
            return;
        }
        hackRfController->startTransfer ();
        ui->startHackRfButton->setOk ("Transmitting...");
    }
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