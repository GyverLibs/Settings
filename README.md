[![latest](https://img.shields.io/github/v/release/GyverLibs/Settings.svg?color=brightgreen)](https://github.com/GyverLibs/Settings/releases/latest/download/Settings.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Settings.svg)](https://registry.platformio.org/libraries/gyverlibs/Settings)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Settings?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Settings [beta]
Библиотека для создания простой вебморды esp8266/esp32
- Веб-приложение весит всего 6кб и вшивается в программу в бинарном gzip виде, никакой возни с файлами
- Удобный билдер панели управления из скетча
- Десяток типовых виджетов с возможностью объединения в группы и вложенные страницы
- Интеграция с библиотекой GyverDB для полностью автоматического хранения данных
- Компактный бинарный протокол связи
- Из коробки работает на базе GyverHTTP, легко адаптируется под любую другую библиотеку HTTP сервера
- Это [GyverHub](https://github.com/GyverLibs/GyverHub) на минималках 

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

### Зависимости
- GTL v1.0.7+
- GyverDB v1.0.1+
- StringUtils v1.4.16+
- GyverHTTP v1.0.8+
- GSON v1.5.3+
## `ЕСЛИ НЕ КОМПИЛИРУЕТСЯ - ОБНОВИ БИБЛИОТЕКИ!`

<details>
<summary>platformio.ini</summary>

```ini
[env]
framework = arduino
lib_deps = 
    https://github.com/GyverLibs/GyverHTTP.git
    https://github.com/GyverLibs/GTL.git
    https://github.com/GyverLibs/StringUtils.git
    https://github.com/GyverLibs/GSON.git
    https://github.com/GyverLibs/GyverDB.git
    https://github.com/GyverLibs/Settings.git

[env:d1_mini]
platform = espressif8266
board = d1_mini
upload_speed = 921600
monitor_speed = 115200
monitor_filters = esp8266_exception_decoder, default
build_type = debug
board_build.filesystem = littlefs

[env:esp32-c3]
monitor_speed = 115200
platform = espressif32
board = esp32dev
board_build.mcu = esp32c3
upload_speed = 2000000
board_build.f_cpu = 80000000L
board_build.filesystem = littlefs

[env:esp32dev]
monitor_speed = 115200
platform = espressif32
board = esp32dev
upload_speed = 921600
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

### Билдер
Пакет собирается устройством в билдере - функция в программе, которая вызывается когда приходит запрос от вебморды. Внутри билдера нужно вызвать методы компонентов в том порядке, в котором они должны находиться в вебморде. Вызов компонента добавляет информацию о нём в пакет. На этом этапе также происходит чтение значения для виджета по его id из базы данных, если она подключена и значение не указано явно.

```cpp
GyverDB db;
Settings sett("My Settings", &db);

void build(sets::Builder& b) {
    b.Input("input1", "Text");
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
    if (b.Input("input1", "Text")) Serial.println("input changed");
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

В системе также реализован механизм обновлений - вебморда периодически запрашивает у устройства обновления. Если база данных подключена - то при изменениях значений в базе где то в программе библиотека автоматически отправит новые значения в вебморду (например если какое то значение изменилось при помощи кнопки). Примечание: если вебморда открыта одновременно с нескольких браузеров - обновления базы данных получит только тот из них, который запросил их первым. Также можно отправить свои значения, если база данных не подключена или не используется для каких-то виджетов. Для этого нужно подключить обработчик обновлений и вручную отправить данные по id виджета.
```cpp
void build(sets::Builder& b) {
    b.Label("lbl1", "Random");
    b.Label("lbl2", "millis()", "", sets::Colors::Red);
}
void update(sets::Updater& upd) {
    upd.update("lbl1", random(100));
    upd.update("lbl2", millis());
}

void setup() {
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);
}
```

Вебморда отслеживает статус устройства, при потере связи появится текст offline в заголовке страницы. После потери связи вебморда будет запрашивать информацию о виджетах, это очень удобно при разработке - например добавляем виджет, загружаем прошивку. За это время вебморда уже понимает что устройство оффлайн и при первом успешном подключении выводит актуальные виджеты. При изменении значений виджетов вебморда также следит за доставкой пакета, при ошибке связи появится надпись error у соответствующего виджета.

## Описание классов
### Settings/SettingsBase
```cpp
SettingsBase(const String& title = "", GyverDB* db = nullptr);

// установить заголовок страницы
void setTitle(const String& title);

// установить период обновлений
void setUpdatePeriod(uint16_t prd);

// подкдлючить базу данных
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

// перезагрузить страницу
void reload();

// контейнеры
bool beginGroup(Text title = Text());
void endGroup();

bool beginMenu(Text title);
void endMenu();

bool beginButtons();
void endButtons();

// виджеты
void Label(size_t id, Text label, Text text = Text(), uint32_t color = SETS_DEFAULT_COLOR);
void Label(size_t id, Text label, Text text, sets::Colors color);
bool Input(size_t id, Text label, Text value = Text());
bool Pass(size_t id, Text label, Text value = Text());
bool Color(size_t id, Text label, Text value = Text());
bool Switch(size_t id, Text label, Text value = Text());
bool Date(size_t id, Text label, Text value = Text());
bool Time(size_t id, Text label, Text value = Text());
bool DateTime(size_t id, Text label, Text value = Text());
bool Slider(size_t id, Text label, float min = 0, float max = 100, float step = 1, Text unit = Text(), Text value = Text());
bool Button(size_t id, Text label, uint32_t color = SETS_DEFAULT_COLOR);
bool Button(size_t id, Text label, sets::Colors color);

// опции разделяются ;
bool Select(size_t id, Text label, Text options, Text value = Text());
```

Здесь `Text` - универсальный текстовый формат, принимает строки в любом виде. При указании `value` отличным от стандартного будет отправлено его значение. Иначе будет отправлено значение из БД, если она подключена. Если в качестве значения нужно число - используйте конструктор `Value`, например `b.Color("col", "Color", Value(my_color));`, где `my_color` это `uint32_t`.

<a id="versions"></a>

## Версии
- v1.0

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