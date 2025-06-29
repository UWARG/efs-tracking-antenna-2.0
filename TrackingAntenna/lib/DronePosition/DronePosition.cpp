#include "DronePosition.hpp"
#include <MAVLink.h>
// #include <Arduino_FreeRTOS.h>
#include <WiFi.h>

DronePosition::DronePosition():
    UDP_ {},
    packetBuffer_ {},
    latitude_ {0},
    longitude_ {0},
    altitude_ {0}
{

}

bool DronePosition::beginWiFi() {
    #ifdef WIFI
        // WiFi.setPins(WIFI_SPI_CS, WIFI_SPI_ACK, WIFI_RESETN, WIFI_GPIO0, &WIFI_SPI);
        // check for the WiFi module:
        WiFi.mode(WIFI_STA);
        if (WiFi.status() == WL_NO_SHIELD) {
            PDEBUG("Communication with WiFi module failed! \n");
            return false;
        } else {
            PDEBUG("Communication with WiFi module succeeded! \n");
            WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
            WiFi.begin(WIFI_SSID, WIFI_PASS);
            return true;
        }

    #else
        PDEBUG("Can't begin WiFi as WiFi is disabled, continuing... \n");
        return true;
    #endif
}

bool DronePosition::beginUDP() {
    #ifdef WIFI
        if (WiFi.status() != WL_CONNECTED) {
            // attempt to connect to Wifi network:
            PDEBUG("Attempting to connect to SSID: ");
            PDEBUG(WIFI_SSID);
            PDEBUG("\n");
            return false;
        } else {
            PDEBUG("Connected to WiFi \n");
            // print the SSID of the network you're attached to:
            PDEBUG("SSID: ");
            PDEBUG(WiFi.SSID());
            PDEBUG("\n");

            // print your board's IP address:
            IPAddress ip = WiFi.localIP();
            PDEBUG("IP Address: ");
            PDEBUG(ip);
            PDEBUG("\n");

            // print the received signal strength:
            long rssi = WiFi.RSSI();
            PDEBUG("Signal strength (RSSI): ");
            PDEBUG(rssi);
            PDEBUG(" dBm\n");

            PDEBUG("Starting UDP connection at port: ");
            PDEBUG(LOCAL_PORT);
            PDEBUG("\n");
            // if you get a connection, report back via serial:
            UDP_.begin(LOCAL_PORT);
            return true;
        }
    #else
        PDEBUG("Can't connect to WiFi as WiFi is disabled, continuing... \n");
        return true;
    #endif
}

uint16_t DronePosition::parseUDP() {
    // if there's data available, read a packet
    int packetSize = UDP_.parsePacket();
    if (packetSize) {
        PDEBUG("Received packet of size ");
        PDEBUG(packetSize);
        PDEBUG("\nFrom ");
        PDEBUG(UDP_.remoteIP());
        PDEBUG(", port ");
        PDEBUG(UDP_.remotePort());
        PDEBUG("\n");

        // read the packet into packetBufffer
        uint16_t messageLength = UDP_.read(packetBuffer_, PACKET_BUFFER_SIZE);
        PDEBUG("Contents:\n");
        for (uint16_t i{0};  i < messageLength; ++i) {
            char c = packetBuffer_[i];
            PDEBUG(c);
        }
        PDEBUG("\n");

        return messageLength;
    }
    return 0;
}

bool DronePosition::getPosition() {
    #ifdef WIFI
        bool retValue {false};
        mavlink_message_t msg;
        mavlink_status_t status;

        uint16_t messageLength = parseUDP();
        for (uint16_t i{0};  i < messageLength; ++i) {
            uint8_t c = packetBuffer_[i];

            if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
                if (msg.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT) {
                    mavlink_global_position_int_t position;
                    mavlink_msg_global_position_int_decode(&msg, &position);

                    latitude_ = position.lat / 1e7;
                    PDEBUG(F("Drone Lat: "));
                    PDEBUG(latitude_);

                    longitude_ = position.lon / 1e7;
                    PDEBUG(F(" Drone Long: "));
                    PDEBUG(longitude_);
                    PDEBUG(F(" (degrees)"));

                    altitude_ = position.alt / 1e3;
                    PDEBUG(F(" Drone Alt: "));
                    PDEBUG(altitude_);
                    PDEBUG(F(" (m) \n"));

                    retValue = true;
                }
            }
        }
        
        return retValue;

    #else
        latitude_ = DRONE_LATITUDE;
        PDEBUG(F("Drone Lat: "));
        PDEBUG(latitude_);

        longitude_ = DRONE_LONGITUDE;
        PDEBUG(F(" Drone Long: "));
        PDEBUG(longitude_);
        PDEBUG(F(" (degrees)"));

        altitude_ = DRONE_ALTITUDE;
        PDEBUG(F(" Drone Alt: "));
        PDEBUG(altitude_);
        PDEBUG(F(" (m) \n"));

        return true;
    #endif
}

// void static startTask(void *pvParameters) { keeping for if we switch back to FreeRTOS
//     auto* context = static_cast<DronePosition*>(pvParameters);
//     while(1) {
//         context->getPosition();
//         vTaskDelay()
//     }
// }

float DronePosition::latitude() {
    return latitude_;
}

float DronePosition::longitude() {
    return longitude_;
}

float DronePosition::altitude() {
    return altitude_;
}