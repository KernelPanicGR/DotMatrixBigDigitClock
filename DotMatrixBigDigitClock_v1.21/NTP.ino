/*
  NTP-TZ-DST (v2). NetWork Time Protocol - Time Zone - Daylight Saving Time
  automatic time adjustment on Summer/Winter change (DST)

  https://github.com/esp8266/Arduino/blob/master/libraries/esp8266/examples/NTP-TZ-DST/NTP-TZ-DST.ino

  TZ.h this file in
  C:\Users\username\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\3.0.2\cores\esp8266\TZ.h
  or
  https://github.com/esp8266/Arduino/blob/master/cores/esp8266/TZ.h
  https://www.iana.org/time-zones
*/

#include <coredecls.h>	// Built-in ESP8266 library - settimeofday_cb()      
#include <time.h>		// Built-in ESP8266 library - time() ctime()
#include <sys/time.h>	// Built-in ESP8266 library - struct timeval
#include <sntp.h>		// Built-in ESP8266 library - sntp_servermode_dhcp()
#define UTC_TEST    1645797370  // to human = Friday, 25 February 2022 13:56:10 UTC (Coordinated Universal Time)

// initial time (possibly given by an external RTC) for testing purpose:
extern "C" int clock_gettime(clockid_t unused, struct timespec *tp);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


// ======================================================================
// sntp_update_delay_MS_rfc_not_less_than_15000
// ! optional change here if you want another NTP polling interval (default 1h)
// ======================================================================
uint32_t sntp_update_delay_MS_rfc_not_less_than_15000 ()  {
  return NTPsyncEvery * 60000UL; // every... synchronization with NTP server
}//== Close sntp_update_delay_MS_rfc_not_less_than_15000 ===


// ======================================================================
// getCurrentTime
// ======================================================================
struct tm*  getCurrentTime() {
  static time_t prevtime = UTC_TEST;
  rawtime = time(nullptr);

