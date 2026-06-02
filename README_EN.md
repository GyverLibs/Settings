This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/Settings.svg?color=brightgreen)](https://github.com/GyverLibs/Settings/releases/latest/download/Settings.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Settings.svg)](https://registry.platformio.org/libraries/gyverlibs/Settings)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Settings?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Settings
A library to create a simple web interface settings on esp8266/esp32
- The web application weighs about 20kb and is sewn into the program in binary gzip form without fussing with files.
- Convenient control panel builder from a sketch
- Two dozen typical widgets with the ability to combine in groups and nested menus, including graph output + custom widgets
- Authorization system with different rights for authorized users and guests
- File Manager and OTA (Air Update)
- Integration with the library[GyverDB](https://github.com/GyverLibs/GyverDB)for fully automatic data storage
- Compact binary communication protocol
- Easily adaptable to any HTTP server library, three versions are implemented out of the box: GyverHTTP, standard esp-WebServer, ESPAsyncWebserver
- Out of the box supports websocket with third-party libraries
- Web application source[here](https://github.com/GyverLibs/Settings-web)

> [!TIP]
> There is an application to search for devices with a library in the local network:
> - [Android](https://github.com/GyverLibs/Settings-discover)
> - [Windows](https://github.com/TonTon-Macout/APP-for-AlexGyver-Settings)
> - iOS: [AppStore](https://apps.apple.com/us/app/gyver-settings/id6751504764) | [Repository](https://github.com/Vl-VSV/Gyver-Settings)

![promo](/img/promo.png)

## Documentation.
Located in[file](https://github.com/GyverLibs/Settings/tree/main/docs/1.main.md)

## Compatibility
ESP8266, ESP32

> [!CAUTION]
> **ESP8266** Connect to the FastLED library project version older[3.7.8](https://github.com/GyverLibs/Settings/issues/35#issuecomment-2691839527)It breaks the web server! We are waiting for fixes from ESP or FastLED

### Dependencies
- [GTL](https://github.com/GyverLibs/GTL) v1.3.0+
- [GyverDB](https://github.com/GyverLibs/GyverDB) v1.3.0+
- [StringUtils](https://github.com/GyverLibs/StringUtils) v1.4.30+
- [GyverHTTP](https://github.com/GyverLibs/GyverHTTP) v1.0.26+
- [BSON](https://github.com/GyverLibs/BSON) v2.1.0+
- [Stamp](https://github.com/GyverLibs/Stamp) v1.4.0+
- [Table](https://github.com/GyverLibs/Table) v1.2.0+
- [StreamIO](https://github.com/GyverLibs/StreamIO) v1.0.5+

> [!TIP]
> When installed from the PIO or Arduino IDE registry, all dependencies will be set automatically!

In addition (manually placed):
- [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)SettingsGyverWS/SettingsESPWS
- [ESPAsyncTCP](https://github.com/esphome/ESPAsyncTCP)SettingsAsync/SettingsAsyncWS
- [ESPAsyncWebServer](https://github.com/esphome/ESPAsyncWebServer)SettingsAsync/SettingsAsyncWS

## Versions
- v1.0
- v1.0.2
  - Added widget Confirm (pop-up confirmation window)
  - Custom pop-ups for Input (Input now works on AP WiFi spots on Xiaomi)

- v1.0.5
  - Added LED widget
  - Added file manager
  - Added OTA update
  - Added authorization and guest filter widgets
  - New style for Select

- v1.1.0
  - Added number widget
  - Creating a widget without an ID (automatic ID)
  - Creating a widget without a label (will be equal to the type of widget)
  - Linking an external variable to a read and write widget
  - Text File Editor in File Manager
  - Removed lag with the past state of the web snout when updating the page
  - Ability to completely remove GyverDB support
  - Small style fixes
  - The methods build(), value(), id() are now members (build, value, id) for short.

- v1.1.4
  - The Slider widget now has a clickable result, you can enter a number
  - DateTime and Date widgets now output and send unix time based on browser time zone

- v1.1.5
  - LED widget added color setting on/off
  - Confirm widget added the ability to connect bool variable for the result
  - Added the ability to send a pop-up window to Update

- v1.1.6
  - Added LabelNum widget for numbers
  - Added settings of webmords (timeout connections, sliders, updates)

- v1.1.8
  - Breakdown into packets for less memory use
  - Added by LabelFloat

- v1.1.12
  - Added color support for Slider and Switch widgets

- v1.2.0
  - Double slider added
  - Added logger (Web Serial)
  - Added support for custom widgets on JS
  - Added file download and download handlers

- v1.2.5
  - Added cleaning to Logger
  - Smart car scrolling in Log
  - Redesigned Password
  - Redesign of buttons
  - Processing event and browser connection status
  - Two Separator Styles for Row
  - Added HTML widget
  - Added button widget with press and release events
  - Input widget added regex + length limit (for char[])
  - Date and DateTime widgets can be set a time zone to display
  - Added time synchronization from the browser
  - Added widget output images from the Internet or flash drive
  - Added stream widget from ESP32-CAM camera
  - Tab widget added
  - Update with color
  - Setting up the color scheme of the entire web-mold
  - Expanded documentation, examples added

- v1.3.0
  - Websocket support added to all three server versions
  - Added SD card support at the same time as Flash memory
  - Sending updates from anywhere in the program (by websocket)
  - Added chart widget: 4 types of charts with support for binary tables and csv
  - Joystick widget added
  - In the logger added backlight with the color of the lines, taking into account their status (error, warning, info)
  - Added option grouping to Select widget
  - When requesting a Reload reboot, the current open menu is saved unless the hierarchy and menu names have changed.
  - Added widget Link
  - Minimum and maximum added to the number
  - Minor corrections and improvements

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
