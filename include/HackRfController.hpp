#ifndef HACKRF_CONTROLLER_HPP
#define HACKRF_CONTROLLER_HPP

/**
 * Handles communication with the HackRF One SDR.
 */

#include <cstdint>
#include <future>

#include <QObject>

class HackRfController : public QObject
{
Q_OBJECT

public:

    /**
     * Creates an instance of the HackRfController.
     *
     * @param sim_file_path GPS simulation file to transmit.
     * @param gain          TX gain to add to transmission power.
     */
    explicit HackRfController (std::string sim_file_path, uint8_t gain = 0, QObject* parent = nullptr);

    ~HackRfController () override;

    void setGain (uint8_t newGain);

    uint8_t getGain () const;

    void startTransfer ();

    void stopTransfer ();

    bool isTransmitting () const;

signals:

    void startedTransmit ();

    void stoppedTransmit ();

private:

    /**
     * Path to GPS simulation file.
     */
    std::string sim_file_path;

    /**
     * Path to the hackrf_transfer program.
     */
    char* hackrf_transfer_path{};

    /**
     * TX gain added to transmission power.
     */
    uint8_t gain;

    /**
     *  Field that is updated when the transfer is started/stopped.
     */
    std::atomic<bool> transmitting;

    /**
     * Thread that runs the hackrf_transfer program and stops it when requested.
     */
    std::thread watchdog_thread;

    /**
     * Signals to tell the transfer thread to stop.
     */
    std::mutex conditionMutex;
    std::condition_variable notifyEnd;

    /**
     * Controls access to the start and stop functions.
     */
    std::mutex startStopMutex;

    /**
     * Implementation of starting the transfer and waiting for the request to stop.
     */
    void watchdog_task (pid_t hackrf_transfer_pid);

    /**
     * Performs a series of runtime checks to ensure hackrf_transfer will succeed. Throws a runtime error if any checks
     * fail.
     */
    void runtimeChecks ();
};

#endif //HACKRF_CONTROLLER_HPP
