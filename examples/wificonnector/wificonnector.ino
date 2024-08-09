// простой пример настройки подключения к WiFi на базе DBConnector
// если ssid pass не задан - откроется AP
// если задан - попытаемся подключиться, по выходе таймаута запустим AP

#include <Arduino.h>
#include <GyverDBFile.h>
#include <LittleFS.h>
GyverDBFile db(&LittleFS, "/data.db");

#include <SettingsESP.h>
SettingsESP sett("WiFi config", &db);

DB_KEYS(
    kk,
    DB_KEY(wifi_ssid),
    DB_KEY(wifi_pass),
    DB_KEY(apply));

#include <DBConnector.h>
DBConnector wifi(&db, kk::wifi_ssid, kk::wifi_pass);

void build(sets::Builder& b) {
    {
        sets::Group g(b, "WiFi");
        b.Input(kk::wifi_ssid, "SSID");
        b.Pass(kk::wifi_pass, "Password", "***");

        if (b.Button(kk::apply, "Connect")) {
            db.update();
            wifi.connect();
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
    wifi.onConnect([]() {
        Serial.print("Connected! ");
        Serial.println(WiFi.localIP());
    });
    wifi.onError([]() {
        Serial.print("Error! start AP ");
        Serial.println(WiFi.softAPIP());
    });
    wifi.connect();

    // запускаем сервер после connect, иначе DNS не подхватится
    sett.begin();
    sett.onBuild(build);
}

void loop() {
    sett.tick();
    wifi.tick();
}