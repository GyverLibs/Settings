[![latest](https://img.shields.io/github/v/release/GyverLibs/Settings.svg?color=brightgreen)](https://github.com/GyverLibs/Settings/releases/latest/download/Settings.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Settings.svg)](https://registry.platformio.org/libraries/gyverlibs/Settings)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Settings?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Settings
Библиотека для создания простого веб-интерфейса настроек на esp8266/esp32
- Веб-приложение весит около 10кб и вшивается в программу в бинарном gzip виде без возни с файлами
- Удобный билдер панели управления из скетча
- Десяток типовых виджетов с возможностью объединения в группы и вложенные меню
- Система авторизации с разными правами для авторизованных юзеров и гостей
- Файловый менеджер и OTA (обновление по воздуху)
- Интеграция с библиотекой [GyverDB](https://github.com/GyverLibs/GyverDB) для полностью автоматического хранения данных
- Компактный бинарный протокол связи
- Легко адаптируется под любую библиотеку HTTP сервера, из коробки реализовано три версии: GyverHTTP, стандартный esp-WebServer, ESPAsyncWebserver
- Это [GyverHub](https://github.com/GyverLibs/GyverHub) на минималках 

> Есть [Android-приложение](https://github.com/GyverLibs/Settings-discover) для поиска устройств с библиотекой

![promo](/img/promo.png)

### Совместимость
ESP8266, ESP32

### Зависимости
- GTL v1.1.1+
- GyverDB v1.1.2+
- StringUtils v1.4.24+
- GyverHTTP v1.0.17+
- GSON v1.5.9+

<details>
<summary>platformio.ini</summary>

```ini
[env]
framework = arduino
lib_deps =
    https://github.com/GyverLibs/Settings.git
    esphome/ESPAsyncWebServer-esphome   ; для версии SettingsAsync
    esphome/ESPAsyncTCP-esphome         ; для версии SettingsAsync

[env:d1_mini]
platform = espressif8266
board = d1_mini
upload_speed = 921600
monitor_speed = 115200
monitor_filters = esp8266_exception_decoder, default
build_type = debug
board_build.filesystem = littlefs

[env:esp32dev]
monitor_speed = 115200
platform = espressif32
board = esp32dev
upload_speed = 921600
board_build.filesystem = littlefs

[env:esp32-c3]
monitor_speed = 115200
platform = espressif32
board = esp32dev
board_build.mcu = esp32c3
upload_speed = 2000000
board_build.f_cpu = 80000000L
board_build.filesystem = littlefs
```
</details>

## Содержание
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Использование
### Как это работает
В библиотеке настроен вебсервер, который отправляет файлы вебморды при заходе на ip платы. Файлы вебморды вшиты в PROGMEM (в код программы) в бинарном виде - загружать файлы в fs не нужно, всё работает из коробки. Лёгкий html файл каждый раз подгружается, а скрипты и стили - кешируются браузером для ускорения загрузки. По сути они загружаются один раз после обновления библиотеки. Вебморда является веб-приложением, которое общается с платой по http. При загрузке она запрашивает у платы пакет с виджетами и прочей информацией, пакет имеет формат бинарного json.

### Captive portal
Во всех трёх реализациях сервера из коробки настроен DNS для работы как Captive portal - если ESP работает в режиме точки доступа (AP или AP_STA), то при подключении к точке автоматически откроется окно браузера со страницей настроек.

### Приложение для поиска
Позволяет находить устройства с библиотекой в локальной сети и заменяет браузер, вебморда открывается сразу в приложении, кнопка назад возвращает к списку устройств. Чтобы удалить устройство - долгое удержание на нём на смартфоне или правой кнопкой мыши на ПК
- [Скачать .apk](https://github.com/GyverLibs/Settings-discover/releases/latest/download/Settings.apk
) последней версии
- [Скачать .html](https://github.com/GyverLibs/Settings-discover/releases/latest/download/index.html) последней версии

Для поиска смартфон/ПК должны быть в одной локальной сети с устройством. В приложении должна быть указана корректная маска подсети (настраивается в роутере). Если в роутере она не менялась - то она там стандартная *255.255.255.0*, как и в приложении по умолчанию.

> Требуется версия библиотеки v1.0.13+

### База данных
Библиотека интегрирована с [GyverDB](https://github.com/GyverLibs/GyverDB) - относительно быстрой базой данных для хранения данных любого типа. Settings автоматически читает и обновляет данные в БД, поэтому рекомендуется изучить как работать с БД на странице описания GyverDB. При использовании GyverDBFile база данных будет автоматически писаться в файл при изменениях, а файловая система позаботится об оптимальном износе flash памяти. При запуске рекомендуется инициализировать БД, указав ключи и соответствующие им начальные значения и типы. Эти значения будут записаны только в том случае, если запись в БД ещё не существует. В то же время автоматическое обновление БД работает только для существующих записей, т.е. Settings будет работать только с сущестующими ячейками и не создаст новых. Минимальный пример:
```cpp
// Подключить библиотеки и создать БД и Settings
#include <GyverDBFile.h>
#include <LittleFS.h>
GyverDBFile db(&LittleFS, "/data.db");

#include <SettingsESP.h>
SettingsESP sett("My Settings", &db);

// Объявить хэш-ключи БД через макрос. Это удобнее, чем "строки" и enum, 
// а также не боится изменения порядка или удаления ключей из середины списка
DB_KEYS(keys,
    DB_KEY(key1),
    DB_KEY(key2),
    DB_KEY(mykey),
);

// билдер
void build(sets::Builder& b) {
}

void setup() {
    Serial.begin(115200);
    Serial.println();

    // WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin("WIFI_SSID", "WIFI_PASS");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println(WiFi.localIP());

    // settings
    sett.begin();
    sett.onBuild(build);

    // запуск файловой системы
#ifdef ESP32
    LittleFS.begin(true); // format on fail
#else
    LittleFS.begin();
#endif

    // запуск БД и чтение из файла
    db.begin();
    
    // инициализация БД начальными значениями
    // db.init(key1, "text");
    // db.init(key2, 12345);
}

void loop() {
    sett.tick();
}
```

### Билдер
Пакет собирается устройством в билдере - функция в программе, которая вызывается когда приходит запрос от вебморды. Внутри билдера нужно вызвать методы компонентов в том порядке, в котором они должны находиться в вебморде. Вызов компонента добавляет информацию о нём в пакет. На этом этапе также происходит чтение значения для виджета по его id из базы данных, если она подключена и *значение не указано явно*.

> Если нужно, чтобы параметр читался из БД, но нужно указать аргумент после него - значением нужно указать `Text()` как и сделано в функции по умолчанию. Например цвет для Label `b.Label(key, "Label!", Text(), sets::Colors::Mint);`. Если указать пустую строку (`b.Label(key, "Label!", "", sets::Colors::Mint);`) - в качестве значения и отправится пустая строка, т.е. значение не из БД.

> Указатель на текущий объект Settings внутри билдера или обработчика обновлений можно получить из `sets::thisSettings`

```cpp
GyverDB db;
SettingsGyver sett("My Settings", &db);

void build(sets::Builder& b) {
    b.Input("input1"_h, "Text");

    // здесь sets::thisSettings - указатель на объект sett
}

void setup() {
    sett.begin();
    sett.onBuild(build);
}

void loop() {
    sett.tick();
}
```

При изменении значений в вебморде она отправляет запрос с id и значением компонента. Если база данных подключена - новое значение автоматически записывается в соответствующую ячейку. Также вызывается билдер, и на этом этапе метод виджета, с которым производилось действие, вернёт true. Можно отловить это для взаимодействия с программой. Таким образом билдер выполняет две функции: сборка виджетов и обратная связь при изменении значения.
```cpp
void build(sets::Builder& b) {
    if (b.Input("input1"_h, "Text")) Serial.println("input changed");
}
```

В билдере можно получить информацию о том, на какой id пришло действие, для взаимодействия с программой:
```cpp
void build(sets::Builder& b) {
    // можно узнать, было ли действие по виджету
    if (b.build().isAction()) {
        Serial.print("Set: 0x");
        Serial.print(b.build().id(), HEX);
        Serial.print(" = ");
        Serial.println(b.build().value());
    }
}
```

### Контейнеры
Виджеты можно объединять в контейнеры. Контейнер нужно начать и закончить, так как пакет данных собирается линейно в целях оптимизации скорости и памяти. Метод `beginКонтейнер` всегда вернёт true для красоты организации кода в блоке условия:
```cpp
void build(sets::Builder& b) {
    if (b.beginGroup("Group 1")) {
        b.Input("input1"_h, "Text");

        b.endGroup();  // закрыть группу
    }
}
```

Второй вариант - у всех контейнеров есть парный класс, который сам откроет и закроет контейнер. Нужно создать объект с любым именем и передать ему билдер:
```cpp
void build(sets::Builder& b) {
    {
        sets::Group g(b, "Group 2");  // должен быть первым в блоке

        b.Input("input1"_h, "Text");
    }
}
```

Можно создавать вложенные меню. Указанный заголовок будет отображаться на кнопке и в заголовке страницы при входе на меню. Все виджеты и группы, находящиеся в блоке с меню, будут находиться на отдельной странице. Вложенность меню неограниченная.
```cpp
void build(sets::Builder& b) {
    b.Input("input1"_h, "Text 1");

    {
        sets::Menu g(b, "Submenu");

        b.Input("input2"_h, "Text 2");
    }
}
```

Отдельный тип контейнера - кнопки, внутри него можно добавлять только кнопки:
```cpp
void build(sets::Builder& b) {
    {
        sets::Buttons btns(b);

        // кнопка вернёт true при клике
        if (b.Button("btn1"_h, "Button 1")) {
            Serial.println("Button 1");
        }

        if (b.Button("btn2"_h, "Button 2", sets::Colors::Blue)) {
            Serial.println("Button 2");
        }
    }
}
```


В системе также реализован механизм обновлений - вебморда периодически запрашивает у устройства обновления. Если база данных подключена - то при изменениях значений в базе где то в программе библиотека автоматически отправит новые значения в вебморду (например если какое то значение изменилось при помощи кнопки). Примечание: если вебморда открыта одновременно с нескольких браузеров - обновления базы данных получит только тот из них, который запросил их первым. Также можно отправить свои значения, если база данных не подключена или не используется для каких-то виджетов. Для этого нужно подключить обработчик обновлений и вручную отправить данные по id виджета.
```cpp
void build(sets::Builder& b) {
    b.Label("lbl1"_h, "Random");
    b.Label("lbl2"_h, "millis()", "", sets::Colors::Red);
}
void update(sets::Updater& upd) {
    upd.update("lbl1"_h, random(100));
    upd.update("lbl2"_h, millis());
}

void setup() {
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);
}
```

Вебморда отслеживает статус устройства, при потере связи появится текст offline в заголовке страницы. После потери связи вебморда будет запрашивать информацию о виджетах, это очень удобно при разработке - например добавляем виджет, загружаем прошивку. За это время вебморда уже понимает что устройство оффлайн и при первом успешном подключении выводит актуальные виджеты. При изменении значений виджетов вебморда также следит за доставкой пакета, при ошибке связи появится надпись error у соответствующего виджета.

### Авторизация
В системе предусмотрена авторизация: если в прошивке указать отличный от пустой строки пароль - вебморда будет работать в "гостевом" режиме: отображаются только разрешённые гостям виджеты, файловый менеджер и OTA скрыты и заблокированы. Для ввода пароля нужно зайти в меню (правая верхняя кнопка) и нажать на ключик. Серый ключик означает что авторизация отключена, зелёный - клиент авторизован, красный - неверный пароль. Пароль может содержать любые символы и иметь любую длину - в явном виде он не хранится и не передаётся. Пароль сохраняется в браузере и авторизация работает автоматически при перезагрузке страницы.

Для разделения админского и гостевого доступа предусмотрен виртуальный контейнер Guest. Если пароль установлен и клиент не авторизован - он будет видеть только виджеты из гостевых контейнеров. Для корректной работы гостевой контейнер не должен прерываться обычными контейнерами. Пример:

```cpp
if (b.beginGroup("Group 1")) {
    // гости не видят
    b.Pass(kk::pass, "Password");

    // виджеты, которые видят гости и админы
    {
        sets::GuestAccess g(b);
        b.Input(kk::uintw, "uint");
        b.Input(kk::intw, "int");
        b.Input(kk::int64w, "int 64");
    }

    // гости не видят
    {
        sets::Menu m(b, "sub sub");
        b.Label(kk::lbl2, "millis()", "", sets::Colors::Red);
    }

    b.endGroup();
}
```
В гостевой контейнер можно поместить несколько обычных контейнеров, например групп.

## Описание классов
- `SettingsGyver` (*SettingsGyver.h*) - на вебсервере GyverHTTP
- `SettingsESP` (*SettingsESP.h*) - на стандартном вебсервере ESP
- `SettingsAsync` (*SettingsAsync.h*) - на асинхронном ESPAsyncWebserver

### SettingsBase/SettingsGyver/SettingsESP/SettingsAsync
```cpp
Settings(const String& title = "", GyverDB* db = nullptr);

// установить пароль на вебморду. Пустая строка "" чтобы отключить
void setPass(Text pass);

// перезагрузить страницу. Можно вызывать где угодно + в обработчике update
void reload();

// установить заголовок страницы
void setTitle(const String& title);

// установить период обновлений (умолч. 2500мс), 0 чтобы отключить
void setUpdatePeriod(uint16_t prd);

// подключить базу данных
void attachDB(GyverDB* db);

// использовать автоматические обновления из БД (при изменении записи новое значение отправится в браузер)
void useAutoUpdates(bool use);

// обработчик билда
void onBuild(BuildCallback cb);

// обработчик обновлений
void onUpdate(UpdateCallback cb);

// тикер, вызывать в родительском классе
void tick();
```

### Builder
```cpp
// инфо о билде
Build build();

// перезагрузить страницу (вызывать в действии)
void reload();

// КОНТЕЙНЕРЫ
// разрешить неавторизованным клиентам следующий код
bool beginGuest();

// запретить неавторизованным клиентам
void endGuest();

// группа
bool beginGroup(Text title = Text());
void endGroup();

// вложенное меню
bool beginMenu(Text title);
void endMenu();

// ряд кнопок
bool beginButtons();
void endButtons();

// ВИДЖЕТЫ
// ПАССИВНЫЕ
// многострочный текст
void Paragraph(size_t id, Text label, Text text = Text());

// текстовое значение
void Label(size_t id, Text label, Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR);
void Label(size_t id, Text label, Text text, sets::Colors color);

// без id
void Label(Text label, Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR);
void Label(Text label, Text text, sets::Colors color);

// светодиод (включен - зелёный, выключен - красный)
void LED(size_t id, Text label, bool value);
void LED(Text label, bool value);

// АКТИВНЫЕ
// ввод текста и цифр
bool Input(size_t id, Text label, Text value = Text());

// ввод пароля
// можно установить "заглушку" для пароля, например "***", чтобы пароль не отображался в браузере
bool Pass(size_t id, Text label, Text value = Text());

// ввод цвета, результат в обычном 24-бит формате
bool Color(size_t id, Text label, Text value = Text());

// переключатель, результат 1/0
bool Switch(size_t id, Text label, Text value = Text());

// дата, результат в unix секундах
bool Date(size_t id, Text label, Text value = Text());

// дата и время, результат в unix секундах
bool DateTime(size_t id, Text label, Text value = Text());

// время, результат в секундах с начала суток
bool Time(size_t id, Text label, Text value = Text());

// слайдер
bool Slider(size_t id, Text label, float min = 0, float max = 100, float step = 1, Text unit = Text(), Text value = Text());

// кнопку можно добавлять как внутри контейнера кнопок, так и как одиночный виджет
bool Button(size_t id, Text label, uint32_t color = SETS_DEFAULT_COLOR);
bool Button(size_t id, Text label, sets::Colors color);

// список выбора, опции в виде текста разделяются ;
bool Select(size_t id, Text label, Text options, Text value = Text());

// окно подтверждения, для активации отправь пустой update на его id
bool Confirm(size_t id, Text label);
```

Здесь `Text` - универсальный текстовый формат, принимает строки в любом виде. При указании `value` отличным от стандартного будет отправлено его значение. Иначе будет отправлено значение из БД, если она подключена. Если в качестве значения нужно число - используйте конструктор `Value`, например `b.Color("col", "Color", Value(my_color));`, где `my_color` это `uint32_t`.

### Build
Инфо о билде
```cpp
// клиент авторизован (или пароль не указан, т.е. все админы)
bool isGranted();

// id виджета (действие)
size_t id();

// значение виджета (действие)
Text& value();
```

### Контейнеры
```cpp
// контейнер гостевого доступа
class GuestAccess(Builder& b);

// контейнер группы виджетов
class Group(Builder& b, Text title = Text());

// контейнер вложенного меню
class Menu(Builder& b, Text title);

// контейнер кнопок
class Buttons(Builder& b);
```

### Updater
```cpp
// пустой апдейт (например для вызова Confirm)
void update(size_t id);

// апдейт с текстом
void update(size_t id, Text value);

// апдейт с float
void update(size_t id, float value, uint8_t dec = 2);

// апдейт с числом
void update(size_t id, <любой численный тип> value);
```

<a id="versions"></a>

## Версии
- v1.0
- v1.0.2
  - Добавлен виджет Confirm (всплывающее окно подтверждения)
  - Кастомные всплывающие окна для Input (Input теперь работает на просмотре AP WiFi точки на Xiaomi)
- v1.0.5
  - Добавлен виджет LED
  - Добавлен файловый менеджер
  - Добавлено ОТА обновление
  - Добавлена авторизация и гостевой фильтр виджетов
  - Новый стиль для Select

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **Settings** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/Settings/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
