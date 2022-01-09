// TZ.h this file in -> AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\cores\esp8266\TZ.h
#include <TZ.h>                         // Built-in - pick a value from TZ.h
#include <coredecls.h>                  // Built-in - settimeofday_cb()      // built in ESP8266 library
#include <time.h>                       // Built-in - time() ctime()         // built in ESP8266 library
#include <sys/time.h>                   // Built-in - struct timeval         // built in ESP8266 library
#include <sntp.h>                       // Built-in - sntp_servermode_dhcp() // built in ESP8266 library
// initial time (possibly given by an external RTC)
#define UTC_TEST    1627776001          // Date and time (UTC): Sunday, 1 August 2021 0:00:01
#define MY_TZ       TZ_Europe_Athens    // "EET-2EEST,M3.5.0/3,M10.5.0/4" 
#define SECS_IN_DAY (24 * 60 * 60)      // Day for Time From Boot

// for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);
//////////////////////////////////////////////////////////////////////////


// ======================================================================
// NTP_setup
// ======================================================================
void NTP_setup() {
  // setup RTC time
  // it will be used until NTP server will send us real current time
  now = UTC_TEST;
  tv = { now, 0 };
  settimeofday(&tv, nullptr);

  // install callback - called when settimeofday is called (by SNTP or us)
  // once enabled (by DHCP), SNTP is updated every hour
  settimeofday_cb(time_is_set_scheduled);
  // NTP servers may be overriden by your DHCP server for a more local one
  configTime(MY_TZ, ESPconfig.ntp1, ESPconfig.ntp2);
  // OPTIONAL: disable obtaining SNTP servers from DHCP
  sntp_servermode_dhcp(0); // 0: disable obtaining SNTP servers from DHCP (enabled by default)

  while (!FlagNTP)
    delay (100);
} //== Close NTP_setup ====


// ======================================================================
// time_is_set_scheduled
// ======================================================================
void time_is_set_scheduled() {
  static time_t  TestingTime;

  yield();
  // everything is allowed in this function
  if (time_machine_days == 0) {
    time_machine_running = !time_machine_running;
  }
  // time machine
  if (time_machine_running) {
    yield();
    if (time_machine_days == 0) {
      PrintShortLines();
      printf ("--- Settimeofday() has been called ---\n");
    }
    now = time (nullptr);
    const tm* tm = localtime (&now);
    /*
      Serial.printf ("future=%3ddays: DST=%s - ",
                   time_machine_days,
                   tm->tm_isdst ? "true " : "false");
      Serial.print (ctime(&now));
    */
    gettimeofday(&tv, nullptr);
    constexpr int days = 30;
    time_machine_days += days;
    if (time_machine_days > 360) {
      tv.tv_sec -= (time_machine_days - days) * 60 * 60 * 24;
      time_machine_days = 0;

    } else {
      tv.tv_sec += days * 60 * 60 * 24;
    }
    settimeofday (&tv, nullptr);
  } else {

    TestingTime = time (nullptr);
    if (TestingTime < UTC_TEST) {
      printf ("\r\n--- ERROR --- NTP service provides incorrect time - \n\n");
      Serial.printf_P(PSTR("UTC_TEST    : %d\n"), UTC_TEST);
      Serial.printf_P(PSTR("TestingTime : %d\n\n"), TestingTime);
      FlagIncorrectTime = true ;
    } else {
      FlagIncorrectTime = false ;
    }

    FlagNTP = true ;
    strftime(BufNTPLastSync, sizeof(BufNTPLastSync), "%F - %T", getCurrentTime()); //
    printf ("Sync from NTP  : %s\n", BufNTPLastSync);
    yield();
    Print_Show();
  }
}// == Close time_is_set_scheduled ===


// ======================================================================
// getCurrentTime
// ======================================================================
struct tm*  getCurrentTime() {
  time ( & now);
  return localtime(&now);
}//== Close getCurrentTime ===


// ======================================================================
// GetLocalTime
// ======================================================================
void GetLocalTime() {
  timeinfo  = getCurrentTime();
  DayOfWeek = timeinfo->tm_wday;
  Hour      = timeinfo->tm_hour;
  Minute    = timeinfo->tm_min;
  Sec       = timeinfo->tm_sec;
} //=== close GetLocalTime ===


// ======================================================================
// printTime
// ======================================================================
void printTime(struct tm* t)  { // http://zetcode.com/articles/cdatetime/
  // Format time as string see: http://www.cplusplus.com/reference/ctime/strftime/
  strftime(BufTime, sizeof(BufTime),  "%F - %T", t); // The Local time is: 2021-01-20 - 22:19:44
  printf("Η τοπική ώρα   : %s\n", BufTime);
}//== Close printTime ===


// ======================================================================
// TimeFromBoot
// ======================================================================
void TimeFromBoot() {
  struct timespec ts;
  clock_gettime(0, &ts);
  long days = ts.tv_sec / SECS_IN_DAY;
  /*
    if (days > 0)
    Serial.printf ("%ld days + ", days);

    Serial.printf ("%02d:%02d",
                 (int) (ts.tv_sec % SECS_IN_DAY) / 3600,
                 (int) (ts.tv_sec % 3600) / 60);
    Serial.printf ("\n");
  */
  snprintf_P(Time_From_Boot, sizeof(Time_From_Boot),
             PSTR("%ld day(s) + %02d:%02d"), days,
             (int) (ts.tv_sec % SECS_IN_DAY) / 3600,
             (int) (ts.tv_sec % 3600) / 60);
} //== Close TimeFromBoot ===


// ======================================================================
// PrintTimeFromBoot
// ======================================================================
void PrintTimeFromBoot() {
  TimeFromBoot();
  Serial.printf_P(PSTR("Time from boot : %s\n"), Time_From_Boot);
} //==  PrintTimeFromBoot ===


// ======================================================================
// SNTPservername
// ======================================================================
void SNTPservername() {
#if LWIP_VERSION_MAJOR > 1
  // LwIP v2 is able to list more details about the currently configured SNTP servers
  for (int i = 0; i < SNTP_MAX_SERVERS; i++) {
    yield();
    IPAddress sntp = *sntp_getserver(i);
    const char* name = sntp_getservername(i);
    if (sntp.isSet()) {
      Serial.printf_P(PSTR("NTP server %d   : "), i);
      if (name) {
        Serial.printf_P(PSTR("%s\n"), name);
        printf ("NTP server IP  : %s\n", sntp.toString().c_str());
        strlcpy(NTP_server, name, sizeof (NTP_server));
        strlcpy(NTP_server_ip, sntp.toString().c_str(), sizeof (NTP_server_ip));
      } else {
        printf ("%s \n", sntp.toString().c_str());
      }
    }
  }
#endif
} //== Close SNTPservername ===


// ======================================================================
// Time_Zone
// ======================================================================
void Time_Zone() {
  printf ("Time Zone      : %s \n", getenv("TZ") ? : "(none)"); // timezone
} //== Close Time_Zone ===


// ======================================================================
// Print_Show
// ======================================================================
void Print_Show() {
  PrintTimeFromBoot();
  Time_Zone();
  SNTPservername();
  printTime(getCurrentTime());
  PrintShortLines();
  yield();
} //== Close Print_Show ===



/*********************************
  END Page
**********************************/
