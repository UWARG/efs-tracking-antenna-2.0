#include "ESP32Servo.h"

class AntennaDynamics {
   private:
    Servo pitchServo_;
    Servo yawServo_;
    float initialAntennaAzimuth_;

   public:
    AntennaDynamics();
    void begin();
    void manualSetup(); // until compass is installed
    void initalizeAzimuth(float azimuth);
    float yawAngle();
    void setInitialAntennaAzimuth(float angle);
    bool setAzimuth(float azimuth);
    bool setYawAngle(float angle);
    bool setElevation(float elevation);
    bool setPitchAngle(float angle);
};
