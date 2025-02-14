#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <GyverDBFile.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
// база данных для хранения настроек
// будет автоматически записываться в файл при изменениях
GyverDBFile db(&LittleFS, "/data.db");

// указывается заголовок меню, подключается база данных
SettingsGyver sett("My Settings", &db);

// имена ячеек базы данных
DB_KEYS(
    kk,
    input
);

// билдер! Тут строится наше окно настроек
void build(sets::Builder& b) {
    b.Input(kk::input, "My input");
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

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    sett.begin();
    sett.onBuild(build);

    // запуск и инициализация полей БД
    db.begin();
    db.init(kk::input, 0);
}

void loop() {
    // тикер, вызывать в лупе
    sett.tick();
}