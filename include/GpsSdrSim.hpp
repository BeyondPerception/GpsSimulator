#ifndef GPS_SDR_SIM_CONTROLLER_HPP
#define GPS_SDR_SIM_CONTROLLER_HPP

class GpsSdrSim
{
public:
    static void generateGpsSimulation (const char* gps_sdr_sim_path, const char* brdc_file_path,
                                       double latitude, double longitude, uint8_t height, const char* outputPath);
};

#endif //GPS_SDR_SIM_CONTROLLER_HPP
