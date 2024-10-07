#include <Arduino.h>

// вывод всех виджетов в две группы, одна с БД, вторая с переменными
// переменные для удобства запакованы в структуру

#define WIFI_SSID ""
#define WIFI_PASS ""

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <GyverDBFile.h>
#include <LittleFS.h>
GyverDBFile db(&LittleFS, "/data2.db");

#include <SettingsGyver.h>
SettingsGyver sett("My Settings", &db);

DB_KEYS(
    kk,
    label,
    led,
    paragr,
    input,
    inputc,
    number,
    pass,
    color,
    sw,
    datew,
    timew,
    datetime,
    slider,
    sel,
    conf,
    btn);

struct Data {
    // String label = "label";
    // bool led = 1;
    String paragr = "paragraph";
    String input = "input";
    char inputc[20] = "input";
    int number = 123456;
    String pass = "pass";
    uint32_t color = 0xff0000;
    bool sw = true;
    uint32_t datew = 1728123055;
    uint32_t timew = 12 * 3600 + 30 * 60;
    uint32_t datetime = 1728123055;
    float slider = 33;
    uint8_t sel = 1;
};

Data data;
bool cfm_f = false;

void build(sets::Builder& b) {
    {
        sets::Group g(b, "database");

        b.Label(kk::label);
        b.LED(kk::led);
        b.Paragraph(kk::paragr);
        b.Input(kk::input);
        b.Number(kk::number);
        b.Pass(kk::pass);
        b.Color(kk::color);
        b.Switch(kk::sw);
        b.Date(kk::datew);
        b.Time(kk::timew);
        b.DateTime(kk::datetime);
        b.Slider(kk::slider);
        b.Select(kk::sel, "", "foo;bar;test");
        if (b.Button(kk::btn)) Serial.println("btn 0");
    }
    {
        sets::Group g(b, "variables");

        // b.Label("", data.label);
        // b.LED("", data.led);
        b.Label("", data.input);
        b.LED("", data.sw);
        b.Paragraph("", data.paragr);
        b.Input("", &data.input);
        b.Input("", AnyPtr(data.inputc, 20));
        b.Number("", &data.number);
        b.Pass("", &data.pass);
        b.Color("", &data.color);
        b.Switch("", &data.sw);
        b.Date("", &data.datew);
        b.Time("", &data.timew);
        b.DateTime("", &data.datetime);
        b.Slider("", 0, 100, 0.5, "", &data.slider);
        b.Select("", "foo;bar;test", &data.sel);
        if (b.Button()) Serial.println("btn 1");
    }

    if (b.Button("Confirm")) cfm_f = true;

    if (b.Confirm(kk::conf, "Confirm")) {
        Serial.println(b.build.value);
    }
}

void update(sets::Updater& u) {
    if (cfm_f) {
        cfm_f = false;
        u.update(kk::conf);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // ======== WIFI ========

    // STA
    WiFi.mode(WIFI_STA);
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

    // ======== SETTINGS ========
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);

    // ======== DATABASE ========
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();

    db.init(kk::label, "label");
    db.init(kk::led, 1);
    db.init(kk::paragr, "paragraph");
    db.init(kk::input, "input");
    db.init(kk::number, 123456);
    db.init(kk::pass, "pass");
    db.init(kk::color, 0xff0000);
    db.init(kk::sw, true);
    db.init(kk::datew, 1728123055);
    db.init(kk::timew, 12 * 3600 + 30 * 60);
    db.init(kk::datetime, 1728123055);
    db.init(kk::slider, 33);
    db.init(kk::sel, 1);
}

void loop() {
    sett.tick();
}