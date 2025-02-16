#include <Arduino.h>

// Пример обновления running и stack графиков через вебсокет

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <SettingsGyverWS.h>
SettingsGyverWS sett("My Settings");

void build(sets::Builder& b) {
    b.PlotRunning(H(run), "kek1;kek2");
    b.PlotStack(H(stack), "kek3;kek4");
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
    if (millis() - tmrs >= 500) {
        tmrs = millis();

        float v[] = {random(100) - 50, (random(100) - 50) / 2.0};
        sett.updater()
            .updatePlot(H(run), v)
            .updatePlot(H(stack), v);
    }
}