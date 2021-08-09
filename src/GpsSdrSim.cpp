#include <filesystem>

#include "loguru.hpp"
#include "GpsSdrSim.hpp"

void GpsSdrSim::generateGpsSimulation (const std::string& gps_sdr_sim_path, const std::string& brdc_file_path,
                                       double latitude, double longitude, uint16_t height, uint16_t duration,
                                       const std::string& outputPath)
{
    if (!std::filesystem::exists (gps_sdr_sim_path))
    {
        LOG_F (ERROR, "Invalid gps-sim-sdr path.");
        return;
    }
    if (!std::filesystem::exists (brdc_file_path))
    {
        LOG_F (ERROR, "Invalid BRDC file path.");
        return;
    }
    if (latitude < -90 || latitude > 90)
    {
        LOG_F (ERROR, "Invalid latitude coordinate.");
        return;
    }
    if (longitude < -180 || longitude > 180)
    {
        LOG_F (ERROR, "Invalid longitude coordinate.");
        return;
    }
    std::string gpsSimCommand =
        gps_sdr_sim_path + " -e " + brdc_file_path + " -b 8 -l " + std::to_string (latitude) + "," +
        std::to_string (longitude) + "," + std::to_string (height) + " -d " + std::to_string (duration) + " -o " +
        outputPath;

    if (system (gpsSimCommand.c_str ()) != 0)
    {
        LOG_F (ERROR, "Failed to generate GPS Simulation file.");
        return;
    }
    std::string homeDir = getenv ("HOME");
    try
    {
        moveSimFileToRamDisk (outputPath, homeDir + "/ramdisk");
    } catch (const std::filesystem::filesystem_error& e)
    {
        LOG_F (ERROR, "Failed to move sim file to ramdisk: %s", e.what ());
    }
}

void GpsSdrSim::moveSimFileToRamDisk (const std::string& simFilePath, const std::string& ramDiskPath)
{
    if (std::filesystem::exists (simFilePath) && std::filesystem::exists (ramDiskPath))
    {
        std::filesystem::path simFile (simFilePath);
        std::filesystem::copy_file (simFile, ramDiskPath + "/" + simFile.filename ().string ());
    }
}
