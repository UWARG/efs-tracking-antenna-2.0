#include "Arduino.h"
#include "AntennaDynamics.hpp"
#include "../Config/Config.hpp"
#include "../Util/Util.hpp"

AntennaDynamics::AntennaDynamics():
    comm_ {CONTROL_PORT},
    initialAntennaAzimuth_ {0},
    pitchServo_{},
    yawServo_{}
{
    
}

void AntennaDynamics::begin() {
    if (pitchServo_.attach(PITCH_PWM,0,PITCH_START_ANGLE,PITCH_END_ANGLE,s3servo::microsecondsToDuty(PITCH_START_MICROSECONDS),s3servo::microsecondsToDuty(PITCH_END_MICROSECONDS))!=-1){
        PDEBUG("attach pitch servo success\n");
    }else {
        PDEBUG("attach pitch servo fail\n");
    }
    if (yawServo_.attach(YAW_PWM,1,YAW_START_ANGLE,YAW_END_ANGLE,s3servo::microsecondsToDuty(YAW_START_MICROSECONDS),s3servo::microsecondsToDuty(YAW_END_MICROSECONDS))!=-1){
        PDEBUG("attach yaw servo success\n");
    }else {
         PDEBUG("attach yaw servo fail\n");
    }
}

bool AntennaDynamics::beginUDP() {
    return comm_.beginUDP();
}

void AntennaDynamics::manualSetup() { // don't use this when we have the compass again
    setPitchAngle(0);
    setYawAngle(0);

    while(comm_.parseUDP() <= 0) {
        PDEBUG("Waiting for incoming packet to determine ip address\n");\
        delay(1000);
    }

    String welcomeMsg = "Beginning initial antenna azimuth calibration (north calibration). \nPlease input the bearing of the antenna\n";
    PDEBUG(welcomeMsg);
    comm_.sendPacket((uint8_t*)welcomeMsg.c_str(), welcomeMsg.length());


    while (true) {
        int msglen = comm_.parseUDP();
        if (msglen > 0) {
            char angleArr[msglen + 1]; 
            for (int i = 0; i < msglen; ++i) {
                char c = comm_.packetBuffer_[i];
                angleArr[i] = c;
            }
            angleArr[msglen] = '\0';
            float angle = atof(angleArr);
            // Converting compass bearing to angle from antenna (range -180 to 180 degrees) to match with previous implementation
            // if (angle < 0 || angle > 360) {
            //     String errorMsg = "Invalid angle. Please enter a value between 0 and 360 degrees.\n";
            //     PDEBUG(errorMsg);
            //     comm_.sendPacket((uint8_t*)errorMsg.c_str(), errorMsg.length());
            //     continue;
            // } else if (angle < 180){
            //     angle = -angle; 
            // } else if (angle > 180) {
            //     angle = 360 - angle;
            // }
            setInitialAntennaAzimuth(angle);
            String response = "Initial antenna azimuth set to: " + String(initialAntennaAzimuth_) + " degrees.\n";
            PDEBUG(response);
            comm_.sendPacket((uint8_t*)response.c_str(), response.length());
            break;
        }
    }
}

void AntennaDynamics::initalizeAzimuth(float azimuth) { // untested function (can't be tested until compass is installed)
    initialAntennaAzimuth_ = azimuth - yawAngle();
}

float AntennaDynamics::yawAngle() {
    return mapFloat(yawServo_.readMicroseconds(), YAW_END_MICROSECONDS, YAW_START_MICROSECONDS, YAW_END_ANGLE, YAW_START_ANGLE);
}

void AntennaDynamics::setInitialAntennaAzimuth(float angle) {
    initialAntennaAzimuth_ = angle;
}

bool AntennaDynamics::setAzimuth(float azimuth) {
    azimuth = fmod((azimuth + 360), 360);

    float angle = azimuth - initialAntennaAzimuth_;

    PDEBUG("Azimuth: ");
    PDEBUG(azimuth);
    PDEBUG("\n");

    PDEBUG("Initial Antenna Azimuth: ");
    PDEBUG(initialAntennaAzimuth_);
    PDEBUG("\n");

    return setYawAngle(angle);
}

// Function to set the yaw angle of the tracker in degrees
bool AntennaDynamics::setYawAngle(float angle) {
    angle = fmod((angle + 360), 360);

    if (angle > 180) {
        angle = angle - 360;
    } else if (angle < -180) {
        angle = angle + 360;
    }

    if (angle > YAW_START_ANGLE) {
        angle = YAW_START_ANGLE;
    } else if (angle < YAW_END_ANGLE) {
        angle = YAW_END_ANGLE;
    }

    int yawMicroseconds = map(angle, YAW_END_ANGLE, YAW_START_ANGLE, YAW_END_MICROSECONDS, YAW_START_MICROSECONDS);

    if (yawMicroseconds > YAW_END_MICROSECONDS || yawMicroseconds < YAW_START_MICROSECONDS) {
        return false;
    }

    yawServo_.writeMicroseconds(yawMicroseconds);

    PDEBUG("Yaw Angle: ");
    PDEBUG(angle);
    PDEBUG("\n");

    PDEBUG("Yaw Microseconds: ");
    PDEBUG(yawMicroseconds);
    PDEBUG("\n");

    return true;
}

bool AntennaDynamics::setElevation(float elevation) {
    PDEBUG("Elevation: ");
    PDEBUG(elevation);
    PDEBUG("\n");

    return setPitchAngle(elevation);
}

// Function to set the pitch angle of the tracker in degrees
bool AntennaDynamics::setPitchAngle(float angle) {
    if (angle > PITCH_END_ANGLE) {
        angle = PITCH_END_ANGLE;
    } else if (angle < PITCH_START_ANGLE) {
        angle = PITCH_START_ANGLE;
    }

    float pitchAngle = 90 - angle;
    
    int pitchMicroseconds = map(pitchAngle, PITCH_START_ANGLE, PITCH_END_ANGLE, PITCH_START_MICROSECONDS, PITCH_END_MICROSECONDS);

    if (pitchMicroseconds > PITCH_END_MICROSECONDS || pitchMicroseconds < PITCH_START_MICROSECONDS) {
        return false;
    }

    pitchServo_.writeMicroseconds(pitchMicroseconds);

    PDEBUG("Pitch Angle: ");
    PDEBUG(angle);
    PDEBUG("\n");

    PDEBUG("Pitch Microseconds: ");
    PDEBUG(pitchMicroseconds);
    PDEBUG("\n");

    return true;
}
