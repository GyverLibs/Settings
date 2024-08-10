This is an automatic translation, may be incorrect in some places. See sources and examples!

# Settings
Library for creating a simple settings web interface on ESP8266/ESP32
- The web application weighs about 10 kb and is sewn to the program in the binary gzip form without the forth with files
- Comfortable builder control panel from sketch
- A dozen typical widgets with the possibility of unification into groups and nested menu
- system of authorization with different rights for authorized users and guests
- file manager and OTA (air update)
- Integration with the library [gyverdb] (https://github.com/gyverlibs/gyverdb) for fully automatic data storage
- Compact binary communication protocol
- Easily adapts to any HTTP server library, three versions are implemented from the box: Gyverhttp, standard ESP-WEBSERVER, ESPASYNCWEBSERVER
- This is [gyverhub] (https://github.com/gyverlibs/gyverhub) on minimum wagons

! [Promo] (/IMG/Promo.png)

## compatibility
ESP8266, ESP32

### Dependencies
- gtl v1.1.1+
- gyverdb v1.1.2+
- Stringutils v1.4.24+
- Gyverhttp v1.0.17+
- Gson v1.5.9+

<details>
<summary> platformio.ini </summary>

`` `ini
[env]
Framework = Arduino
Lib_Deps =
https://github.com/gyverlibs/settings.git
Esphome/Espasyncwebserver-Eesphome;For the version of Settingsasync
Esphome/Espasynctcp-Eesphome;For the version of Settingsasync

[env: d1_mini]
Platform = Espressif8266
Board = d1_mini
Upload_Speed ​​= 921600
Monitor_Speed ​​= 115200
Monitor_filters = ESP8266_Exception_Decoder, Default
Build_type = Debug
Board_Build.filesystem = Littlefs

[ENV: ESP32DEV]
Monitor_Speed ​​= 115200
Platform = Espressif32
Board = ESP32DEV
Upload_Speed ​​= 921600
Board_Build.filesystem = Littlefs

[ENV: ESP32-C3]
Monitor_Speed ​​= 115200
Platform = Espressif32
Board = ESP32DEV
Board_Build.mcu = ESP32C3
Upload_Speed ​​= 200000
Board_Build.f_cpu = 80000000L
Board_Build.filesystem = Littlefs
`` `
</details>

## Content
- [use] (#usage)
- [versions] (#varsions)
- [installation] (# Install)
- [bugs and feedback] (#fedback)

<a id="usage"> </a>

## Usage
### how it works
The web server is configured in the library, which sends webmord files when entering the IP board.Webmord files are sewn in the Progmem (to the program code) in binary form - you do not need to upload files to FS, everything works out of the box.A light html file is loaded each time, and scripts and styles are cramped by a browser to speed up loading.In fact, they are loaded once after updating the library.Webmord is a web application that communicates with the HTTP board.When loading, it requests a package with widgets and other information at the board, the package has a binary json format.

## Captive Portal
In all three server implementations, the DNS is configured to work as Captive Portal - if the ESP operates in access point (AP or AP_STA), then when connecting to the point, the browser window with the settings page will automatically open.

### database
The library is integrated with [gyverdb] (https://github.com/gyverlibs/gyverdb) - a relatively quick database for storing data from any type.Settings automatically reads and updates the data in the database, soCranberries are mental to study how to work with a database on the description page Gyverdb.When using Gyverdbfile, the database will automatically write to the file with changes, and the file system will take care of the optimal wear of the Flash memory.When starting, it is recommended to initialize the database by indicating the keys and the corresponding initial values ​​and types.These values ​​will be recorded only if the recording in the database does not yet exist.At the same time, the automatic renewal of the database works only for existing records, i.e.Settings will only work with nourishing cells and will not create new ones.Minimum example:
`` `CPP
// connect libraries and create databases and settings
#include <gyverdbfile.h>
#include <Littlefs.h>
Gyverdbfile DB (& Littlefs, "/data.db");

#include <settingSesp.h>
SettingSesp sett ("My settings", & db);

// declare a shutter of the database through macro.This is more convenient than "lines" and enum,
// and is also not afraid of changes in order or removal of keys from the middle of the list
Db_keys (keys,
Db_key (key1),
Db_key (key2),
Db_key (mykey),
);

// builder
VOID Build (Sets :: Builder & B) {
}

VOID setup () {
Serial.Begin (115200);
Serial.println ();

// wifi
Wifi.mode (wifi_sta);
Wifi.begin ("wifi_ssid", "wifi_pass");
when (wifi.status ()! = Wl_ConNECTED) {
Delay (500);
Serial.print (".");
}
Serial.println ();
Serial.println (wifi.localip ());

// settings
sett.begin ();
sett.onbuild (Build);

// launch of the file system
# IFDEF ESP32
Littlefs.Begin (True);// Format on Fail
# LSE
Littlefs.Begin ();
#endif

// Launch of the database and reading from the file
db.begin ();

// Initialization of the database initial values
// db.init (key1, "text");
// db.init (key2, 12345);
}

VOID loop () {
sett.tick ();
}
`` `

## biller
The package is assembled by a build in a builder - a function in a program that is called when a request from webmord comes.Inside the builder, you need to call the components in the order in which they should be in webmord.Calling the component adds information about it to the package.At this stage, the value is also reading for the widget according to its ID from the database, if it is connected and *the value is not indicated explicitly *.

> If you need the parameter to be read from the database, but you need to specify the argument after it - the value must be specified `Text ()` as it is made in the default function.For example, color for Label `B.Label (Key," Label! ", Text (), sets :: colors :: Mint);`.If you specify an empty line (`B.Label (Key," Label! "," ", Sets :: colors :: Mint);`) - an empty line will go as a value, i.e.The value is not from the database.

`` `CPP
Gyverdb DB;
Settingsgyver Sett ("My Settings", & DB);

VOID Build (Sets :: Builder & B) {
B.INPUT ("Input1" _H, "text");
}

VOID setup () {
sett.begin ();
sett.onbuild (Build);
}

VOID loop () {
sett.tick ();
}
`` `

When changing values ​​in webmord, she sends a request from the ID and the meaning of the component.If the database is connected, a new value is automatically written to the corresponding cell.A builder is also called, and at this stage the widget method with which the action was performed will return True.You can catch this for interaction with the program.Thus, the builder performs two functions: assembly of widgets and feedback when changing the value.
`` `CPP
VOID Build (Sets :: Builder & B) {
if (B.Input ("Input1" _H, "Text")) serial.println ("Input Changed");
}
`` `

In the builder, you can get information about which ID action came to interact with the program:
`` `CPP
VOID Build (Sets :: Builder & B) {
// you can find out if there was an action on the widget
if (B.Build (). ISACTION ()) {
Serial.print ("set: 0x");
Serial.print (B.Build (). ID (), hex);
Serial.print ("=");
Serial.println (B.Build (). Value ());
}
}
`` `

### containers
Widgets can be combined into containers.The container must be started and finished, since the data package is assembled linearly in order to optimize speed and memory.The `begin -stagnant method will always return True for the beauty of the organization of code in the conditions:
```CPP
VOID Build (Sets :: Builder & B) {
if (B.Begingroup ("Group 1")) {
B.INPUT ("Input1" _H, "text");

B.EndGroup ();// Close the group
}
}
`` `

The second option - all containers have a pair class that will open and close the container.It is necessary to create an object with any name and give it a builder:
`` `CPP
VOID Build (Sets :: Builder & B) {
{
Sets :: Group G (b, "Group 2");// should be the first in the block

B.INPUT ("Input1" _H, "text");
}
}
`` `

You can create an invested menu.The specified title will be displayed on the button and in the page heading at the entrance to the menu.All widgets and groups located in the block with the menu will be on a separate page.The nuclearment of the menu is unlimited.
`` `CPP
VOID Build (Sets :: Builder & B) {
B.INPUT ("Input1" _H, "Text 1");

{
sets :: menu g (b, "submenu");

B.INPUT ("Input2" _H, "Text 2");
}
}
`` `

A separate type of container - buttons, inside it you can add only the buttons:
`` `CPP
VOID Build (Sets :: Builder & B) {
{
Sets :: Buttons BTNS (B);

// button will return True when clicking
if (b.button ("btn1" _h, "button 1")) {
Serial.println ("Button 1");
}

if (B.Button ("btn2" _h, "button 2", sets :: colors :: blue)) {
Serial.println ("Button 2");
}
}
}
`` `


The system also implements the update mechanism - webmord periodically requests an update from the device.If the database is connected, then with changes in values ​​in the database somewhere in the program, the library will automatically send new values ​​to webmord (for example, if any value has changed using a button).Note: if webmord is open simultaneously from several browsers - the database will be renewed only the one that requested them first.You can also send your values ​​if the database is not connected or is not used for any widgets.To do this, you need to connect the update processor and manually send data on ID widget.
`` `CPP
VOID Build (Sets :: Builder & B) {
b.label ("lbl1" _h, "random");
B.Label ("LBL2" _H, "Millis ()", "", sets :: colors :: RED);
}
VOID update (Sets :: Updater & Upd) {
UPD.UPDATE ("LBL1" _H, RANDOM (100));
UPD.UPDATE ("LBL2" _H, Millis ());
}

VOID setup () {
sett.begin ();
sett.onbuild (Build);
sett.onupdate (update);
}
`` `

Webmord monitors the status of the device, with a communication loss, the Offline text will appear in the page title.After losing the connection, webmord will request information about widgets, this is very convenient when developing - for example, add a widget, load the firmware.During this time, webmord already understands that the device is offline and with the first successful connection, it displays relevant widgets.When changing the values ​​of webmord widgets, he also monitors the delivery of the package, when the communication error will appear the inscription error in the corresponding widget.

### Authorization
The system provides for authorization: if in the firmware, indicate the password different from the empty line - webmord will work in the “guest” mode: only widgets allowed to guests are displayed, the file manager and OTA are hidden and blocked.To enter the password, you need to go to the menu (right upper button) and press on the key.The gray key means that the authorization is disabled, the green is authorized, red - the wrong password.The password can contain any characters and have any length - in explicitly it is not stored and not transmitted.The password is stored in the browser and the authorization works automatically when the page reloads.

The Guest virtual container is provided for the separation of admin and guest access.If the password is installed and the client is not authorized, he will only see widgets from guest containers.For the correct operation, the guest container should not be interrupted by conventional containers.Example:

`` `CPP
if (B.Begingroup ("Group 1")) {
// Guests do not see
b.pass (kk :: pass, "password");

// Widgets that guests and admins see
{
Sets :: Guestaccess G (b);
B.Input (kk :: uintw, "uint");
B.INPUT (KK :: Intw,Cranberries "int");
B.INPUT (KK :: int64W, "int 64");
}

// Guests do not see
{
Sets :: Menu M (b, "subject sub");
B.Label (kk :: lbl2, "millis ()", "", sets :: colors :: red);
}

B.EndGroup ();
}
`` `
In the guest container, you can place several ordinary containers, such as groups.

## description of classes
- `settingsgyver` (*settingsgyver.h*) - on the webcerver Gyverhttp
- `settingSesp` (*settingSesp.h*) - on the standard ESP webcierver
- `settingsasync` (*settingsasync.h*) - on asynchronous Espasyncwebserver

## settingsbase/settingsgyver/settingSesp/settingsasync
`` `CPP
Settings (const string & title = "", gyverdb* db = nullptr);

// Install the password on the webmord.Empty line "" to turn off
Void setpass (Text Pass);

// Reload page.Can be called anywhere + in the update processor
VOID RELOAD ();

// Install page title
VOID settitle (Constation String & Title);

// Install the update period (default 2500MS), 0 to turn off
VOID setupdateperiod (Uint16_T PRD);

// connect the database
VOID Attachdb (Gyverdb* DB);

// Use automatic updates from the database (when the record changes, the new value will go to the browser)
VOID USEUTOUPDATES (BOOL USE);

// Bild processor
VOID Onbuild (BuildCallback CB);

// Processor processor
VOID Onupdate (UpdateCallback CB);

// ticker, call in the parental class
VOID Tick ();
`` `

### Builder
`` `CPP
// Info about build
Build Build ();

// Reload page (call in action)
VOID RELOAD ();

// containers
// allow unauthorized customers the following code
Bool Betinguest ();

// prohibit non -authorized customers
VOID Enguest ();

// group
Bool BeginGroup (Text Title = Text ());
VOID EndGroup ();

// Bethered menu
Bool Beginmenu (Text Title);
VOID ENDMENU ();

// Row buttons
Bool Beginbuttons ();
VOID Endbuttons ();

// Widgets
// Passive
// multi -line text
VOID Paragraph (Size_T ID, Text Label, Text Text = Text ());

// Text value
Void Label (Size_T ID, Text Label, Text Text = Text (), Uint32_T Color = Sets_default_color);
Void Label (Size_t ID, Text Label, Text Text, Sets :: Colors Color);

// without ID
Void Label (Text Label, Text Text = Text (), Uint32_t Color = Sets_default_color);
Void Label (Text Label, Text Text, Sets :: Colors Color);

// LED (turned on - green, turned off - red)
VOID LED (SIZE_T ID, Text Label, Bool Value);
VOID LED (Text Label, Bool Value);

// Active
// Entering text and numbers
Bool Input (Size_T ID, Text Label, Text Value = Text ());

// Password entering
// you can install a "plug" for a password, for example "***" so that the password does not display in the browser
Bool Pass (Size_T ID, Text Label, Text Value = Text ());

// Input of color, the result in the usual 24-bit format
Bool Color (Size_T ID, Text Label, Text Value = Text ());

// switch, result 1/0
Bool Switch (Size_T ID, Text Label, Text Value = Text ());

// Date, result in unix seconds
Bool Date (Size_T ID, Text Label, Text Value = Text ());

// Date and time, result in UNIX seconds
Bool DateTime (Size_T ID, Text Label, Text Value = Text ());

// Time, result in seconds from the beginning of the day
Bool Time (Size_T ID, Text Label, Text Value = Text ());

// Slider
Bool Slider (Size_t ID, Text Label, Float Min = 0, Float Max = 100, Float Step = 1, Text Unit = Text (), Text Value = Text ());

// button can be added both inside the buttons container, and as a single widget
Bool Button (Size_T ID, Text Label, Uint32_T Color = Sets_default_color);
Bool Button (Size_T ID, Text Label, Sets :: Colors Color);

// list of choice, options in the form of text are separated;
Bool Select (Size_T ID, Text Label, Text Options, Text Value = Text ());

// Confirmation window, for activation, send an empty update to its ID
Bool Confirm (Size_T ID, Text Label);
`` `

Here `Text` is a universal text format, takes lines in any form.When indicating `value`, its value will be sent different from the standard.Otherwise, the value from the database will be sent if it is connected.If you need a value as a value - use the constructor `vaLue`, for example `B.color (" color "," color ", value (my_color));` where `my_color` is` uint32_t`.

### Build
Info about build
`` `CPP
// Client is authorized (or password is not indicated, i.e. all admins)
Bool ISGranted ();

// ID widget (action)
size_t id ();

// Widget value (action)
Text & Value ();
`` `

### containers
`` `CPP
// Guest access container
Class guestaccess (Builder & B);

// Container group of widgets
Class Group (Builder & B, Text Title = Text ());

// container of an invested menu
Class Menu (Builder & B, Text Title);

// Container buttons
Class Buttons (Builder & B);
`` `

## updater
`` `CPP
// empty update (for example, for call confirm)
VOID update (size_t ID);

// update with text
VOID update (Size_T ID, Text Value);

// update with Float
VOID update (Size_t ID, Float Value, Uint8_t Dec = 2);

// Update with the number
VOID update (Size_T ID, <Any numerical type> value);
`` `

<a id="versions"> </a>

## versions
- V1.0
- V1.0.2
- Added a confirm widget (a pop -up confirmation window)
- custom pop -up windows for Input (Input now works on a view of AP wifi points on Xiaomi)
- V1.0.5
- Added LED widget
- Added file manager
- Added Ot Update
- Added authorization and guest filter of widgets
- New style for Select

<a id="install"> </a>
## Installation
- The library can be found by the name ** settings ** and installed through the library manager in:
- Arduino ide
- Arduino ide v2
- Platformio
- [download the library] (https://github.com/gyverlibs/settings/archive/refs/heads/main.zip) .Zip archive for manual installation:
- unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
- unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
- unpack and put in *documents/arduino/libraries/ *
- (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!

<a id="feedback"> </a>

## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code