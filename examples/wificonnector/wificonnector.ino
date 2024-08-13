// простой пример настройки подключения к WiFi с WiFiConnector

#include <Arduino.h>
#include <GyverDBFile.h>
#include <LittleFS.h>
#include <SettingsESP.h>
#include <WiFiConnector.h>

GyverDBFile db(&LittleFS, "/data.db");
SettingsESP sett("WiFi config", &db);

DB_KEYS(
    kk,
    DB_KEY(wifi_ssid),
    DB_KEY(wifi_pass),
    DB_KEY(apply));

void build(sets::Builder& b) {
    {
        sets::Group g(b, "WiFi");
        b.Input(kk::wifi_ssid, "SSID");
        b.Pass(kk::wifi_pass, "Password", "***");

        if (b.Button(kk::apply, "Connect")) {
            db.update();
            WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // базу данных запускаем до подключения к точке
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();

    // подключение и реакция на подключение или ошибку
    WiFiConnector.onConnect([]() {
        Serial.print("Connected! ");
        Serial.println(WiFi.localIP());
    });
    WiFiConnector.onTimeout([]() {
        Serial.print("Error! start AP ");
        Serial.println(WiFi.softAPIP());
    });

    WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);

    // запускаем сервер после connect, иначе DNS не подхватится
    sett.begin();
    sett.onBuild(build);
}

void loop() {
    WiFiConnector.tick();
    sett.tick();
}