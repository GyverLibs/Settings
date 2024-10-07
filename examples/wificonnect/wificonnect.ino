// простой пример настройки подключения к WiFi
// ESP работает в режиме AP_STA, чтобы всегда был доступ к настройкам через точку доступа
// для упрощения логики работы подключение делается при старте
// Сохранение настроек wifi перезагружает чип

#include <Arduino.h>
#include <GyverDBFile.h>
#include <LittleFS.h>
GyverDBFile db(&LittleFS, "/data.db");

#include <SettingsESP.h>
SettingsESP sett("WiFi config", &db);

DB_KEYS(
    kk,
    wifi_ssid,
    wifi_pass,
    apply);

void build(sets::Builder& b) {
    {
        sets::Group g(b, "WiFi");
        b.Input(kk::wifi_ssid, "SSID");
        b.Pass(kk::wifi_pass, "Password");
        if (b.Button(kk::apply, "Save & Restart")) {
            db.update();  // сохраняем БД не дожидаясь таймаута
            ESP.restart();
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // режим AP_STA. Вызываем перед sett.begin(), чтобы
    // settings знал о текущем режиме wifi
    WiFi.mode(WIFI_AP_STA);

    sett.begin();
    sett.onBuild(build);

    // базу данных запускаем до подключения к точке
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();
    db.init(kk::wifi_ssid, "");
    db.init(kk::wifi_pass, "");

    // ======= AP =======
    WiFi.softAP("AP ESP");
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());

    // ======= STA =======
    // если логин задан - подключаемся
    if (db[kk::wifi_ssid].length()) {
        WiFi.begin(db[kk::wifi_ssid], db[kk::wifi_pass]);
        Serial.print("Connect STA");
        int tries = 20;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print('.');
            if (!--tries) break;
        }
        Serial.println();
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    }
}

void loop() {
    sett.tick();
}
