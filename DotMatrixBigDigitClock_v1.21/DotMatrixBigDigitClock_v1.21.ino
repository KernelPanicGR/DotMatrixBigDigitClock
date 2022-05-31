/*
    https://github.com/KernelPanicGR/DotMatrixBigDigitClock
     _____________________________________
    | Arduino: IDE 1.8.19                 |
    | Board  : ESP8266 WEMOS D1R1         |
    | Core   : 3.0.2                      |
    | Project: Dot Matrix Web Clock       |
    | Version: 1.21                       |
    | Created: 19/08/2021                 |
    | Las Mod: 22/05/2022                 |
    | Author : Kernel Panic               |
    |_____________________________________|
     _____________________________________
    |       D1 mini GPIO Pin mapping      |
    |     ----------------------------    |
    | D0  = 16 | GPI/O                    |
    | D1  =  5 | SCL                      |
    | D2  =  4 | SDA                      |
    | D3  =  0 | GPI/O                    |
    | D4  =  2 | BUILTIN_LED              |
    | D5  = 14 | SCK                      |
    | D6  = 12 | MISO                     |
    | D7  = 13 | MOSI                     |
    | D8  = 15 | SS                       |
    | D10 =  1 | RST                      |
    | A0       | ADC0                     |
    |_____________________________________|
     _____________________________________
    | ESP8266 SPI pin ->  LED Matrix Pin  |
    |       D5  = 14  ->  CLK             |
    |       D7  = 13  ->  DIN             |
    |       D8  = 15  ->  CS              |
    |-------------------------------------|
    | ESP8266 I2C pin ->  bme Pin         |
    |       D1  =  5  ->  SCL             |
    |       D2  =  4  ->  SDA             |
    |_____________________________________|

  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
  Reference:
  MajicDesigns/MD_Parola        -> https://github.com/MajicDesigns/MD_Parola
  Benoît Blanchon/ArduinoJson   -> https://github.com/bblanchon/ArduinoJson
  Assistant for ArduinoJson     -> https://arduinojson.org/v6/assistant
  more info for ArduinoJson     -> https://arduinojson.org

  ConvertDataTypes              -> https://www.convertdatatypes.com/Convert-bool-to-char-in-CPlusPlus.html
  HTML Minifier (v4.0.0)        -> https://kangax.github.io/html-minifier/
  json editor online            -> https://jsoneditoronline.org/

  Upload LittleFS files         -> https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases
  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/


#if !(defined(ESP8266))
#error This code is intended to run only on the ESP8266 boards ! Please check your Tools -> Board setting.
#endif

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
const char* MY_SKETCH = __FILE__; // Project Name
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <FS.h>                 // Built-in
#include <LittleFS.h>           // Built-in
#include <ESP8266WiFi.h>        // Built-in
#include <Wire.h>               // Built-in
#include <SPI.h>                // Built-in
#include <ESPAsyncTCP.h>        // version: 1.1.1,  url: https://github.com/me-no-dev/AsyncTCP
#include <ESPAsyncWebServer.h>  // version: 1.2.3,  url: https://github.com/me-no-dev/ESPAsyncWebServer
#include <SPIFFSEditor.h>       // Built-in ESPAsyncWebServer Lib
#include <ArduinoJson.h>        // version: 6.19.4, url: https://github.com/bblanchon/ArduinoJson
#include <MD_MAX72xx.h>         // version: 3.3.0,  url: https://github.com/MajicDesigns/MD_MAX72XX
#include <MD_Parola.h>          // version: 3.5.6,  url: https://github.com/MajicDesigns/MD_Parola
#include <Adafruit_Sensor.h>    // url: https://github.com/adafruit/Adafruit_Sensor
#include <Adafruit_BME280.h>    // version: 2.2.2    url: https://github.com/adafruit/Adafruit_BME280_Library
#include <PolledTimeout.h>      // Built-in
#include "common.h"
#include "MyFonts.h"
#include <ESP8266mDNS.h>        // Built-in     info url: https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266mDNS


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
FS* fileSystem = &LittleFS;
LittleFSConfig fileSystemConfig = LittleFSConfig();
FSInfo fs_info;

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket wsBright("/brightness");
AsyncWebSocket wsTime("/time");
AsyncEventSource eventWeather("/eventweather");
AsyncEventSource eventinfo("/eventinfo");

Adafruit_BME280     bme;

// polledTimeout
//esp8266::polledTimeout::periodicMs Trigger500ms(500);
esp8266::polledTimeout::periodicMs TriggCleanup(987);
////////////////////////////////////////////////////////


// ======================================================================
// setup function
// ======================================================================
void setup() {
  Serial.begin(74880);
  while (!Serial) yield();
  // Serial.setDebugOutput(true);

  // Disable the WiFi persistence to avoid any re-configuration that may erase static lease when starting softAP
  WiFi.persistent(false);

  WiFi.mode(WIFI_OFF); // turn WiFi off.
  delay(100);

  pinMode(MY_LED, OUTPUT );
  digitalWrite (MY_LED, LED_BUILTIN_OFF);

  DBGPSTRF("\r\n--------------- Booting ESP ---------------------------\n\r");
  DBGPSTRF("Arduino IDE      : %d.%d.%d\n", ARDUINO / 10000, ARDUINO % 10000 / 100, ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10);
  DBGPSTRF("On Board         : %s\nUsing            : %s\nStarting project : %s \n", ARDUINO_BOARD, "LittleFS", __FILE__ );
  //DBGPSTRF("ESP Full Version : %s\n", ESP.getFullVersion().c_str());
  DBGPSTRF("Author           : Kernel Panic\n");
  DBGPSTRF("Project Date     : %s\n", __TIMESTAMP__ );
  DBGPSTRF(DASHED_LINES);

  DotMatrix_setup();
  Display_Booting();
  Sensor_setup();
  FileSystem_setup();
  WiFiEvents_setup();
  WiFi_setup();
  WebSocket_setup();
  WebServer_setup();
  NTP_setup();
  printInfo();
  ClearDisplay();
  DisplayMyIntro();
  DisplayConnectInfo();
  PrintTimeFromBoot();

  TriggCleanup.reset();

}// == Close setup ===


/* ======================================================================
  loop
  ======================================================================= */
