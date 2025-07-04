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
    float currentAngle {0};

    String welcomeMsg = "Beginning initial antenna azimuth calibration (north calibration). \nUse the 'a' and 'd' keys to rotate the antenna and space key when it is facing north. \n";
    PDEBUG(welcomeMsg);

    while (true) {
        int msglen = comm_.parseUDP();
        if (msglen > 0) {
            for (int i = 0; i < msglen; ++i) {
                char c = comm_.packetBuffer_[i];
                String left_msg = "Turning left. Yaw angle: ";
                String right_msg = "Turning right. Yaw angle: ";
                String completeMsg = "Calibration complete, using angle: ";
                switch (c) {
                    case 'a':
                        currentAngle -= 3;
                        if(currentAngle < YAW_END_ANGLE) {
                            currentAngle = YAW_END_ANGLE;
                        }
                        PDEBUG("char a detected. \n");
                        left_msg += String(currentAngle) + "\n";
                        comm_.sendPacket((uint8_t*)left_msg.c_str(), strlen(left_msg.c_str()));
                        setYawAngle(currentAngle);
                        break;
                    case 'd':
                        currentAngle += 3;
                        if(currentAngle > YAW_START_ANGLE) {
                            currentAngle = YAW_START_ANGLE;
                        }
                        PDEBUG("char d detected. \n");
                        right_msg += String(currentAngle) + "\n";
                        comm_.sendPacket((uint8_t*)right_msg.c_str(), strlen(right_msg.c_str()));
                        setYawAngle(currentAngle);
                        break;
                    case ' ':
                        setInitialAntennaAzimuth(yawAngle());
                        PDEBUG("Calibration complete, using angle: ");
                        PDEBUG(initialAntennaAzimuth_);
                        PDEBUG("\n");
                        completeMsg += String(initialAntennaAzimuth_) + "\n";
                        comm_.sendPacket((uint8_t*)completeMsg.c_str(), strlen(completeMsg.c_str()));
                        return;
                }
            }
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
    
    int pitchMicroseconds = map(angle, PITCH_START_ANGLE, PITCH_END_ANGLE, PITCH_START_MICROSECONDS, PITCH_END_MICROSECONDS);

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
