// #include "ESP32Servo.h"
#include "s3servo.hpp"
#include "Communication.hpp"

class AntennaDynamics {
   private:

    float initialAntennaAzimuth_;
    Communication comm_;
    s3servo pitchServo_;
    s3servo yawServo_;

   public:
    AntennaDynamics();
    void begin();
    bool beginUDP();
    void manualSetup(); // until compass is installed
    void initalizeAzimuth(float azimuth);
    float yawAngle();
    void setInitialAntennaAzimuth(float angle);
    bool setAzimuth(float azimuth);
    bool setYawAngle(float angle);
    bool setElevation(float elevation);
    bool setPitchAngle(float angle);
};
