[![latest](https://img.shields.io/github/v/release/GyverLibs/Settings.svg?color=brightgreen)](https://github.com/GyverLibs/Settings/releases/latest/download/Settings.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Settings.svg)](https://registry.platformio.org/libraries/gyverlibs/Settings)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Settings?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Settings
Библиотека для создания простого веб-интерфейса настроек на esp8266/esp32
- Веб-приложение весит около 20кб и вшивается в программу в бинарном gzip виде без возни с файлами
- Удобный билдер панели управления из скетча
- Два десятка типовых виджетов с возможностью объединения в группы и вложенные меню, в том числе вывод графиков + кастомные виджеты
- Система авторизации с разными правами для авторизованных юзеров и гостей
- Файловый менеджер и OTA (обновление по воздуху)
- Интеграция с библиотекой [GyverDB](https://github.com/GyverLibs/GyverDB) для полностью автоматического хранения данных
- Компактный бинарный протокол связи
- Легко адаптируется под любую библиотеку HTTP сервера, из коробки реализовано три версии: GyverHTTP, стандартный esp-WebServer, ESPAsyncWebserver
- Из коробки поддерживает вебсокет со сторонними библиотеками
- Исходник веб-приложения [здесь](https://github.com/GyverLibs/Settings-web)

> [!TIP]
> Есть приложение для поиска устройств с библиотекой в локальной сети:
> - [Android](https://github.com/GyverLibs/Settings-discover)
> - [Windows](https://github.com/TonTon-Macout/APP-for-AlexGyver-Settings)

![promo](/img/promo.png)

## Документация
Находится в [папке docs](https://github.com/GyverLibs/Settings/tree/main/docs/1.main.md)

## Совместимость
ESP8266, ESP32

> [!CAUTION]
> Подключение в проект библиотеки FastLED версии старше [3.7.8](https://github.com/GyverLibs/Settings/issues/35#issuecomment-2691839527) ломает вебсервер! Ждём фиксов со стороны ESP или FastLED

### Зависимости
- [GTL](https://github.com/GyverLibs/GTL) v1.3.0+
- [GyverDB](https://github.com/GyverLibs/GyverDB) v1.3.0+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.4.30+
- [GyverHTTP](https://github.com/GyverLibs/GyverHTTP) v1.0.26+
- [BSON](https://github.com/GyverLibs/BSON) v2.1.0+
- [Stamp](https://github.com/GyverLibs/Stamp) v1.4.0+
- [Table](https://github.com/GyverLibs/Table) v1.2.0+
- [StreamIO](https://github.com/GyverLibs/StreamIO) v1.0.5+

> [!TIP]
> При установке из реестра PIO или Arduino IDE все зависимости установятся автоматически!

Дополнительно (ставится вручную):
- [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets) для версии SettingsGyverWS/SettingsESPWS
- [ESPAsyncTCP](https://github.com/esphome/ESPAsyncTCP) для версии SettingsAsync/SettingsAsyncWS
- [ESPAsyncWebServer](https://github.com/esphome/ESPAsyncWebServer) для версии SettingsAsync/SettingsAsyncWS

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

- v1.1.0
  - Добавлен виджет Number
  - Создание виджета без ID (автоматический ID)
  - Создание виджета без лейбла (будет равен типу виджета)
  - Привязка внешней переменной к виджету на чтение и запись
  - Редактор текстовых файлов в менеджере файлов
  - Убран лаг с прошлым состоянием вебморды при обновлении страницы
  - Возможность полностью убрать поддержку GyverDB дефайном
  - Мелкие фиксы стилей
  - Методы build(), value(), id() теперь являются членами (build, value, id) для краткости

- v1.1.4
  - У виджета Slider теперь кликабельный результат, можно ввести число
  - Виджеты DateTime и Date теперь выводят и отправляют unix время с учётом часового пояса браузера

- v1.1.5
  - Виджету LED добавлена настройка цвета включенный/выключенный
  - Виджету Confirm добавлена возможность подключить bool переменную для результата
  - Добавлена возможность отправить всплывающее окно в Update

- v1.1.6
  - Добавлен виджет LabelNum для чисел
  - Добавлены настройки вебморды (таймаут соединения, слайдеров, апдейтов)

- v1.1.8
  - Разбивка на пакеты для меньшего использования памяти
  - Добавлен LabelFloat

- v1.1.12
  - Добавлена поддержка цвета виджетам Slider и Switch

- v1.2.0
  - Добавлен двойной слайдер
  - Добавлен логгер (Web Serial)
  - Добавлена поддержка кастомных виджетов на JS
  - Добавлены обработчики скачивания и загрузки файлов

- v1.2.5
  - Добавлена очистка в Logger
  - Умная автопрокрутка в Log
  - Изменён дизайн Password
  - Редизайн кнопок
  - Обработка события и состояния подключения браузера
  - Два стиля разделителя для Row
  - Добавлен виджет HTML
  - Добавлен виджет кнопки с событиями нажатия и отпускания
  - Виджету Input добавлен regex + ограничение длины (для char[])
  - Виджетам Date и DateTime можно задать часовой пояс для отображения
  - Добавлена синхронизация времени с браузера
  - Добавлен виджет вывода изображения из интернета или с флешки
  - Добавлен виджет стрима с камеры ESP32-CAM
  - Добавлен виджет "Вкладки"
  - Обновление с цветом
  - Настройка цветовой схемы всей вебморды
  - Расширена документация, добавлены примеры

- v1.3.0
  - Добавлена поддержка websocket во все три версии сервера
  - Добавлена поддержка SD карты одновременно с Flash памятью
  - Отправка обновлений из любого места в программе (по websocket)
  - Добавлен виджет графиков: 4 типа графиков с поддержкой бинарных таблиц и csv
  - Добавлен виджет джойстик
  - В логгер добавлена подсветка цветом строк с учётом их статуса (ошибка, предупреждение, инфо)
  - Добавлена группировка опций в виджет Select
  - При запросе перезагрузки Reload теперь сохраняется текущее открытое меню, если иерархия и названия меню не изменились
  - Добавлен виджет Ссылка
  - Добавлен минимум и максимум в Number
  - Мелкие исправления и улучшения

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
