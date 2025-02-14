#include "sets.h"

#include <LittleFS.h>
#include <WiFiConnector.h>

GyverDBFile db(&LittleFS, "/data.db");
SettingsGyver sett(PROJECT_NAME, &db);

// ========== build ==========
static void build(sets::Builder& b) {
    {
        sets::Group g(b, "WiFi");
        b.Input(kk::wifi_ssid, "SSID");
        b.Pass(kk::wifi_pass, "Pass", "");
        if (b.Switch(kk::close_ap, "Закрывать AP")) {
            WiFiConnector.closeAP(db[kk::close_ap]);
        }
        if (b.Button("Подключить")) {
            WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
        }
    }
}

// ========== update ==========
static void update(sets::Updater& u) {
}

// ========== begin ==========
void sett_begin() {
    // fs
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    // database
    db.begin();
    db.init(kk::wifi_ssid, "");
    db.init(kk::wifi_pass, "");
    db.init(kk::close_ap, true);

    // wifi
    WiFiConnector.onConnect([]() {
        Serial.print("Connected: ");
        Serial.println(WiFi.localIP());
    });
    WiFiConnector.onError([]() {
        Serial.print("Error. Start AP: ");
        Serial.println(WiFi.softAPIP());
    });

    WiFiConnector.setName(PROJECT_NAME);
    WiFiConnector.closeAP(db[kk::close_ap]);
    WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);

    // settings
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);
}

// ========== loop ==========
void sett_loop() {
    WiFiConnector.tick();
    sett.tick();
}