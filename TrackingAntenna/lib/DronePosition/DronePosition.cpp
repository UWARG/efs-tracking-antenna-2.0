#include "DronePosition.hpp"
#include <MAVLink.h>
// #include <Arduino_FreeRTOS.h>
#include <WiFi.h>

DronePosition::DronePosition():
    // UDP_ {},
    // packetBuffer_ {},
    comm_ {LOCAL_PORT},
    latitude_ {0},
    longitude_ {0},
    altitude_ {0}
{

}

bool DronePosition::beginUDP() {
    return comm_.beginUDP();
}

bool DronePosition::getPosition() {
    #ifdef WIFI
        bool retValue {false};
        mavlink_message_t msg;
        mavlink_status_t status;

        uint16_t messageLength = comm_.parseUDP();
        for (uint16_t i{0};  i < messageLength; ++i) {
            uint8_t c = comm_.packetBuffer_[i];

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