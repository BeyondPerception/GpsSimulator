#include <filesystem>
#include <thread>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <iostream>
#include <utility>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <linux/sched.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "loguru.hpp"
#include "HackRfController.hpp"

HackRfController::HackRfController (std::string sim_file_path, uint8_t gain, QObject* parent) :
    sim_file_path (std::move (sim_file_path)),
    gain (gain),
    transmitting (false),
    hackrf_transfer_path (nullptr),
    QObject (parent)
{
}

HackRfController::~HackRfController ()
{
    if (transmitting)
    {
        stopTransfer ();
    }
    free (hackrf_transfer_path);
}

void HackRfController::setGain (uint8_t newGain)
{
    gain = newGain;
}

uint8_t HackRfController::getGain () const
{
    return gain;
}

void HackRfController::runtimeChecks ()
{
    if (transmitting)
    {
        throw std::runtime_error ("Already transmitting.");
    }

    LOG_F (INFO, "Checking if hackrf_transfer exists...");
    // Verify hackrf_transfer exists. system () returns a non-zero value if it does not.
    if (system ("which hackrf_transfer"))
    {
        LOG_F (ERROR, "hackrf_tranfer unavailable!");
        throw std::runtime_error ("hackrf_tranfer unavailable!");
    }
    // Get the fully qualified path of the hackrf_transfer program.
    std::unique_ptr<FILE, decltype (&pclose)> pipe (popen ("which hackrf_transfer", "r"), pclose);
    if (!pipe)
    {
        std::string logString = "popen() failed! " + std::string (strerror (errno));
        LOG_F (ERROR, "%s", logString.c_str ());
        throw std::runtime_error (logString);
    }
    size_t blockSize = 0;
    size_t len = getline (&hackrf_transfer_path, &blockSize, pipe.get ());
    hackrf_transfer_path[len - 1] = '\0';

    LOG_F (INFO, "Checking if any HackRF boards are connected...");
    if (system ("hackrf_info") != 0)
    {
        LOG_F (ERROR, "No HackRF boards found!");
        throw std::runtime_error ("No HackRF boards found!");
    }

    // Verify arguments.
    if (!std::filesystem::exists (sim_file_path))
    {
        LOG_F (WARNING, "Simulation file has not been generated.");
        throw std::invalid_argument ("Simulation file has not been generated.");
    }
    if (gain > 47)
    {
        LOG_F (WARNING, "Gain must be between 0 and 47.");
        throw std::invalid_argument ("Gain must be between 0 and 47.");
    }
}

void HackRfController::startTransfer ()
{
    std::scoped_lock lock (startStopMutex);
    runtimeChecks ();

    LOG_F (INFO, "Resetting hackrf board.");
    if (system ("hackrf_spiflash -R") != 0)
    {
        LOG_F (ERROR, "Failed to reset hackrf board. Transmit may not function normally.");
    }
    // Let the board come back up.
    for (int i = 0; i < 30; i++)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for (1s);

        if (system ("hackrf_info > /dev/null") == 0)
        {
            break;
        }
    }

    LOG_F (INFO, "Starting hackrf_transfer.");

    pid_t pid;
    if ((pid = fork ()) < 0)
    {
        std::string logString = "fork() failed: " + std::string (strerror (errno));
        LOG_F (ERROR, "%s", logString.c_str ());
        throw std::runtime_error (logString);
    }
    // Semaphore that will signal the parent process once the child has spawned hackrf_transfer.
    sem_t* childCompleteSignal = sem_open ("child_complete", O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (childCompleteSignal == SEM_FAILED)
    {
        std::string logString = "sem_open() failed: " + std::string (strerror (errno));
        LOG_F (ERROR, "%s", logString.c_str ());
        throw std::runtime_error (logString);
    }

    if (pid == 0)
    {
        // Child process
        // Open shared semaphore in child process.
        sem_t* childSem = sem_open ("child_complete", O_RDWR);

        struct sched_attr
        {
            uint32_t size;              /* Size of this structure */
            uint32_t sched_policy;      /* Policy (SCHED_*) */
            uint64_t sched_flags;       /* Flags */
            int32_t sched_nice;        /* Nice value (SCHED_OTHER,
                                         SCHED_BATCH) */
            uint32_t sched_priority;    /* Static priority (SCHED_FIFO,
                                         SCHED_RR) */
            /* Remaining fields are for SCHED_DEADLINE */
            uint64_t sched_runtime;
            uint64_t sched_deadline;
            uint64_t sched_period;
        } attr{};

        // Get the current scheduling attributes for the process.
        syscall (SYS_sched_getattr, 0, &attr, sizeof (struct sched_attr), 0);
        // Set the process as real-time with the highest priority.
        attr.sched_policy = SCHED_FIFO;
        attr.sched_priority = 99;
        syscall (SYS_sched_setattr, 0, &attr, 0);

        const char* argv[] = { hackrf_transfer_path, "-t", sim_file_path.c_str (), "-f", "1575420000", "-s", "2600000",
                               "-a", "1", "-x", std::to_string (gain).c_str (), "-R",
                               nullptr };
        const char* envp[] = { nullptr };
        sem_post (childSem);
        sem_close (childSem);
        // Start hackrf_transfer.
        execve (hackrf_transfer_path, (char**) argv, (char**) envp);
        // Should not reach this point
        std::string logString = "execve() failed: " + std::string (strerror (errno));
        LOG_F (ERROR, "%s", logString.c_str ());
        throw std::runtime_error (logString);
    } else
    {
        // Parent process
        // Wait for hackrf_transfer to start, then begin the watchdog thread.
        sem_wait (childCompleteSignal);
        sem_close (childCompleteSignal);
        transmitting = true;
        watchdog_thread = std::thread (&HackRfController::watchdog_task, this, pid);
    }
    sem_unlink ("child_complete");
    emit startedTransmit ();
}

void HackRfController::stopTransfer ()
{
    std::scoped_lock lock (startStopMutex);
    if (transmitting)
    {
        LOG_F (INFO, "Requesting stop of hackrf_transfer.");
        transmitting = false;
        notifyEnd.notify_all ();
        watchdog_thread.join ();
        emit stoppedTransmit ();
    }
}

void HackRfController::watchdog_task (pid_t hackrf_transfer_pid)
{
    std::unique_lock<std::mutex> lock (conditionMutex);
    while (transmitting)
    {
        // Handle spurious wakeups.
        notifyEnd.wait (lock);
    }
    LOG_F (INFO, "Stopping hackrf_transfer.");
    kill (hackrf_transfer_pid, SIGINT);
    // Allow signal to propagate.
    sleep (1);
    if (kill (hackrf_transfer_pid, 0) == 0)
    {
        // If process won't end with SIGTERM, forcibly stop it.
        kill (hackrf_transfer_pid, SIGKILL);
    }
}

bool HackRfController::isTransmitting () const
{
    return transmitting;
}
