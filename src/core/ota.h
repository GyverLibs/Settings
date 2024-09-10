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

bool beginOta(bool ota_flash = true, bool async = false);

}  // namespace sets