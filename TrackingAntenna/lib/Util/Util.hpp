#define EARTH_RADIUS 6372797.56085

bool myDelay(int delayMilliseconds);

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
inline double toRadians(double degrees);
inline double toDegrees(double radians);

float calculateDistance(float lat1, float lon1, float lat2, float lon2);
double calculateAzimuth(double baseLat, double baseLon, double droneLat, double droneLon);
double calculateElevation(double distance, double baseAlt, double droneAlt);
