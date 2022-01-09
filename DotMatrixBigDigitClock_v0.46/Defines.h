/*
    -- NTP servers --
  "gr.pool.ntp.org"   // Greece
  "pool.ntp.org"      // Global
  "time.google.com"   // google
  "ntp2.otenet.gr"    // otenet
*/

#ifndef Defines_h
#define Defines_h

#define         SKAI                          // Uncomment to enable SKAI confing
//#define DEBUG_SERIAL                          // Uncomment to enable Serial info
#define DEBUG2_SERIAL                           // Uncomment to enable Serial info

#define         FILE_FS         LittleFS
#define         FS_NAME         "LittleFS"

#ifdef          SKAI            // SKAI
#define         WIFI_SSID       "Service"
#define         WIFI_PASS       "6752881254"
#define         NTP_SERVER_1    "10.10.19.5"    // SKAI TV primary
#define         NTP_SERVER_2    "10.10.19.6"    // SKAI TV secondary 
#define         HTTP_PORT       8081

typedef struct DefultConfig {
  bool DHCP = false ; // false = Static IP, true = DHCP
  IPAddress stationIP   = IPAddress(192, 168, 0, 81);
  IPAddress gatewayIP   = IPAddress(192, 168, 0, 1);
  IPAddress netMaskIP   = IPAddress(255, 255, 255, 0);
  IPAddress dns1        = IPAddress(195, 170, 0, 1);
  IPAddress dns2        = IPAddress(195, 170, 2, 2);
  char  ntp1[20]        = NTP_SERVER_1;
  char  ntp2[20]        = NTP_SERVER_2;
} DefultConfig;
DefultConfig   ESPconfig;

#else   // Home
#define         ENABLE_MDNS
#define         DNS_NAME        "dm-clock"
#define         WIFI_SSID       "Stavros"
#define         WIFI_PASS       "stavros6947898456"
#define         NTP_SERVER_1    "gr.pool.ntp.org"   // Greece
#define         NTP_SERVER_2    "pool.ntp.org"      // Global
#define         HTTP_PORT       80

typedef struct DefultConfig {
  bool DHCP = true ; // false = Static IP, true = DHCP
  IPAddress stationIP   = IPAddress(192, 168, 1, 83);
  IPAddress gatewayIP   = IPAddress(192, 168, 1, 1);
  IPAddress netMaskIP   = IPAddress(255, 255, 255, 0);
  IPAddress dns1        = IPAddress(192, 168, 1, 1);
  IPAddress dns2        = IPAddress(8, 8, 8, 8);
  char  ntp1[20]        = NTP_SERVER_1;
  char  ntp2[20]        = NTP_SERVER_2;
} DefultConfig;
DefultConfig   ESPconfig;
#endif // SKAI

// Labels parameters For json file
#define         BRITGHTNS_JS    "BrightVal" // Brightness value
#define         HOSTNAME_JS     "HostName"
#define         DHCP_FLAG_JS    "DHCP_FLAG"
#define         HTTP_PORT_JS    "HTTPport"
#define         MY_IP           "IPaddress"
#define         SUB_MASK_JS     "SubMask"
#define         GATEWAY_JS      "GateWay"
#define         DNS_1_JS        "dns_1"
#define         DNS_2_JS        "dns_2"
#define         NTP_0_JS        "ntp_0"
#define         NTP_1_JS        "ntp_1"

#define         LED_BUILTIN     2           // 2 = D4 -> On Board Led
#define         ONE_WIRE_BUS    4           // 4 = D2 -> SDA pin
#define         LED_ON          LOW         // only for ESP8266 LED_BUILTIN
#define         LED_OFF         HIGH        // only for ESP8266 LED_BUILTIN

// sensors
#define         BME_ADDRES      0x76        // set I2C Address bme : 0x76 or 0x77
//#define         SEALEVELPRESSURE_HPA (1013.25)

const char*     http_username           = "admin";
const char*     http_password           = "0123456789";

