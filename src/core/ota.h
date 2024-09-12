#pragma once

#ifdef ESP8266
#include <Updater.h>
#else
#include <Update.h>
#endif

namespace sets {

bool beginOta(bool ota_flash = true, bool async = false);

}  // namespace sets