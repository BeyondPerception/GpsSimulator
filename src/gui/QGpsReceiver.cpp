
#include <QGpsReceiver.hpp>
#include <QVBoxLayout>
#include <cmath>
#include <iostream>
#include <sstream>
#include "loguru.hpp"

QGpsReceiver::QGpsReceiver (QWidget* parent) : QStatusButton (parent), threadRunning (false)
{
    auto* vBoxLayout = new QVBoxLayout (this);

    QPalette pal = palette ();
    pal.setBrush (QPalette::All, QPalette::Text, Qt::black);
    setPalette (pal);

    mainText = new QLabel ("No Fix");
    mainText->setFont (QFont ("Ubuntu Mono", 14, QFont::Bold));
    mainText->setAlignment (Qt::AlignCenter);
    mainText->setWordWrap (true);
    vBoxLayout->addWidget (mainText);

    errorText = new QLabel ("");
    errorText->setFont (QFont ("Ubuntu Mono", 14, QFont::Medium));
    errorText->setAlignment (Qt::AlignCenter);
    errorText->hide ();
    vBoxLayout->addWidget (errorText);

    qRegisterMetaType<Status> ();

    connect (this, &QGpsReceiver::fixAcquired, this, &QGpsReceiver::setMainText);
    connect (this, &QGpsReceiver::errorEncountered, this, &QGpsReceiver::setErrorText);
}

QGpsReceiver::~QGpsReceiver ()
{
    if (threadRunning)
    {
        threadRunning = false;
        gpsQueryThread.join ();
    }

    delete mainText;
    delete errorText;
}

void QGpsReceiver::startReceiver ()
{
    threadRunning = true;
    gpsQueryThread = std::thread (&QGpsReceiver::gpsquery_task, this);
}

void QGpsReceiver::transmitStarted ()
{
    transmitStartTime = std::chrono::high_resolution_clock::now ();
    transmitEndTime = transmitStartTime;
}

void QGpsReceiver::setMainText (const QString& text, Status status)
{
    mainText->setText (text);
    setStatus (status);
}

void QGpsReceiver::setErrorText (const QString& error, Status status)
{
    errorText->setText (error);
    setStatus (status);
    switch (status)
    {
        case OFF:
        case OK:
            errorText->hide ();
            break;
        case WARNING:
        case ERROR:
            errorText->show ();
            break;
    }
}

void QGpsReceiver::gpsquery_task ()
{
    int ret = gps_open ("localhost", "2947", &gpsData);
    if (ret)
    {
        emit
        errorEncountered (QString::fromStdString (gps_errstr (ret)), ERROR);
        return;
    }
    errorText->hide ();

    gps_stream (&gpsData, WATCH_ENABLE | WATCH_JSON, nullptr);

    struct gps_data_t oldGpsData{};
    uint64_t loops = 0;

    while (threadRunning)
    {
        if (loops > 20)
        {
            // Stale data, emit no fix.
            emit fixAcquired (QString::fromStdString ("No Fix"), OFF);
            loops = 0;
            continue;
        }
        if (gps_waiting (&gpsData, 500000))
        {
            int gpsReadRet;
#if GPSD_API_MAJOR_VERSION < 7
            gpsReadRet = gps_read (&gpsData);
#else
            gpsReadRet = gps_read (&gpsData, nullptr, 0);
#endif
            if (gpsReadRet != -1)
            {
                if (gpsData.fix.mode >= MODE_2D && gpsData.dop.hdop < 20 && gpsData.satellites_used > 0)
                {
                    if (memcmp (&oldGpsData, &gpsData, sizeof (gps_data_t)) == 0)
                    {
                        loops++;
                    } else
                    {
                        loops = 0;
                    }
                    oldGpsData = gpsData;
                    if (transmitStartTime == transmitEndTime)
                    {
                        transmitEndTime = std::chrono::high_resolution_clock::now ();
                    }
                    std::ostringstream hdopFormat;
                    hdopFormat.precision (2);
                    hdopFormat << std::fixed << gpsData.dop.hdop;
                    if (gpsData.fix.mode == MODE_2D)
                    {
                        std::string display = "2D Fix\n" + std::to_string (gpsData.fix.latitude) + "," +
                                              std::to_string (gpsData.fix.longitude) + "\nhdop: " +
                                              hdopFormat.str () + "\n# Sats: " +
                                              std::to_string (gpsData.satellites_used) + "\n\nFix in: " +
                                              std::to_string (std::chrono::duration_cast<std::chrono::seconds> (
                                                  transmitEndTime - transmitStartTime).count ()) + " seconds";
                        emit fixAcquired (QString::fromStdString (display), WARNING);
                    } else
                    {
                        std::string display = "3D Fix\n" + std::to_string (gpsData.fix.latitude) + "," +
                                              std::to_string (gpsData.fix.longitude) + "," +
                                              std::to_string (gpsData.fix.altitude) + "\nhdop: " +
                                              hdopFormat.str () + "\n# Sats: " +
                                              std::to_string (gpsData.satellites_used) + "\n\nFix in: " +
                                              std::to_string (std::chrono::duration_cast<std::chrono::seconds> (
                                                  transmitEndTime - transmitStartTime).count ()) + " seconds";
                        emit fixAcquired (QString::fromStdString (display), OK);
                    }
                } else
                {
                    emit fixAcquired (QString::fromStdString ("No Fix"), OFF);
                }
            }
        } else
        {
            loops++;
        }
    }

    gps_stream (&gpsData, WATCH_DISABLE, nullptr);
    gps_close (&gpsData);
}
