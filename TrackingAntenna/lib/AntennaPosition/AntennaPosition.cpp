#include "AntennaPosition.hpp"
#include <Wire.h>
#include "../Config/Config.hpp"

bool AntennaPosition::beginGPS() {
    #ifdef GPS
        PDEBUG("Waiting for GPS fix... \n");

        Wire.begin();

        if (GPS_.begin() == false) //Connect to the u-blox module using Wire port
        {
            PDEBUG(F("u-blox GPS not detected. Retrying... \n"));
            return false;
        }

        GPS_.setI2COutput(COM_TYPE_UBX | COM_TYPE_NMEA); //Set the I2C port to output both NMEA and UBX messages
        GPS_.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); //Save (only) the communications port settings to flash and BBR
        
        // these were with the old code, don't know how to implement these yet
        /* GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
        GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
        GPS.sendCommand(PGCMD_ANTENNA); */
        PDEBUG("GPS: connected through I2C \n");

        return true;

    #else
        PDEBUG("GPS Disabled, using hard coded values \n");
        return true;
    #endif
}

bool AntennaPosition::getGPSPosition() {
    #ifdef GPS
        latitude_ = GPS_.getLatitude() / 10000000.0;;
        PDEBUG(F("Lat: "));
        PDEBUG(latitude_);

        longitude_ = GPS_.getLongitude() / 10000000.0;
        PDEBUG(F(" Long: "));
        PDEBUG(longitude_);
        PDEBUG(F(" (degrees)"));

        altitude_ = GPS_.getAltitudeMSL() / 1000.0; // Altitude above Mean Sea Level
        PDEBUG(F(" Alt: "));
        PDEBUG(altitude_);
        PDEBUG(F(" (m)"));

        byte SIV = GPS_.getSIV();
        PDEBUG(F(" SIV: "));
        PDEBUG(SIV);

        PDEBUG("\n");

        return SIV > MIN_SATELLITES;
    
    #else
        latitude_ = ANTENNA_LATITUDE;
        PDEBUG(F("Lat: "));
        PDEBUG(latitude_);

        longitude_ = ANTENNA_LONGITUDE;
        PDEBUG(F(" Long: "));
        PDEBUG(longitude_);
        PDEBUG(F(" (degrees)"));

        altitude_ = ANTENNA_LATITUDE;
        PDEBUG(F(" Alt: "));
        PDEBUG(altitude_);
        PDEBUG(F(" (m)"));

        PDEBUG(F(" SIV: "));
        PDEBUG("No antennas, GPS disabled");

        PDEBUG("\n");

        return true;
    #endif
}

float AntennaPosition::latitude() {
    return latitude_;
}

float AntennaPosition::longitude() {
    return longitude_;
}

float AntennaPosition::altitude() {
    return altitude_;
}
