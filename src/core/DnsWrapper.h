#pragma once
#include <Arduino.h>
#include <DNSServer.h>

#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#else
#include <ESPmDNS.h>
#include <WiFi.h>
#endif

namespace sets {

class DnsWrapper {
   public:
    void begin(bool captive, const char* domain) {
#ifndef SETT_NO_DNS
        if (captive) {
            captF = true;
            dns.start(53, "*", WiFi.softAPIP());
        }
        if (domain) {
            if (localF) MDNS.end();
            localF = true;
            MDNS.begin(domain);
        }
#endif
    }

    void stop() {
#ifndef SETT_NO_DNS
        dns.stop();
        MDNS.end();
        localF = false;
        captF = false;
#endif
    }

    void tick() {
#ifndef SETT_NO_DNS
        if (captF) dns.processNextRequest();
#ifdef ESP8266
        if (localF) MDNS.update();
#endif
#endif
    }

   private:
#ifndef SETT_NO_DNS
    DNSServer dns;
    bool captF = false;
    bool localF = false;
#endif
};

}  // namespace sets