void loop() {
  MDNS.update();

  DotMatrixDisplay();

  if (TriggCleanup) {
    wsBright.cleanupClients();
    wsTime.cleanupClients();
  }

  if (FlagEventWather)
    SendSensorsData();

  if (FlagEventInfo)
    SendEvent_Info();

  if (FlagWsTime)
    SendWebSocketCurrentTime();

  if (FlagRestart)
    restartESP();

  yield(); // -- Yield should not be necessary, but cannot hurt either.
} // == Close Loop ===


/* ======================================================================
  printInfo
  ======================================================================= */
void printInfo() {
#if DEBUGLEVEL > 1// DEBUG SERIAL
  FlashMode_t ideMode = ESP.getFlashChipMode();

  LittleFS.info(fs_info);

  DBGPSTRF("\r\n===========  i n f o r m a t i o n s  ==================\n");
  //Project info
  DBGPSTRF("\r\t--- Project info ---\n");
  DBGPSTRF("Sketch Name       : %s\n",     __FILE__); //
  DBGPSTRF("Build             : %s\n",     __TIMESTAMP__ );
  DBGPSTRF("Sketch Size       : %s \n",    formatBytes(ESP.getSketchSize()));  // the size of the current sketch
  DBGPSTRF("Free Sketch Space : %s \n",    formatBytes(ESP.getFreeSketchSpace()));  //  free sketch space

  // ESP info -> specific APIs  https://arduino-esp8266.readthedocs.io/en/latest/libraries.html
  DBGPSTRF("\r\t--- ESP info ---\n");
  DBGPSTRF("My Board          : %s \n",    ARDUINO_BOARD);
  DBGPSTRF("Chip ID           : %d \n",    ESP.getChipId()); // chip ID as a 32-bit integer.
  DBGPSTRF("Flash Chip ID     : %0d\n",    ESP.getFlashChipId()); // the flash chip ID as a 32-bit integer.
  DBGPSTRF("ESP Mac Address   : %s\n",     WiFi.macAddress().c_str());
  DBGPSTRF("SDK ver           : %s\n",     ESP.getSdkVersion());
  DBGPSTRF("Core Version      : %s\n",     ESP.getCoreVersion().c_str()); // a String containing the core version.
  DBGPSTRF("Boot Version      : %u\n",     ESP.getBootVersion());
  DBGPSTRF("Reset reason      : %s\n",     ESP.getResetReason().c_str());
  DBGPSTRF("MCU Frequency     : %d MHz\n", ESP.getCpuFreqMHz());
  DBGPSTRF("Flash Speed       : %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
  DBGPSTRF("Flash Chip Mode   : %s \n",    (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  DBGPSTRF("Real Flash Size   : %s \n",    formatBytes(ESP.getFlashChipRealSize()));  // the real chip size in MB
  //DBGPSTRF("Free Heap         : %s \n",    formatBytes(ESP.getFreeHeap()));  // free heap size
  DBGPSTRF("FS Total          : %s \n",    formatBytes(fs_info.totalBytes));
  DBGPSTRF("FS Used           : %s \n",    formatBytes(fs_info.usedBytes));
  // DBGPSTRF("Fragmentation     : %d%%\n",   ESP.getHeapFragmentation()); // fragmentation metric (0% is clean, more than ~50% is not harmless)

  //Wi-Fi info
  DBGPSTRF("\r\t--- WiFi info ---\n");
  DBGPSTRF("Connected to SSID : %s\n",     WiFi.SSID().c_str());
  DBGPSTRF("WiFi Channel      : %d\n",     WiFi.channel());
  DBGPSTRF("RSSI signal       : %d dBm\n", WiFi.RSSI());
  DBGPSTRF("IP address        : %s\n",     WiFi.localIP().toString().c_str());
  DBGPSTRF("Subnet Mask       : %s\n",     WiFi.subnetMask().toString().c_str());
  DBGPSTRF("Gateway IP        : %s\n",     WiFi.gatewayIP().toString().c_str());
  DBGPSTRF("DNS               : %s\n",     WiFi.dnsIP().toString().c_str());

  //SPI pin info
  DBGPSTRF("\r\t--- SPI -> ESP pin ---\n");
  DBGPSTRF("ESP pin MOSI      : %d \n", (MOSI));
  DBGPSTRF("ESP pin SS        : %d \n", (SS));
  DBGPSTRF("ESP pin SCK       : %d \n", (SCK));
  DBGPSTRF("\r========== End informations ============================\n\n\r");
#endif // DEBUGLEVEL > 1
}// == Close printInfo ===



/*********************************
  END Page
**********************************/