  if (rawtime >= prevtime ) {
    prevtime = rawtime;
    FlagIncorrectTime = false;
    return localtime(&rawtime);
  }
  else {
    FlagIncorrectTime = true;
#if DEBUGLEVEL > 2 // DEBUG SERIAL
    DBGPSTRF("\r\n---[ERROR]: The time() function failed\n");
#endif // DEBUGLEVEL > 2
    return localtime(&rawtime);
  }
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
// SNTPservername
// ======================================================================
void SNTPservername() {
  for (int i = 0; i < SNTP_MAX_SERVERS; i++) {
    IPAddress sntp = *sntp_getserver(i);
    const char* name = sntp_getservername(i);
    if (sntp.isSet()) {
#if DEBUGLEVEL > 4
      DBGPSTRF("sntp %d         : ", i);
#endif // > 4
      if (name) {
#if DEBUGLEVEL > 4
        DBGPSTRF("%s\n", name);
        DBGPSTRF("NTP server IP  : %s \n", sntp.toString().c_str());
#endif // > 4
        strlcpy(NTP_server, name, sizeof (NTP_server));
        strlcpy(NTP_server_ip, sntp.toString().c_str(), sizeof (NTP_server_ip));

      } else {

        DBGPSTRF("%s \n", sntp.toString().c_str());
      }
#if DEBUGLEVEL > 4
      DBGPSTRF("NTP IPv6?      : %s \n- Reachability : %o\n",
               sntp.isV6() ? "Yes" : "No",
               sntp_getreachability(i));
#endif // > 4
    }
    yield();
  }
}//== Close SNTPservername ===


// ======================================================================
// TimeFromBoot
// ======================================================================
char* TimeFromBoot() {
  static char Time_From_Boot[32]  = {0};
  struct timespec ts;
  clock_gettime(0, &ts);
  uint16_t  days =  ts.tv_sec / 86400UL; // Day in seconds 24h * 60m * 60s = 86400

  if (days > 0)
  {
    snprintf_P(Time_From_Boot, sizeof(Time_From_Boot),
               PSTR("%ld days + %02d:%02d [h:m]"), days,
               (int) (ts.tv_sec % 86400UL) / 3600UL,
               (int) (ts.tv_sec % 3600UL) / 60UL);
    return Time_From_Boot;
  }
  else
  {
    snprintf_P(Time_From_Boot, sizeof(Time_From_Boot),
               PSTR("%02d:%02d:%02d [h:m:s]"),
               (int) (ts.tv_sec % 86400UL) / 3600UL,
               (int) (ts.tv_sec % 3600UL) / 60UL,
               (int) (ts.tv_sec % 60UL));
    return Time_From_Boot;
  }
} //== Close TimeFromBoot ===


// ======================================================================
// PrintTimeFromBoot
// ======================================================================
void PrintTimeFromBoot() {
  DBGPSTRF("Time from boot : %s\n", TimeFromBoot());
} //==  PrintTimeFromBoot ===


// ======================================================================
// Print_GR_Date
// ======================================================================
void Print_GR_Date(struct tm* ptm) {
  // http://www.cplusplus.com/reference/ctime/tm/
  static const char* const PROGMEM DAY_NAMES[]   =
  { "Κυριακή", "Δευτέρα", "Τρίτη", "Τετάρτη", "Πέμπτη", "Παρασκευή", "Σάββατο"};

  static const char* const PROGMEM MONTH_NAMES[] =
  { "Ιανουαρίου", "Φεβρουαρίου", "Μαρτίου", "Απριλίου", "Μαΐου", "Ιουνίου",
    "Ιουλίου", "Αυγούστου", "Σεπτεμβρίου", "Οκτωβρίου", "Νοεμβρίου", "Δεκεμβρίου"
  };

  char Buf_GR[120] = {0};
  snprintf_P(Buf_GR, sizeof(Buf_GR), "%s %02d %s %04d \nΗ ώρα  είναι   : %02d:%02d:%02d (%s) \n",
             DAY_NAMES[ptm->tm_wday], ptm->tm_mday, MONTH_NAMES[ptm->tm_mon], ptm->tm_year + 1900,
             ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_isdst ? "Θερινή ώρα " : "Χειμερινή ώρα" );
  DBGPSTRF("Σήμερα είναι   : %s \n", Buf_GR);
}//== Close Print_GR_Date ===


// ======================================================================
// PrintUTCtime
// ======================================================================
void PrintUTCtime() { // http://zetcode.com/articles/cdatetime/
  getCurrentTime();
  strftime(BufTime, sizeof(BufTime),  "%F - %T ", gmtime(&rawtime)); //
  DBGPSTRF("UTC time       : %s\n", BufTime);
}//== Close PrintUTCtime ===


// ======================================================================
// printTime
// ======================================================================
void printTime(struct tm* t)  { // http://zetcode.com/articles/cdatetime/
  // Format time as string see: http://www.cplusplus.com/reference/ctime/strftime/
  strftime(BufTime, sizeof(BufTime),  "%F - %T", t); // Looks like: 2022-02-20 - 22:19:44
  DBGPSTRF("Η τοπική ώρα   : %s %s\n", BufTime, t->tm_isdst ? "Θερινή ώρα" : "Χειμερινή ώρα");
}//== Close printTime ===


// ======================================================================
// PrintINFO_ntp_server
// ======================================================================
void PrintINFO_ntp_server() {
#if DEBUGLEVEL >= 1
  DBGPSTRF("NTP server name: %s \n", NTP_server);
  DBGPSTRF("NTP server IP  : %s \n", NTP_server_ip);
  DBGPSTRF("NTP Last Sync  : %s \n", BufNTPLastSync);
  DBGPSTRF("Time from boot : %s \n", TimeFromBoot());
  DBGPSTRF(DASHED_LINES);
#endif // >= 1
}//== Close PrintINFO_ntp_server ==


// ======================================================================
// PrintLocalTime
// ======================================================================
void PrintLocalTime() { // http://zetcode.com/articles/cdatetime/
  //  DBGPSTRF("Time Zone      : %s \n", getenv("TZ") ? : "(none)");
  strftime(BufTime, sizeof(BufTime),  "%F - %T (UTC %z)", getCurrentTime());
  DBGPSTRF("Η ώρα Αθήνας   : %s\n", BufTime);
}//== Close PrintLocalTime ===


// ======================================================================
// PrintTheSec
// ======================================================================
void PrintTheSec() {
  static time_t  prevTime = UTC_TEST;
  rawtime = time(nullptr);

  if (rawtime > prevTime ) {
    prevTime = rawtime;
    timeinfo = localtime(&rawtime);
    strftime(BufTime, sizeof(BufTime), "%F - %T", timeinfo);
    DBGPSTRF("Display Time   : %s (%s)\n", BufTime,  timeinfo->tm_isdst ? "Θερινή ώρα" : "Χειμερινή ώρα");
  }
}//== Close PrintTheSec ==


// ======================================================================
// Print_Show
// ======================================================================
void Print_Show() {
  DBGPSTRF(DASHED_LINES);
  // PrintTimeFromBoot();
  PrintINFO_ntp_server();
  // PrintUTCtime();
  // printTime(getCurrentTime());
  PrintLocalTime();
  // DBGPSTRF("human time     : %s \n", ctime(&rawtime));// ok
  // Print_GR_Date(getCurrentTime());
  DBGPSTRF(DASHED_LINES);
  yield();
} //== Close Print_Show ===


// ======================================================================
// time_is_set - ! optional  callback function to check if NTP server called
// ======================================================================
void time_is_set(bool from_sntp /* <= this parameter is optional */) {
  timeSync = true ;
  strftime(BufNTPLastSync, sizeof(BufNTPLastSync), "%F - %T (%z)", getCurrentTime());

  SNTPservername();
  DBGPSTRF("\r\n--- Τime synchronization with NTP server ---\n");

  PrintLocalTime();
  PrintINFO_ntp_server();
} //== Close time_is_set ===


// ======================================================================
// NTP_setup
// ======================================================================
void NTP_setup() {
  configTime(MYTZ, ntp1, ntp2);
  settimeofday_cb(time_is_set); // ! optional  callback function to check if NTP server called

  uint32_t TimeoutNTP = millis() + 10000UL;

  while (!timeSync && millis() < TimeoutNTP) yield();

  if (timeSync) {
    FlagIncorrectTime = false;
  } else {
    DBGPSTRF("\r\n---[ERROR]: NTP Server not sending responses \n\n");
    FlagIncorrectTime = true;
  }

} //== Close NTP_setup ====




/*********************************
  END Page
**********************************/
