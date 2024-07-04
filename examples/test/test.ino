#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <GyverDBFile.h>
#include <LittleFS.h>
// база данных для хранения настроек
// будет автоматически записываться в файл при изменениях
GyverDBFile db(&LittleFS, "db.bin");

#include <Settings.h>
// указывается заголовок меню, подключается база данных
Settings sett("My Settings", &db);

// ключи для хранения в базе данных
// разоврачивается в enum /имя = хэш(имя)/, так просто компактнее выглядит
DB_KEYS(
    kk,
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

// билдер! Тут строится наше окно настроек
void build(sets::Builder& b) {
    // можно узнать, было ли действие по виджету
    if (b.build().isAction()) {
        Serial.print("Set: 0x");
        Serial.print(b.build().id(), HEX);
        Serial.print(" = ");
        Serial.println(b.build().value());
    }

    // группа. beginGroup всегда вернёт true для удобства организации кода
    if (b.beginGroup("Group 1")) {
        b.Input(kk::txt, "Text");
        b.Pass(kk::pass, "Password");
        b.Input(kk::int8w, "8 bit int");
        b.Input(kk::int16w, "16 bit int");
        b.Input(kk::uint32w, "32 bit uint");

        b.endGroup();  // НЕ ЗАБЫВАЕМ ЗАВЕРШИТЬ ГРУППУ
    }

    // пара лейблов вне группы. Так тоже можно
    b.Label(kk::lbl1, "Random");
    b.Label(kk::lbl2, "millis()", "", sets::Colors::Red);

    // ещё группа
    if (b.beginGroup("Group 2")) {
        b.Color(kk::color, "Color");
        b.Switch(kk::toggle, "Switch");
        b.Select(kk::selectw, "Select", "var1;var2;hello");
        b.Slider(kk::slider, "Slider", -10, 10, 0.5, "deg");
        b.endGroup();
    }

    // и ещё
    if (b.beginGroup("Group3")) {
        b.Date(kk::date, "Date");
        b.Time(kk::timew, "Time");
        b.DateTime(kk::datime, "Datime");

        // а это кнопка на вложенное меню. Далее нужно описать его содержимое
        if (b.beginMenu("Submenu")) {
            // тут тоже могут быть группы
            if (b.beginGroup("Group 3")) {
                b.Switch("sw1"_h, "switch 1");
                b.Switch("sw2"_h, "switch 2");
                b.Switch("sw3"_h, "switch 3");
                b.endGroup();
            }

            // и просто виджеты
            b.Label("lbl3"_h, "Another label", "Val", sets::Colors::Green);
            b.Label("lbl4"_h, "Привет", "Val", sets::Colors::Blue);

            b.endMenu();  // не забываем завершить меню
        }

        b.endGroup();
    }

    // кнопки являются "групповым" виджетом, можно сделать несколько кнопок в одной строке
    if (b.beginButtons()) {
        // кнопка вернёт true при клике
        if (b.Button(kk::btn1, "reload")) {
            Serial.println("reload");
            b.reload();
        }

        if (b.Button(kk::btn2, "clear db", sets::Colors::Blue)) {
            Serial.println("clear db");
            db.clear();
            db.update();
        }

        b.endButtons();     // завершить кнопки
    }
}

// это апдейтер. Функция вызывается, когда вебморда запрашивает обновления
void update(sets::Updater& upd) {
    // можно отправить значение по имени (хэшу) виджета
    upd.update(kk::lbl1, random(100));
    upd.update(kk::lbl2, millis());

    // примечание: при ручных изменениях в базе данных отправлять новые значения не нужно!
    // библиотека сделает это сама =)
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

    // инициализация базы данных начальными значениями
}

void loop() {
    // тикер, вызывать в лупе
    sett.tick();
}