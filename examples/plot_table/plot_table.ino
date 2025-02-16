#include <Arduino.h>

/*
    Пример с таблицей-буфером, в котором таблица графика заполняется точками с привязкой
    ко времени в миллисекундах и отправляется раз в несколько секунд, чтобы уменьшить
    количество отправок с сохранением миллисекундной точности точек. При отправке апдейта таблица очищается
*/

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <SettingsGyverWS.h>
SettingsGyverWS sett("My Settings");

Table t(0, 3, cell_t::Uint64, cell_t::Float, cell_t::Float);

void build(sets::Builder& b) {
    b.Plot(H(plot));
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
}

void loop() {
    sett.tick();

    // набиваем таблицу с временем в миллисекундах
    static uint32_t tmr1;
    if (millis() - tmr1 >= 200) {
        tmr1 = millis();
        static int i;
        if (sett.rtc.synced()) {
            t.append(sett.rtc.getUnixMs(), sin(i / 10.0f) * 10.0f, sin((i - 10) / 10.0f) * 15.0f);
            i++;
        }
    }

    // отправляем пачкой
    static uint32_t tmr;
    if (millis() - tmr >= 2000) {
        tmr = millis();
        sett.updater().updatePlot(H(plot), t);
    }
}