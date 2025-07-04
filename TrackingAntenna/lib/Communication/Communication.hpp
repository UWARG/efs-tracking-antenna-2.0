#include <WiFiUdp.h>
#include "../Config/Config.hpp"
#pragma once

class Communication {
    private:
        WiFiUDP UDP_;
        uint16_t port;

    public:
        uint8_t packetBuffer_[PACKET_BUFFER_SIZE]; // buffer to hold incoming packet

        Communication(uint16_t port);
        static bool beginWiFi();
        bool beginUDP();
        uint16_t parseUDP();
        void sendPacket(uint8_t* data, int size);
};