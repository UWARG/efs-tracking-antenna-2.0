#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

class AntennaPosition {
   private:
    SFE_UBLOX_GNSS GPS_;
    float latitude_;
    float longitude_;
    float altitude_;

   public:
    bool beginGPS();
    bool getGPSPosition();
    float latitude();
    float longitude();
    float altitude();
    float azimuth();
};
