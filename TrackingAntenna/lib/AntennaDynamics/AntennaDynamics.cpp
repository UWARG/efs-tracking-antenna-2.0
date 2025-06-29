#include "Arduino.h"
#include "AntennaDynamics.hpp"
#include "../Config/Config.hpp"
#include "../Util/Util.hpp"
#include "../Communication/Communication.hpp"

AntennaDynamics::AntennaDynamics():
    initialAntennaAzimuth_ {0}
{
    
}

void AntennaDynamics::begin() {
    pitchServo_.attach(PITCH_PWM);
    yawServo_.attach(YAW_PWM);
}

void AntennaDynamics::manualSetup() { // don't use this when we have the compass again
    float currentAngle {0};

    PDEBUG("Beginning initial antenna azimuth calibration (north calibration). \nUse the 'a' and 'd' keys to rotate the antenna and space key when it is facing north. \n");

    while (true) {
        while (MANUAL_SERIAL.available() > 0) {
            char c = MANUAL_SERIAL.read();
            switch (c) {
                case 'a':
                    currentAngle -= 3;
                    if(currentAngle < YAW_END_ANGLE) {
                        currentAngle = YAW_END_ANGLE;
                    }
                    setYawAngle(currentAngle);
                    break;
                case 'd':
                    currentAngle += 3;
                    if(currentAngle > YAW_START_ANGLE) {
                        currentAngle = YAW_START_ANGLE;
                    }
                    setYawAngle(currentAngle);
                    break;
                case ' ':
                    setInitialAntennaAzimuth(yawAngle());
                    PDEBUG("Calibration complete, using angle: ");
                    PDEBUG(initialAntennaAzimuth_);
                    PDEBUG("\n");
                    return;
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
