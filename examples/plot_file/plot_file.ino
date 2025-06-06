#include <Arduino.h>

// пример с графиком из файла, сам файл не читаем, только добавляем в него новые данные
// таким образом файл может быть большого размера - он не читается в оперативку
// бинарные таблицы в ~2 раза легче CSV таблиц
// обновлять в реальном времени такой большой график нет смысла, но можно отправить апдейт с путём к файлу - он будет скачан и показан

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <LittleFS.h>
#include <SettingsGyverWS.h>
#include <TableFileStatic.h>
SettingsGyverWS sett("My Settings");

void build(sets::Builder& b) {
    b.Plot(H(plot1), "/file_plot1.tbl");  // из бинарной таблицы
    b.Plot(H(plot2), "/file_plot2.csv");  // из csv таблицы
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

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    sett.begin();
    sett.onBuild(build);
}

void loop() {
    sett.tick();

    // забиваем данными для примера по часам вебморды
    // если время синхронизировано
    if (sett.rtc.synced()) {
        static uint32_t tmr;
        if (millis() - tmr >= 2000) {
            tmr = millis();

            // ==================== бинарная таблица ====================
            TableFileStatic t(&LittleFS, "/file_plot1.tbl", 100);   // макс. 100 строк, будет смещаться при append()

            // инициализация, должна быть вызвана хотя бы один раз после непосредственного создания файла
            t.init(3, cell_t::Uint32, cell_t::Float, cell_t::Int8);

            // добавление в файл
            t.append(sett.rtc.getUnix(), (random(100) - 50) / 2.0, random(-100, 100));

            // отправить апдейт
            // sett.updater().update(H(plot1), "/file_plot1.tbl");

            // ======================= CSV таблица ======================
            File f = LittleFS.open("/file_plot2.csv", "a");
            if (f) {
                f.print(sett.rtc.getUnix());
                f.print(';');
                f.print((random(100) - 50) / 2.0);
                f.print(';');
                f.print(random(-100, 100));
                f.println();
                // перевод строки можно делать или в начале, или в конце строки
                // если он в начале или конце файла - будет проигнорирован вебмордой
            }
        }
    }
}