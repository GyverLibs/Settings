#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <GyverDBFile.h>
#include <LittleFS.h>
GyverDBFile db(&LittleFS, "/data.db");

#include <SettingsGyver.h>
#include <SettingsCamera.h>
SettingsGyver sett("My Settings", &db);

void build(sets::Builder& b) {
    b.Stream();
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // ======== WIFI ========

    // STA
    WiFi.mode(WIFI_AP_STA);
    // if (strlen(WIFI_SSID)) {
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    uint8_t tries = 20;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if (!--tries) break;
    }
    Serial.println();
    Serial.print("Connected: ");
    Serial.println(WiFi.localIP());
    // }

    // AP
    WiFi.softAP("AP ESP");
    Serial.print("AP: ");
    Serial.println(WiFi.softAPIP());

    // ======== SETTINGS ========
    sett.begin();
    sett.onBuild(build);

    // ======== DATABASE ========
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();

    sets::cameraInit();
    sets::streamBegin();
}

void loop() {
    sett.tick();
}