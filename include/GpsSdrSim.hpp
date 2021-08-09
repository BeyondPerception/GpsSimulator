#ifndef GPS_SDR_SIM_CONTROLLER_HPP
#define GPS_SDR_SIM_CONTROLLER_HPP

class GpsSdrSim
{
public:
    static void generateGpsSimulation (const std::string& gps_sdr_sim_path, const std::string& brdc_file_path,
                                       double latitude, double longitude, uint16_t height, uint16_t duration,
                                       const std::string& outputPath);

    /**
     * @param simFilePath Path to simulation file to move
     * @param ramDiskPath Path to ramdisk directory. Requires no trailing `/`
     */
    static void moveSimFileToRamDisk (const std::string& simFilePath, const std::string& ramDiskPath);
};

#endif //GPS_SDR_SIM_CONTROLLER_HPP
