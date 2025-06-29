#include <WiFiUdp.h>
#include "../Config/Config.hpp"

class DronePosition {
   private:
    WiFiUDP UDP_;
    uint8_t packetBuffer_[PACKET_BUFFER_SIZE]; //buffer to hold incoming packet
    float latitude_;
    float longitude_;
    float altitude_;

    uint16_t parseUDP();

   public:
    DronePosition();
    bool beginWiFi();
    bool beginUDP();
    bool getPosition();
    float latitude();
    float longitude();
    float altitude();
};