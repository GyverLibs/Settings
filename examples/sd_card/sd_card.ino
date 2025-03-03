// пример с работой Flash и SD карты, например для графиков

#include <Arduino.h>

#define WIFI_SSID ""
#define WIFI_PASS ""

#include <SettingsGyver.h>
SettingsGyver sett("My Settings");

// БИБЛИОТЕКА SD КАРТЫ
#ifdef ESP8266
#include <SDFS.h>
#else  // ESP32
#include <SD.h>
#endif

#include <LittleFS.h>

void build(sets::Builder& b) {
    b.Plot("/file_plot2.csv");     // из csv таблицы
    b.Plot("/sd/file_plot2.csv");  // из csv таблицы

    // селект с файлами
    String s;
    sett.fs.flash.listDir(s);
    b.Select("", s);
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

#ifdef ESP8266
    LittleFS.begin();

    // ESP8266 SPI
    // MOSI     MISO    CLK     CS
    // D7(13)   D6(12)  D5(14)  D2(4) default!
    SPI.begin();

    // карта подключена к SPI(MOSI/MISO/CLK) + CS на пине D8 (по умолчанию там D4)
    SDFSConfig cfg(D8);
    SDFS.setConfig(cfg);

    SDFS.begin();
    // sett.fs.setFS(LittleFS, SDFS);  // подключение обеих FS
    sett.fs.sd.setFS(SDFS); // подключение SD
#else
    LittleFS.begin(true);

    // ESP32 SPI
    // xSPI     MOSI    MISO    CLK     CS
    // HSPI     13      12      14      15
    // VSPI     23      19      18      5
    SPI.begin();

    // карта подключена к VSPI (по умолч. SPI=VSPI)
    SD.begin(5);

    // sett.fs.setFS(LittleFS, SD);  // подключение обеих FS
    sett.fs.sd.setFS(SD); // подключение SD
#endif

    // список файлов
    // String s;
    // sett.fs.flash.listDir(s);
    // Serial.println(s);

    // s = "";
    // sett.fs.sd.listDir(s);
    // Serial.println(s);
}

void loop() {
    sett.tick();
}