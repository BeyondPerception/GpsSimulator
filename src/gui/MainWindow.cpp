#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <iostream>
#include <sstream>
#include <HackRfController.hpp>
#include <QHackRfButton.hpp>

MainWindow::MainWindow (QWidget* parent) :
    QMainWindow (parent), ui (new Ui::MainWindow), hackRfController (nullptr)
{
    ui->setupUi (this);

    // Setup on-screen log
    qout = new QDebugStream (std::cout, ui->logButton);
    qerr = new QDebugStream (std::cerr, ui->logButton, true);

    QPalette palette = ui->logButton->palette ();
    palette.setColor (QPalette::Base, QColor (33, 33, 41));
    palette.setColor (QPalette::Text, Qt::white);
    ui->logButton->setPalette (palette);
    ui->logButton->setFont (QFont ("Ubuntu Mono", 12));

    // Try to do an initial setup of hackrf controller.

    // Register callbacks
    connect (ui->internalStatusButton, &QPushButton::released, this, &MainWindow::internalStatusPressed);
    connect (ui->logButton, &QLogButton::released, this, &MainWindow::toggleFullScreenLog);
    connect (ui->startHackRfButton, &QPushButton::released, this, &MainWindow::startHackRfPressed);
    connect (ui->gainSlider, &QSlider::sliderMoved, ui->startHackRfButton, &QHackRfButton::setDbGain);
}

MainWindow::~MainWindow ()
{
    delete qout;
    delete qerr;
    delete ui;
}

void MainWindow::internalStatusPressed ()
{
    switch (ui->internalStatusButton->getStatus ())
    {
        case OK:
            ui->internalStatusButton->setStatus (WARNING);
            break;
        case WARNING:
            ui->internalStatusButton->setStatus (ERROR);
            break;
        case ERROR:
            ui->internalStatusButton->setStatus (OFF);
            break;
        case OFF:
            ui->internalStatusButton->setStatus (OK);
            break;
    }
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
            hackRfController = new HackRfController ("/home/ronak/gpssim.bin", ui->startHackRfButton->getDbGain ());
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
