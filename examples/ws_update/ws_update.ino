// пример с обновлениями для вебсокет версии

#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <GyverDBFile.h>
#include <LittleFS.h>
GyverDBFile db(&LittleFS, "/data.db");

#include <SettingsGyverWS.h>
SettingsGyverWS sett("My Settings", &db);

DB_KEYS(
    kk,
    label2);

void build(sets::Builder& b) {
    b.Label(H(label));
    b.Label(H(label2));
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected: ");
    Serial.println(WiFi.localIP());

    sett.begin();
    sett.onBuild(build);

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    db.begin();
    db.init(kk::label2, 0);
}

void loop() {
    sett.tick();

    static uint32_t tmr;
    if (millis() - tmr >= 1000) {
        tmr = millis();
        // отправить апдейт прямо сейчас
        sett.updater().update(H(label), random(100));

        // сделать изменение в БД - обновление будет отправлено само чуть позже
        db[kk::label2] = random(100);
    }
}