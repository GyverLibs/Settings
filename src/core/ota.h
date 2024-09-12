#pragma once

#ifndef ESP8266
#include <Update.h>
#endif

namespace sets {

bool beginOta(bool ota_flash = true, bool async = false);

}  // namespace sets