// минимальный пример с хранением данных в переменных
// для хранения в энергонезависимой памяти используй структуры+FileData или пример с базой данных

#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <SettingsGyver.h>
SettingsGyver sett("My Settings");

int slider;
String input;
bool swit;

void build(sets::Builder& b) {
    b.Slider("My slider", 0, 50, 1, "", &slider);
    b.Input("My input", &input);
    b.Switch("My switch", &swit);
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    WiFi.mode(WIFI_STA);
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
}