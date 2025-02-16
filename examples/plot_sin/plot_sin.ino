#include <Arduino.h>

// Пример с running графиком - настройка параметров синусоиды 

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <SettingsGyverWS.h>
SettingsGyverWS sett("My Settings");

float phase = 0, omega = 0.1, ampli = 1;

void build(sets::Builder& b) {
    b.PlotStack(H(sin), "sin 1;sin 2");
    b.Slider("Phase", 0, 3.14 * 2, 0.01, "", &phase);
    b.Slider("Omega", 0.05, 1, 0.01, "", &omega);
    b.Slider("Ampli", 0.2, 5, 0.1, "", &ampli);
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

    static uint32_t tmrs;
    if (millis() - tmrs >= 100) {
        tmrs = millis();
        static int i;
        i++;
        float v[] = {sin(i * omega + phase) * ampli, sin(i * 0.1f) * 2.5f};
        sett.updater().updatePlot(H(sin), v);
    }
}