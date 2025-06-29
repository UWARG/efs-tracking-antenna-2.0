#include <Arduino.h>
#include "DronePosition.hpp"
#include "AntennaPosition.hpp"
#include "AntennaDynamics.hpp"
#include "Util.hpp"
#include "Config.hpp"

AntennaPosition antennaPos{};
AntennaDynamics antennaDyn{};
DronePosition   dronePos{};

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    while(!Serial);

    antennaDyn.begin();

    while (!dronePos.beginWiFi()) {
        PDEBUG("Could not connect to WiFi, retrying... \n");
        delay(1000);
    }
    
    while (!dronePos.beginUDP()) {
        PDEBUG("Attempting to connect to WiFi and initialize UDP, this may take a bit... \n");
        delay(10000);
    }

    while (!antennaPos.beginGPS()) {
        PDEBUG("Could not connect to GPS, retrying... \n");
        delay(1000);
    }

    while (!antennaPos.getGPSPosition()) {
        PDEBUG("Not enough satellites found, retrying... \n"); 
        delay(1000);
    }

    #ifndef COMPASS
        antennaDyn.manualSetup(); // until compass is installed
    #else
        antennaDyn.initializeAzimuth(antennaPos.azimuth()); // Once compass is installed
    #endif

}

void runAntenna() {
    // More about the Azimuth/Elevation coordinate system https://en.wikipedia.org/wiki/Horizontal_coordinate_system
    float antennaToDroneDistance = calculateDistance(antennaPos.latitude(), antennaPos.longitude(), dronePos.latitude(), dronePos.longitude());
    float antennaToDroneAzimuth = calculateAzimuth(antennaPos.latitude(), antennaPos.longitude(), dronePos.latitude(), dronePos.longitude());
    float antennaToDroneElevation = calculateElevation(antennaToDroneDistance, antennaPos.altitude(), dronePos.altitude());
    antennaDyn.setAzimuth(antennaToDroneAzimuth);
    antennaDyn.setElevation(antennaToDroneElevation);
}

static unsigned long lastMillisGetPosition {0};
static unsigned long lastMillisRunAntenna {0};
void loop() {
    // put your main code here, to run repeatedly:
    if (millis() - lastMillisGetPosition > 10) {
        lastMillisGetPosition = millis();
        dronePos.getPosition();
    }

    if (millis() - lastMillisRunAntenna > 1000) {
        lastMillisRunAntenna = millis();        
        runAntenna();
    }
}