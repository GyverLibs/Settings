#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <GyverDBFile.h>
#include <LittleFS.h>
// база данных для хранения настроек
// будет автоматически записываться в файл при изменениях
GyverDBFile db(&LittleFS, "/data.db");

#include <SettingsGyver.h>
// указывается заголовок меню, подключается база данных
SettingsGyver sett("My Settings", &db);

// ключи для хранения в базе данных
enum kk : size_t {
    txt,
    pass,
    uintw,
    intw,
    int64w,

    color,
    toggle,
    slider,
    selectw,
    sldmin,
    sldmax,

    lbl1,
    lbl2,

    date,
    timew,
    datime,

    btn1,
    btn2,
};

sets::Logger logger(150);

// билдер! Тут строится наше окно настроек
void build(sets::Builder& b) {
    // можно узнать, было ли действие по виджету
    if (b.build.isAction()) {
        Serial.print("Set: 0x");
        Serial.print(b.build.id, HEX);
        Serial.print(" = ");
        Serial.println(b.build.value);

        logger.print("Set: 0x");
        logger.println(b.build.id, HEX);
    }

    // группа. beginGroup всегда вернёт true для удобства организации кода
    if (b.beginGroup("Group 1")) {
        b.Input(kk::txt, "Text");
        b.Pass(kk::pass, "Password");
        b.Input(kk::uintw, "uint");
        b.Input(kk::intw, "int");
        b.Input(kk::int64w, "int 64");
        b.HTML("Link", R"(<a href="https://alexgyver.ru/">alexgyver.ru</a>)");

        b.endGroup();  // НЕ ЗАБЫВАЕМ ЗАВЕРШИТЬ ГРУППУ
    }

    if (b.beginGroup("Regex")) {
        char str[10] = "123";
        b.Input("", AnyPtr(str, 10));
        b.Input("", AnyPtr(str, 10), R"(^\d+$)");
        b.Input("", AnyPtr(str, 10), R"(^\d+$)", "Только цифры");

        b.endGroup();
    }

    // пара лейблов вне группы. Так тоже можно
    b.Label(kk::lbl1, "Random");
    b.Label(kk::lbl2, "millis()", "", sets::Colors::Red);

    // image
    // b.Image(H(img), "", "/image.png");
    // b.Image("", "https://alexgyver.ru/wp-content/uploads/2021/03/ardu.jpg");

    // ещё группа
    // можно использовать такой синтаксис: sets::Group(Builder&, title) заключается в блок кода в самом начале,
    // вызывать endGroup в этом случае не нужно
    // так же работают остальные контейнеры (Menu, Buttons)
    {
        sets::Group g(b, "Group 2");
        b.Color(kk::color, "Color");
        b.Switch(kk::toggle, "Switch");
        b.Select(kk::selectw, "Select", "var1;var2;hello");
        b.Slider(kk::slider, "Slider", -10, 10, 0.5, "deg");
        b.Slider2(kk::sldmin, kk::sldmax, "Slider", -10, 10, 0.5, "deg");

        // логгер, в него печатаем выше
        b.Log(logger);

        if (b.beginRow()) {
            if (b.Button("click")) {
                Serial.print("click: ");
                Serial.println(b.build.pressed());
            }
            if (b.ButtonHold("hold")) {
                Serial.print("hold: ");
                Serial.println(b.build.pressed());
            }
            b.endRow();
        }
    }

    // разные стили Row ВНЕ ГРУППЫ
    if (b.beginRow("", sets::DivType::Default)) {
        b.Switch("Switch");
        b.Switch("Switch");
        b.endRow();
    }
    if (b.beginRow("", sets::DivType::Block)) {
        b.Switch("Switch");
        b.Switch("Switch");
        b.endRow();
    }
    if (b.beginRow("", sets::DivType::Line)) {
        b.Switch("Switch");
        b.Switch("Switch");
        b.endRow();
    }

    // и ещё группа
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

        b.endButtons();  // завершить кнопки
    }
}

// это апдейтер. Функция вызывается, когда вебморда запрашивает обновления
void update(sets::Updater& upd) {
    // можно отправить значение по имени (хэшу) виджета
    upd.update(kk::lbl1, random(100));
    upd.update(kk::lbl2, millis());

    // примечание: при ручных изменениях в базе данных отправлять новые значения не нужно!
    // библиотека сделает это сама
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

    sett.onFocusChange([]() {
        Serial.print("Focus: ");
        Serial.println(sett.focused());
    });

    // настройки вебморды
    // sett.config.requestTout = 3000;
    // sett.config.sliderTout = 500;
    // sett.config.updateTout = 1000;
    // sett.config.theme = sets::Colors::Green;

    // ======== DATABASE ========
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    db.begin();

    // инициализация базы данных начальными значениями
    db.init(kk::txt, "text");
    db.init(kk::pass, "some pass");
    db.init(kk::uintw, 64u);
    db.init(kk::intw, -10);
    db.init(kk::int64w, 1234567ll);
    db.init(kk::color, 0xff0000);
    db.init(kk::toggle, (bool)1);
    db.init(kk::slider, -3.5);
    db.init(kk::selectw, (uint8_t)1);
    db.init(kk::date, 1719941932);
    db.init(kk::timew, 60);
    db.init(kk::datime, 1719941932);
    db.init(kk::sldmin, -5);
    db.init(kk::sldmax, 5);

    // db.dump(Serial);

    // часовой пояс для rtc
    setStampZone(3);
}

void loop() {
    // тикер, вызывать в лупе
    sett.tick();

    // время получено с браузера
    if (sett.rtc.newSecond()) {
        Serial.println(sett.rtc.toString());
    }
}