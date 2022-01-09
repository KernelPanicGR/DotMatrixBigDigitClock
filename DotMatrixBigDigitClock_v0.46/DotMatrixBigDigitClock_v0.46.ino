/*
   _____________________________________
  | Arduino: IDE 1.8.16                 |
  | Board  : ESP8266 WEMOS D1R1         |
  | Core   : 3.0.2                      |
  | Project: Dot Matrix Big Digit Clock |
  | Version: 0.46                       |
  | First version: 19/03/2019 		    |
  | Last update  : 26/12/2021           |
  | Author       : kernel panic		    |
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


  Reference:
  MajicDesigns/MD_Parola        -> https://github.com/MajicDesigns/MD_Parola
  Benoît Blanchon/ArduinoJson   -> https://github.com/bblanchon/ArduinoJson
  Assistant for ArduinoJson     -> https://arduinojson.org/v6/assistant/
  ConvertDataTypes              -> https://www.convertdatatypes.com/Convert-bool-to-char-in-CPlusPlus.html
  json editor online            -> https://jsoneditoronline.org/

  Upload LittleFS files         -> https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases
*/


#if !(defined(ESP8266))
#error This code is designed to run on the ESP8266 platform! Please check your Tools->Board setting.
#endif

#include <FS.h>                 // Built-in
#include <LittleFS.h>           // Built-in
#include <ESP8266WiFi.h>        // Built-in
#include <Wire.h>               // Built-in
#include <SPI.h>                // Built-in
#include <ESPAsyncTCP.h>        // version: 1.1.1,  url: https://github.com/me-no-dev/AsyncTCP
#include <ArduinoJson.h>        // version: 6.18.5, url: https://arduinojson.org/  
#include <ESPAsyncWebServer.h>  // version: 1.2.4,  url: https://github.com/lorol/ESPAsyncWebServer
#include <SPIFFSEditor.h>       // Built-in ESPAsyncWebServer.h
#include <MD_MAX72xx.h>         // version: 3.3.0,  url: https://github.com/MajicDesigns/MD_MAX72XX
#include <MD_Parola.h>          // version: 3.5.6,  url: https://github.com/MajicDesigns/MD_Parola
#include <Adafruit_Sensor.h>    // url: https://github.com/adafruit/Adafruit_Sensor
#include <Adafruit_BME280.h>    // url: https://github.com/adafruit/Adafruit_BME280_Library
#include "defines.h"
#include "MyFonts.h"
#include <Schedule.h>           // Built-in
#include <PolledTimeout.h>      // Built-in
#ifdef ENABLE_MDNS // use mDNS
#include <ESP8266mDNS.h>        // Built-in
#endif // ENABLE_MDNS

////////////////////////////////////////////////////////
// ======================================================================
// Definition of global variables
// ======================================================================
const char ProjectName[40] = __FILE__;
AsyncWebServer server(HTTP_PORT);
AsyncWebSocket wsBright("/brightness");
AsyncWebSocket wsTime("/time");
AsyncEventSource eventWeather("/eventweather");
AsyncEventSource eventinfo("/eventinfo");
WiFiEventHandler gotIpEventHandler, disconnectedEventHandler;

// SPI hardware interface
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
Adafruit_BME280         bme;

// polledTimeout
esp8266::polledTimeout::periodicMs DisplayDots(500);

////////////////////////////////////////////////////////


// ======================================================================
// setup function
// ======================================================================
void setup() {
  Serial.begin(74880);
  // Wait for USB Serial.
  while (!Serial)  yield();

  // Serial.setDebugOutput(true);
  pinMode(LED_BUILTIN, OUTPUT );
  digitalWrite (LED_BUILTIN, LED_OFF);
  Serial.printf ("\r\n--------------- Booting ESP ---------------------------\n\r");
  Serial.printf_P(PSTR("Starting project  : %s\nUsing             : %s\nOn Board          : %s \n"),
                  (__FILE__), "LittleFS", ARDUINO_BOARD);
  Serial.printf_P(PSTR("Project Date      : %s\n"), (__DATE__ " Time: " __TIME__));

  PrintShortLines();
  DotMatrix_setup();
  Display_Booting();
  Sensor_setup();
  FileSystem_setup();
  WiFiEvents();
  WiFi_setup();
  WebSocket_setup();
  WebServer_setup();
  NTP_setup();
  printInfo();
  ClearDisplay();
  MyIntro();
  info_IP();
}// == Close setup ===




/* ======================================================================
  loop
  ======================================================================= */
void loop() {
#ifdef ENABLE_MDNS  // use mDNS
  MDNS.update();
#endif              // ENABLE_MDNS

  if (FlagEventWather)
    SendSensorsData();

  if (FlagEventInfo)
    SendDataInfo();

  if (FlagRestart)
    restartESP();

  if (FlagWsTime)
    SendWebSocketCurrentTime();

  DotMatrixDisplay();
  wsBright.cleanupClients();
  wsTime.cleanupClients();
  yield();
} // == Close Loop ===



/* ======================================================================
  Sensor_setup
  ======================================================================= */
void Sensor_setup() {
  Serial.printf_P(PSTR("\r\n--------- Start the BME sensor ------------------------\n\r"));

  if (! bme.begin(BME_ADDRES, &Wire)) {
    FlagSensors = false ;
    printf ("--- ERROR --- Could not find a valid BME280 sensor, check wiring! \n\r");
    printf ("--- Test with random valum \n\r");
  }
  else {
    FlagSensors = true ;
    Serial.println(" OK find a BME280 sensor");
    // indoor navigation
    /*
      Serial.println("-- Indoor Navigation Scenario --");
      Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
      Serial.println("0.5ms standby period, filter 16x");
    */
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,  // temperature
                    Adafruit_BME280::SAMPLING_X16, // pressure
                    Adafruit_BME280::SAMPLING_X1,  // humidity
                    Adafruit_BME280::FILTER_X16,
                    Adafruit_BME280::STANDBY_MS_0_5 );
  }
  PrintShortLines();
}// == Close Sensor_setup ===



