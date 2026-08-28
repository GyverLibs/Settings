#include "ota.h"

#include <Arduino.h>
#include <FS.h>

#ifdef ESP8266
#include <flash_hal.h>
#endif

namespace sets {
size_t OtaGetMaxSize(bool ota_flash)
{
  size_t ota_size = 0;
  if (ota_flash) {
#ifdef ESP8266
    ota_size = (size_t)((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000);
#else
    ota_size = UPDATE_SIZE_UNKNOWN;
#endif
  } else {
#ifdef ESP8266
    close_all_fs();
    ota_size = (size_t)&_FS_end - (size_t)&_FS_start;
#else
    ota_size = UPDATE_SIZE_UNKNOWN;
#endif
  }
  return ota_size;
}

bool beginOta(bool ota_flash, bool async) {
    size_t ota_size = OtaGetMaxSize(ota_flash);
    int ota_type = 0;

    if (ota_flash) {
        ota_type = U_FLASH;
    } else {
#ifdef ESP8266
        ota_type = U_FS;
        close_all_fs();
#else
        ota_type = U_SPIFFS;
#endif
    }

#ifdef ESP8266
    if (async) Update.runAsync(true);
#endif
    return Update.begin(ota_size, ota_type);
}

}  // namespace sets
