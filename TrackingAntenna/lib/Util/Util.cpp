#include "Util.hpp"
// #include <cmath> // for seeeduino
#include <math.h> // for arduino mega
#include "Arduino.h"

bool myDelay(int delayMilliseconds) { // keeping this here for now as if we reintroduce FreeRTOS this will be useful, remove once we know we're never going to use FreeRTOS
    uint32_t lastMillis {millis()};
    while (millis() - lastMillis > delayMilliseconds) {
        
    }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline double toRadians(double degrees) {
    return degrees * (M_PI / 180);
}

inline double toDegrees(double radians) {
    return radians * (180 / M_PI);
}

//https://stackoverflow.com/questions/27126714/c-latitude-and-longitude-distance-calculator
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    lat1  = toRadians(lat1);
    lon1 = toRadians(lon1);
    lat2  = toRadians(lat2);
    lon2 = toRadians(lon2);
    double haversine = (pow(sin((1.0 / 2) * (lat2 - lat1)), 2)) + ((cos(lat1)) * (cos(lat2)) * (pow(sin((1.0 / 2) * (lon2 - lon1)), 2)));
    double temp = 2 * asin(min(1.0, sqrt(haversine)));
    float point_dist = EARTH_RADIUS * temp;
    return point_dist;
}

//https://stackoverflow.com/questions/21060891/android-how-can-i-get-the-bearing-degree-between-two-locations
double calculateAzimuth(double baseLat, double baseLon, double droneLat, double droneLon) {
    double startLat = toRadians(baseLat);
    double startLon = toRadians(baseLon);
    double endLat = toRadians(droneLat);
    double endLon = toRadians(droneLon);

    double dLon = endLon - startLon;

    double x = sin(dLon) * cos(endLat);
    double y = cos(startLat) * sin(endLat) - sin(startLat) * cos(endLat) * cos(dLon);
    double initialAzimuth = atan2(x, y);

    // Convert azimuth from radians to degrees
    double azimuth = toDegrees(initialAzimuth);

    // Normalize the azimuth
    azimuth = fmod((azimuth + 360), 360);

    return azimuth;
}

double calculateElevation(double distance, double baseAlt, double droneAlt) {
    double elevationAngle = atan((droneAlt-baseAlt)/distance);

    return toDegrees(elevationAngle);
}

