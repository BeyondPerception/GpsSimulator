#include <filesystem>

#include "loguru.hpp"
#include "GpsSdrSim.hpp"

void GpsSdrSim::generateGpsSimulation (const char* gps_sdr_sim_path, const char* brdc_file_path,
                                                 double latitude, double longitude, uint8_t height,
                                                 const char* outputPath)
{
    if (!std::filesystem::exists (gps_sdr_sim_path))
    {
        LOG_F (ERROR, "Invalid gps-sim-sdr path.");
        throw std::invalid_argument ("Invalid gps-sim-sdr path.");
    }
    if (!std::filesystem::exists (brdc_file_path))
    {
        LOG_F (ERROR, "Invalid BRDC file path.");
        throw std::invalid_argument ("Invalid BRDC file path.");
    }
    if (latitude < -90 || latitude > 90)
    {
        LOG_F (ERROR, "Invalid latitude coordinate.");
        throw std::invalid_argument ("Invalid latitude coordinate.");
    }
    if (longitude < -180 || longitude > 180)
    {
        LOG_F (ERROR, "Invalid longitude coordinate.");
        throw std::invalid_argument ("Invalid longitude coordinate.");
    }
    std::string gpsSimCommand = std::string (gps_sdr_sim_path) + " -e " + std::string (brdc_file_path) + " -b 8 -l " +
                               std::to_string (latitude) + "," + std::to_string (longitude) + "," +
                               std::to_string (height) + " -o " + std::string (outputPath);

    if (system (gpsSimCommand.c_str ()) != 0)
    {
        LOG_F (ERROR, "Failed to generate GPS Simulation file.");
        throw std::runtime_error ("Failed to generate GPS Simulation file.");
    }
}
