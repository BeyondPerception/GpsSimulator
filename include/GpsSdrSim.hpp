#ifndef GPS_SDR_SIM_CONTROLLER_HPP
#define GPS_SDR_SIM_CONTROLLER_HPP

class GpsSdrSim
{
public:
    static void generateGpsSimulation (const std::string& gps_sdr_sim_path, const std::string& brdc_file_path,
                                       double latitude, double longitude, uint16_t height, uint16_t duration,
                                       const std::string& outputPath);
};

#endif //GPS_SDR_SIM_CONTROLLER_HPP
