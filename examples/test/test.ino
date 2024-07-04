#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <LittleFS.h>
#include <GyverDBFile.h>

GyverDBFile db(&LittleFS, "db.bin");

#include <Settings.h>
Settings sett("My Settings", &db);

DB_KEYS(kk,
        DB_KEY(txt),
        DB_KEY(pass),
        DB_KEY(int8w),
        DB_KEY(int16w),
        DB_KEY(uint32w),

        DB_KEY(color),
        DB_KEY(toggle),
        DB_KEY(slider),
        DB_KEY(selectw),

        DB_KEY(lbl1),
        DB_KEY(lbl2),

        DB_KEY(date),
        DB_KEY(timew),
        DB_KEY(datime),

        DB_KEY(btn1),
        DB_KEY(btn2),

);

void build(sets::Builder& b) {
    if (b.build().isAction()) {
        Serial.print("Set: 0x");
        Serial.print(b.build().id(), HEX);
        Serial.print(" = ");
        Serial.println(b.build().value());
    }

    if (b.beginGroup("Group 1")) {
        b.Input(kk::txt, "Text");
        b.Pass(kk::pass, "Password");
        b.Input(kk::int8w, "8 bit int");
        b.Input(kk::int16w, "16 bit int");
        b.Input(kk::uint32w, "32 bit uint");
        b.endGroup();
    }

    b.Label(kk::lbl1, "Random");
    b.Label(kk::lbl2, "millis()", "", sets::Colors::Red);

    if (b.beginGroup("Group 2")) {
        b.Color(kk::color, "Color");
        b.Switch(kk::toggle, "Switch");
        b.Select(kk::selectw, "Select", "var1;var2;hello");
        b.Slider(kk::slider, "Slider", -10, 10, 0.5, "deg");
        b.endGroup();
    }

    if (b.beginGroup("Group3")) {
        b.Date(kk::date, "Date");
        b.Time(kk::timew, "Time");
        b.DateTime(kk::datime, "Datime");

        if (b.beginMenu("Submenu")) {
            if (b.beginGroup("Group 3")) {
                b.Switch("sw1"_h, "switch 1");
                b.Switch("sw2"_h, "switch 2");
                b.Switch("sw3"_h, "switch 3");
                b.endGroup();
            }
            b.Label("lbl3"_h, "Another label", "Val", sets::Colors::Green);
            b.Label("lbl4"_h, "Привет", "Val", sets::Colors::Blue);
            b.endMenu();
        }

        b.endGroup();
    }

    if (b.beginButtons()) {
        if (b.Button(kk::btn1, "reload")) {
            Serial.println("reload");
            b.reload();
        }
        if (b.Button(kk::btn2, "clear db", sets::Colors::Blue)) {
            Serial.println("clear db");
            db.clear();
            db.update();
        }
        b.endButtons();
    }
}

void update(sets::Updater& upd) {
    upd.update(kk::lbl1, random(100));
    upd.update(kk::lbl2, millis());
}

void test(const char* t) {
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // ======== WIFI ========

    // STA
    WiFi.mode(WIFI_AP_STA);
    // if (strlen(WIFI_SSID)) {
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
    // }

    // AP
    WiFi.softAP("AP ESP");
    Serial.print("AP: ");
    Serial.println(WiFi.softAPIP());

    // ======== SETTINGS ========
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);

    // ======== DATABASE ========
    LittleFS.begin();
    db.begin();
    db.init(kk::txt, "text");
    db.init(kk::pass, "some pass");
    db.init(kk::int8w, (int8_t)64);
    db.init(kk::int16w, (int16_t)-10);
    db.init(kk::uint32w, (uint32_t)1234567);
    db.init(kk::color, 0xff0000);
    db.init(kk::toggle, (bool)1);
    db.init(kk::slider, -3.0);
    db.init(kk::selectw, (uint8_t)1);
    db.init(kk::date, 1719941932);
    db.init(kk::timew, 60);
    db.init(kk::datime, 1719941932);
}

void loop() {
    sett.tick();
}