/* ======================================================================
  restartESP
  ======================================================================= */
void restartESP() {
  FlagRestart = false ;
  digitalWrite (LED_BUILTIN, LED_OFF);
  WiFi.disconnect();
  delay(10);
  WiFi.mode(WIFI_OFF);
  printf ("\r\n\n\t---> R e s t a r t    E S P <--- \n\n\r");

  // DotMatrix Display
  //                              Ε   π    α    ν    ε    κ    κ    ί    ν    η    σ    η
  static char   txtUP[]     = "\x084\x0a7\x098\x0a4\x09c\x0a1\x0a1\x0e5\x0a4\x09e\x0a9\x09e";  // Επανεκκίνηση
  //                           Τ   η    ς     σ    υ    σ   κ    ε    υ    ή    ς
  static char   txtBOT[]    = "T\x09e\x0aa \x0a9\x0ac\x0a9\x0a1\x09c\x0ac\x0e3\x0aa";  // Της συσκευής
  Set2ZonesInfo();
  P.displayZoneText(ZONE_one, txtUP, PA_CENTER, 30, 4000, PA_OPENING_CURSOR, PA_OPENING_CURSOR);
  P.displayZoneText(ZONE_LOWER, txtBOT, PA_CENTER, 35, 4500, PA_OPENING_CURSOR, PA_OPENING_CURSOR);
  while (!P.getZoneStatus(ZONE_LOWER)) {
    yield();
    P.displayAnimate();
  }
  delay ( 1000 );
  ESP.restart();
  delay( 100 );
}// == Close restartESP ===



/* ======================================================================
  printInfo
  ======================================================================= */
void printInfo() {
#ifdef DEBUG_SERIAL
  printf ("\r\n==========  i n f o r m a t i o n s  ===================\n");
  printf ("\r\t--- Project info ---\n");
  Serial.printf_P(PSTR("Project Name      : %s\n"), __FILE__ ); //
  Serial.printf_P(PSTR("Project Date      : %s\n"), (__DATE__ " Time: " __TIME__));
  /*
    printf ("Arduino IDE       : %d.%d.%d\n%s\n",
    ARDUINO / 10000, ARDUINO % 10000 / 100,
    ARDUINO % 100 / 10 ? ARDUINO % 100 : ARDUINO % 10, ESP.getFullVersion().c_str());
  */
  //ESP info
  printf ("\r\t--- ESP info ---\n");
  Serial.printf_P(PSTR("My Board          : %s \n"), ARDUINO_BOARD);
  // ESP8266  https://arduino-esp8266.readthedocs.io/en/latest/libraries.html
  printf  ("ESP Mac Address   : %s\n", WiFi.macAddress().c_str());
  printf  ("SDK ver           : %s\n", ESP.getSdkVersion());
  Serial.printf_P(PSTR("Core Version      : %s\n"), ESP.getCoreVersion().c_str()); // a String containing the core version.
  Serial.printf_P(PSTR("Boot Version      : %u\n"), ESP.getBootVersion());
  Serial.printf_P(PSTR("Reset reason      : %s\n"), ESP.getResetReason().c_str());
  printf  ("MCU Frequency     : %d MHz\n", ESP.getCpuFreqMHz());
  printf  ("Flash Speed       : %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
  printf  ("Chip ID           : %d \n", ESP.getChipId()); // chip ID as a 32-bit integer.
  Serial.printf_P(PSTR("Flash Chip ID     : %0d\n"), ESP.getFlashChipId()); // the flash chip ID as a 32-bit integer.
  printf  ("Real Flash Size   : %d MB\n", (ESP.getFlashChipRealSize() / 1048576));  // free heap size.
  printf  ("Fragmentation     : %d%%\n", ESP.getHeapFragmentation()); // fragmentation metric (0% is clean, more than ~50% is not harmless)

  //Wi-Fi info
  printf ("\r\t--- WiFi info ---\n");
  printf ("Connected to SSID : %s\n", WiFi.SSID().c_str());
  Serial.printf_P(PSTR("WiFi Channel      : %d\n"), WiFi.channel());
  Serial.printf_P(PSTR("RSSI signal       : %d dBm\n"), WiFi.RSSI());
  Serial.printf_P(PSTR("IP address        : %s\n"), WiFi.localIP().toString().c_str());
  Serial.printf_P(PSTR("Subnet Mask       : %s\n"), WiFi.subnetMask().toString().c_str());
  Serial.printf_P(PSTR("Gateway IP        : %s\n"), WiFi.gatewayIP().toString().c_str());
  Serial.printf_P(PSTR("DNS               : %s\n"), WiFi.dnsIP().toString().c_str());

  //SPI pin info
  printf ("\r\t--- SPI -> ESP pin ---\n");
  Serial.printf_P(PSTR("ESP pin MOSI      : %d \n"), (MOSI));
  Serial.printf_P(PSTR("ESP pin SS        : %d \n"), (SS));
  Serial.printf_P(PSTR("ESP pin SCK       : %d \n"), (SCK));
  printf ("\r========== End informations ============================\n\r");
#endif // DEBUG_SERIAL
}// == Close printInfo ===



/*********************************
  END Page
**********************************/
