#pragma once
#include <Arduino.h>
#include <DNSServer.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

namespace sets {

class ESP_DNS {
   public:
    void begin() {
#ifndef SETT_NO_DNS
        if (WiFi.getMode() == WIFI_AP || WiFi.getMode() == WIFI_AP_STA) {
            dns_f = true;
            dns.start(53, "*", WiFi.softAPIP());
        }
#endif
    }

    void tick() {
#ifndef SETT_NO_DNS
        if (dns_f) dns.processNextRequest();
#endif
    }

   private:
#ifndef SETT_NO_DNS
    DNSServer dns;
    bool dns_f = false;
#endif
};

}  // namespace sets