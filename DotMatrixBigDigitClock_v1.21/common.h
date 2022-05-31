#ifndef COMMON_H
#define COMMON_H
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//#define OFFICE                                      // Uncomment to enable, OFFICE confing

#define DEBUGLEVEL  5                               // Debug Level from 0 to 5 => 0 = no print, 5 = print all

//----- ESP Wi-Fi networks Settings ------------------------------------------
#ifdef  OFFICE  // OFFICE WiFi Config
#define MY_SSID         "-*****-"                    // Set these to your WiFi desired credentials.
#define MY_PASS         "-*******-"                  // Set these to your WiFi desired credentials.
#define USE_DHCP         false                       // false = Static IP, true = DHCP (Dynamic Host Configuration Protocol)
#define STATIC_IP       IPAddress(192, 168, 0, 84)
#define GATEWAY_IP      IPAddress(192, 168, 0, 1)
#define NETMASK_IP      IPAddress(255, 255, 255, 0)
#define DNS1_IP         IPAddress(195, 170, 0, 1)
#define DNS2_IP         IPAddress(195, 170, 2, 2)
#define HTTP_PORT       8084                        // port for webserver
#define NTP_SERVER_1    "10.10.29.11"               // OFFICE primary
#define NTP_SERVER_2    "10.10.29.12"               // OFFICE secondary 
#else           // Home WiFi Config
#define MY_SSID         "KernelPanic"               // Set these to your WiFi desired credentials.
#define MY_PASS         "0123456789"                // Set these to your WiFi desired credentials.
#define USE_DHCP         true                       // false = Static IP, true = DHCP (Dynamic Host Configuration Protocol)
#define STATIC_IP       IPAddress(192, 168, 1, 84)
#define GATEWAY_IP      IPAddress(192, 168, 1, 1)
#define NETMASK_IP      IPAddress(255, 255, 255, 0)
#define DNS1_IP         IPAddress(192, 168, 1, 1)
#define DNS2_IP         IPAddress(8, 8, 8, 8)       // Google DNS Server IP
#define HTTP_PORT       8084                        // port for webserver
#define NTP_SERVER_1    "gr.pool.ntp.org"           // Greece NTP time server
#define NTP_SERVER_2    "pool.ntp.org"              // Global NTP time server
#endif  // OFFICE
//-------------------------------------------------------------

#define AP_SSID         "DM-APclock"                // Set WiFi Access Point SSID credentials
#define AP_PASS         "0123456789"                // Set WiFi Access Point Password credentials
#define DNS_NAME        "dm-clock"                  // provide a domain name
#define HTTP_USERNAME   "admin"                     // 
#define HTTP_PASS       "0123456789"                // 

#define MY_LED          LED_BUILTIN                 // LED_BUILTIN -> On Board Led 2 = D4
#define ONE_WIRE_BUS    4                           // 4 = D2 -> SDA pin
#define LED_BUILTIN_ON  LOW                         // only for ESP8266 LED_BUILTIN
#define LED_BUILTIN_OFF HIGH                        // only for ESP8266 LED_BUILTIN
#define LED_ON          HIGH
#define LED_OFF         LOW
#define TIME_OFF        720                         // 960 ms 
#define TIME_ON         40                          // 40 ms

// Labels parameters For json file
#define LBL_WIFI_SSID   "WiFiSSID"
#define LBL_WIFI_PASS   "WiFiPASS"
#define	LBL_BRIGHTNESS  "BrightVal"             // Brightness value
#define	LBL_HOSTNAME    "HostName"
#define	LBL_DHCP_FLAG   "DHCP_FLAG"
#define	LBL_HTTP_PORT   "HTTPport"
#define	LBL_MY_IP       "IPaddress"
#define	LBL_SUB_MASK    "SubMask"
#define	LBL_GATEWAY     "GateWay"
#define	LBL_DNS_1       "dns_1"
#define	LBL_DNS_2       "dns_2"
#define LBL_MY_TZ       "My_TZ"
#define	LBL_NTP_SYNC    "NTPsync"
#define LBL_NTP_1       "ntp_1"
#define LBL_NTP_2       "ntp_2"

// sensors
#define BME_ADDRES      0x76                        // set I2C Address bme : 0x76 or 0x77
//#define         SEALEVELPRESSURE_HPA (1013.25)

#define Out_1           16                          // D0  = 16 |
#define Out_2           0                           // D3  =  0 |
#define Out_3           2                           // D4  =  2 | BUILTIN_LED
#define Out_4           14                          // D5  = 14 | SCK
#define Out_5           12                          // D6  = 12 | MISO
#define Out_6           13                          // D7  = 13 | MOSI
#define Out_7           15                          // D8  = 15 | SS


// Debugging Serial
#define DBGPSTRF(f,...) do { Serial.printf(PSTR(f), ##__VA_ARGS__); } while (0)
//#define DBGPSTRF(f,...) do { printf(PSTR(f), ##__VA_ARGS__); } while (0)
#define DASHED_LINES     "\r-------------------------------------------------------\n\r"

IPAddress   staticIP;
IPAddress   netMaskIP;
IPAddress   gatewayIP;
IPAddress   dns1;
IPAddress   dns2;

char        sta_ssid[32]        = {0};
char        sta_pass[32]        = {0};
char        MYTZ[30]            = {0};
char        ntp1[20]            = {0};
char        ntp2[20]            = {0};
char        Hostname[20]        = {0};
char        BufTime[40]         = {0};
char        BufNTPLastSync[32]  = {0};
char        NTP_server[20]      = {0};
char        NTP_server_ip[17]   = {0};

bool        fsOK,
            FlagSaveSettings,
            timeSync,
            FlagRestart,
            FlagSensors,
            FlagIncorrectTime,
            FlagDHCP,
            FlagWsTime,
            FlagEventWather,
            FlagEventInfo;

uint16_t	NTPsyncEvery;// interval, synchronization with NTP server, in minutes.


// setup time
// https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
// https://www.iana.org/time-zones
#define GR_TZ       "EET-2EEST,M3.5.0/3,M10.5.0/4" // TZ_Europe_Athens
time_t  rawtime;
struct  tm *timeinfo;
uint8_t DayOfWeek, OLDwday = 8, Hour, Minute, Sec;


#endif // COMMON_H
/*********************************
  END Page
**********************************/
//