// dot matrix
#define         HARDWARE_TYPE   MD_MAX72XX::ICSTATION_HW
#define         CS_PIN          15          // D8 or any pin 
#define         MAX_ZONES       4
#define         MAX_DEVICES     16
#define         inFX            PA_SCROLL_LEFT
#define         outFX           PA_SCROLL_LEFT
#define         SPEED_TIME      75          // in milliseconds
#define         ZONE_LOWER      0
#define         ZONE_one        1
#define         ZONE_UPPER      2
#define         ZONE_SecUP      3
#define         Out_1           16          // D0  = 16 |
#define         Out_2           0           // D3  =  0 |
#define         Out_3           2           // D4  =  2 | BUILTIN_LED
#define         Out_4           14          // D5  = 14 | SCK
#define         Out_5           12          // D6  = 12 | MISO
#define         Out_6           13          // D7  = 13 | MOSI
#define         Out_7           15          // D8  = 15 | SS
#define         maxBrightness   15

uint8_t         Brightness  = 7, DayOfWeek, Hour, Minute, Sec;
uint8_t         degC[]      = { 6, 3, 3, 56, 68, 68, 68 };   // Deg C

bool            LoadConfigFile(),
                SaveFSconfig(),
                Load_Brightness(),
                Save_FS_Brightness(),
                FlagSensors,
                FlagIncorrectTime,
                FlagWsTime              = false ,
                FlagEventWather         = false ,
                FlagEventInfo           = false ,
                FlagNTP                 = false ,
                time_machine_running    = false ,
                FlagRestart             = false ,
                FlagDHCP                = false ,
                Flag_Pacman             = true ,
                FlagEffe                = false ,
                Flag2ZonesB             = false ,
                Set_4_Zones             = false ,
                Set_2_Zones             = false ;

uint16_t        PAUSE_TIME              = 0,            // in milliseconds
                time_machine_days       = 0;            // 0 = now

char            Temperature[10]         = { 0 };        // Temperature
char            Buf_TimeL[6]            = { 0 };        // hh:mm LOWER
char            Buf_TimeUp[6]           = { 0 };        // hh:mm UPPER
char            Buf_Sec_UP[3]           = { 0 };        // :ss
char            Buf_time[13]            = { 0 };        // 2 zone Time buffer Zone Upper
char            BufMsg_0[200]           = { 0 };        // 2 zone message buffer Zone Lower
char            BufMsg_1[20]            = { 0 };        // 2 zone message buffer Zone Upper
char            BufGEN[100]             = { 0 };        // 2 zone message, buffer ZONE UPPER
char            NTP_server_ip[17]       = { 0 };
char            Time_From_Boot[30]      = { 0 };
char            BufTime[40]             = { 0 };
char            BufNTPLastSync[30]      = { 0 };
char            Hostname[20]            = { 0 };
char            NTP_server[20]          = { 0 };        //

// Sprite Definitions
const uint8_t   F_PMAN1 = 6 ;
const uint8_t   W_PMAN1 = 8 ;
const uint8_t PROGMEM   pacman1[F_PMAN1 * W_PMAN1] =    // gobbling pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};

const uint8_t           F_PMAN2 = 6;
const uint8_t           W_PMAN2 = 18;
const uint8_t PROGMEM   pacman2[F_PMAN2 * W_PMAN2] =    // ghost pursued by a pacman
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
};


// Dot Matrix parameters configuration block
struct cfgParameter_t
{
  uint8_t   speed_1;        // animation frame delay
  uint16_t  pause_1;        // animation pause in milliseconds
  uint8_t   speed_2;        // animation frame delay
  uint16_t  pause_2;        // animation pause in milliseconds
  textPosition_t align;     // text alignment at pause
  textEffect_t INeffe_1;    // text animation effect
  textEffect_t OUTeffe_1;   // text animation effect
  textEffect_t INeffe_2;    // text animation effect
  textEffect_t OUTeffe_2;   // text animation effect
  bool invert = false;      // inverted display
} DMcfg;


// setup time
static timeval  tv;
static timespec tp;
static time_t   now;
struct tm *timeinfo;


void PrintShortLines() {
  Serial.printf_P(PSTR("\r-------------------------------------------------------\n\r"));
}

#endif    //Defines_h

//////////////////////////////////////////////////////////////////////////////


/*********************************
  END Page
**********************************/
