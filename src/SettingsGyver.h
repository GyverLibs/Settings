// реализация на GyverHTTP

#pragma once
#include "SettingsT.h"

class SettingsGyver : public SettingsT<WiFiServer, WiFiClient> {
   public:
    using SettingsT<WiFiServer, WiFiClient>::SettingsT;
};