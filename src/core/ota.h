#pragma once
#include <Arduino.h>
#include <FS.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <Updater.h>
#else
#include <Update.h>
#include <WiFi.h>
#endif

namespace sets {

bool beginOta(bool ota_flash = true, bool async = false) {
    size_t ota_size = 0;
    int ota_type = 0;

    if (ota_flash) {
        ota_type = U_FLASH;
#ifdef ESP8266
        ota_size = (size_t)((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
#else
        ota_size = UPDATE_SIZE_UNKNOWN;
#endif
    } else {
#ifdef ESP8266
        ota_type = U_FS;
        close_all_fs();
#ifndef GH_NO_FS
        ota_size = (size_t)&_FS_end - (size_t)&_FS_start;
#endif
#else
        ota_type = U_SPIFFS;
        ota_size = UPDATE_SIZE_UNKNOWN;
#endif
    }
    
#ifdef ESP8266
    if (async) Update.runAsync(true);
#endif
    return Update.begin(ota_size, ota_type);
}

}  // namespace sets