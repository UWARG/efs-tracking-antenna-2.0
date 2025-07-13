#include <WiFi.h>
#include "Communication.hpp"

Communication::Communication(uint16_t port):
    port {port},
    UDP_ {},
    packetBuffer_ {}
{

}

bool Communication::beginWiFi() {
    #ifdef WIFI
        // WiFi.setPins(WIFI_SPI_CS, WIFI_SPI_ACK, WIFI_RESETN, WIFI_GPIO0, &WIFI_SPI);
        // check for the WiFi module:
        WiFi.mode(WIFI_STA);
        if (WiFi.status() == WL_NO_SHIELD) {
            PDEBUG("Communication with WiFi module failed! \n");
            return false;
        } else {
            PDEBUG("Communication with WiFi module succeeded! \n");
            // WiFi.mode(WIFI_STA);
            WiFi.disconnect();
            // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
            WiFi.begin(WIFI_SSID, WIFI_PASS);
            // return true;
        }
        delay(1000); // wait for connection to establish
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
            return true;
        }


    #else
        PDEBUG("Can't begin WiFi as WiFi is disabled, continuing... \n");
        return true;
    #endif
}

bool Communication::beginUDP() {
    #ifdef WIFI
        if (WiFi.status() != WL_CONNECTED) {
            return false;
        } else {
            PDEBUG("Starting UDP connection at port: ");
            PDEBUG(port);
            PDEBUG("\n");
            // if you get a connection, report back via serial:
            UDP_.begin(port);
            return true;
        }
    #else
        PDEBUG("Can't connect to WiFi as WiFi is disabled, continuing... \n");
        return true;
    #endif
}

uint16_t Communication::parseUDP() {
    // if there's data available, read a packet
    int packetSize = UDP_.parsePacket();
    if (packetSize) {
        // PDEBUG("Received packet of size ");
        // PDEBUG(packetSize);
        // PDEBUG("\nFrom ");
        // PDEBUG(UDP_.remoteIP());
        // PDEBUG(", port ");
        // PDEBUG(UDP_.remotePort());
        // PDEBUG("\n");

        // read the packet into packetBufffer
        uint16_t messageLength = UDP_.read(packetBuffer_, PACKET_BUFFER_SIZE);
        // PDEBUG("Contents:\n");
        // for (uint16_t i{0};  i < messageLength; ++i) {
        //     char c = packetBuffer_[i];
        //     PDEBUG(c);
        // }
        // PDEBUG("\n");

        return messageLength;
    }
    return 0;
}

void Communication::sendPacket(uint8_t *data, int size) {
    // send a reply, to the IP address and port that sent us the packet we received
    UDP_.beginPacket(UDP_.remoteIP(), port);
    PDEBUG("Sending packet to ");
    PDEBUG(UDP_.remoteIP());
    PDEBUG(" at port ");
    PDEBUG(port);
    PDEBUG("\n");
    UDP_.write(data,size);
    UDP_.endPacket();
}