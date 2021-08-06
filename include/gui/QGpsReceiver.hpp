#ifndef QGPS_RECEIVER_HPP
#define QGPS_RECEIVER_HPP

#include <QStatusButton.hpp>
#include <QLabel>

#include <gps.h>
#include <thread>

class QGpsReceiver : public QStatusButton
{
Q_OBJECT

public:
    explicit QGpsReceiver (QWidget* parent = nullptr);

    ~QGpsReceiver () override;

    void startReceiver ();

    void transmitStarted ();

private slots:

    void setErrorText (const QString& error, Status status);

    void setMainText (const QString& text, Status status);

signals:

    void errorEncountered (const QString& error, Status status);

    void fixAcquired (const QString& text, Status status);

private:
    std::atomic<bool> threadRunning;

    std::thread gpsQueryThread;

    QLabel* mainText;

    QLabel* errorText;

    struct gps_data_t gpsData;

    std::chrono::system_clock::time_point transmitStartTime;
    std::chrono::system_clock::time_point transmitEndTime;

    void gpsquery_task ();
};

#endif //QGPS_RECEIVER_HPP
