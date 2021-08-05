#ifndef HACKRF_CONTROLLER_HPP
#define HACKRF_CONTROLLER_HPP

/**
 * Handles communication with the HackRF One SDR.
 */

#include <cstdint>
#include <future>

class HackRfController
{
public:

    /**
     * Creates an instance of the HackRfController.
     *
     * @param sim_file_path GPS simulation file to transmit.
     * @param gain          TX gain to add to transmission power.
     */
    HackRfController (const std::string& sim_file_path, uint8_t gain);

    ~HackRfController ();

    void startTransfer ();

    void stopTransfer ();

    bool isTransmitting () const;

private:

    /**
     * Path to GPS simulation file.
     */
    std::string sim_file_path;

    /**
     * Path to the hackrf_transfer program.
     */
    char* hackrf_transfer_path;

    /**
     * TX gain added to transmission power.
     */
    uint8_t gain;

    /**
     *  Field that is updated when the transfer is started/stopped.
     */
    bool transmitting;

    /**
     * Thread that runs the hackrf_transfer program and stops it when requested.
     */
    std::thread watchdog_thread;

    /**
     * Signals to tell the transfer thread to stop.
     */
    std::binary_semaphore notifyEnd{ 0 };

    /**
     * Implementation of starting the transfer and waiting for the request to stop.
     */
    void watchdog_task (pid_t hackrf_transfer_pid);
};

#endif //HACKRF_CONTROLLER_HPP
