#pragma once

#ifdef ESP8266
#include <Updater.h>
#else
#include <Update.h>
#endif

namespace sets {
size_t OtaGetMaxSize(bool ota_flash = true);
bool beginOta(bool ota_flash = true, bool async = false);

}  // namespace sets