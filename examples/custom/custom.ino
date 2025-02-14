#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <GyverDBFile.h>
#include <LittleFS.h>
// база данных для хранения настроек
// будет автоматически записываться в файл при изменениях
GyverDBFile db(&LittleFS, "/data1.db");

#include <SettingsGyver.h>
SettingsGyver sett("My Settings", &db);

// ключи для хранения в базе данных
DB_KEYS(kk,
        testw

);

// билдер! Тут строится наше окно настроек
void build(sets::Builder& b) {
    BSON p;
    p["kek"] = -123456789l;
    p["pek"] = 123456789ul;
    p["true"] = true;
    p["false"] = false;
    b.Custom("TestWidget", kk::testw, p);
}

// это апдейтер. Функция вызывается, когда вебморда запрашивает обновления
void update(sets::Updater& upd) {
    BSON p;
    p["value"] = random(100);
    p["text"] = "kek hello";
    upd.update(kk::testw, p);
}

const char customjs[] PROGMEM = R"raw(
class TestWidget extends WidgetBase {
    constructor(data) {
        super(data);

        super.addOutput(Component.make('span', {
            context: this,
            var: 'out',
            class: 'test',
        }));

        this.update(data);
        console.log(data);
    }

    update(data) {
        this.$out.innerText = data.value ?? '';
        console.log(data);
    }

    static css = `.test {color: blue;}`;
}
)raw";

void setup() {
    Serial.begin(115200);
    Serial.println();

    // ======== WIFI ========
    WiFi.mode(WIFI_AP_STA);
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
    sett.setCustom(customjs, strlen_P(customjs), false);

    // ======== DATABASE ========
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();
    db.init(kk::testw, "text");
}

void loop() {
    // тикер, вызывать в лупе
    sett.tick();
}