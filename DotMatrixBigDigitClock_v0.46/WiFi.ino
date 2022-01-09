#define POWER_dBm       10.0	// max 20.5 dBm and should be multiples of 0.25.
#define SSID_AP         "ESP8266"
#define PASS_AP         "0123456789"
#define WAIT_CONNECT    20		// in seconds
#define DELAY_RESTART   30      // in seconds


/////////////////////////////////////////////////////////////////////////

// ======================================================================
// WiFi_setup
// ======================================================================
void WiFi_setup() {
  // WiFi.mode(WIFI_AP);        // ESP8266 works in Access Point
  // WiFi.mode(WIFI_AP_STA);    // ESP8266 works in both AP mode and station mode
  WiFi.mode(WIFI_STA);          // ESP8266 works in station mode
  yield();
  printf ("\r\n--------------- WiFi ----------------------------------\n\r");
  if (FlagDHCP == false ) {
    WiFi.config (ESPconfig.stationIP, ESPconfig.gatewayIP, ESPconfig.netMaskIP, ESPconfig.dns1, ESPconfig.dns2);
    Serial.printf_P(PSTR("-- Wait for WiFi connection with STATIC IP --\n"));
  } else {
    Serial.printf_P(PSTR("-- Wait for WiFi connection with DHCP IP --\n"));
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setOutputPower(POWER_dBm); //
  delay(10);

  while (WiFi.status() != WL_CONNECTED && millis() < (WAIT_CONNECT * 1000L)) yield();

  if (WiFi.isConnected()) {
    Serial.printf_P(PSTR("Time waiting for Wifi connection    : %d sec. \n"), millis() / 1000);
  } else {
    Serial.printf_P(PSTR("After waiting: %d millis --> The WiFi status is: %s\n"), millis(), WiFiStatusToString(WiFi.status()));
    Serial.printf_P(PSTR("-- Try again to connect after: %d sec. --\n"), DELAY_RESTART);
    digitalWrite (LED_BUILTIN, LED_OFF);

    ClearDisplay();
    DMcfg.pause_2  = 1500;
    DMcfg.OUTeffe_2 = PA_SCROLL_LEFT;

    strlcpy(BufGEN,   "WiFi Status", sizeof(BufGEN));
    sprintf(BufMsg_0, "%s  %s %d  %s  ", WiFiStatusToString(WiFi.status()), " ** Try again to connect after: ", DELAY_RESTART, " sec.  ** " );

    DotMatrixDisplayInfo();

    WiFi.disconnect();
    delay (100);
    WiFi.mode(WIFI_OFF);

    uint8_t RestartTO = DELAY_RESTART;

    ClearDisplay();

    DMcfg.speed_1     = 0;
    DMcfg.speed_2     = 25;
    DMcfg.pause_1     = 990;
    DMcfg.pause_2     = 990;
    DMcfg.align       = PA_CENTER;
    DMcfg.INeffe_1    = PA_PRINT;
    DMcfg.INeffe_2    = PA_SCROLL_UP;
    DMcfg.OUTeffe_1   = PA_NO_EFFECT;
    DMcfg.OUTeffe_2   = PA_SCROLL_UP;

    strlcpy(BufGEN,   "Restart to:", sizeof(BufGEN));
    sprintf(BufMsg_0, "%d", RestartTO);

    DotMatrixDisplayInfo();
    yield();

    while ( RestartTO > 1) {
      RestartTO -- ;
      delay(10);
      sprintf(BufMsg_0, "%d", RestartTO);
      DotMatrixDisplayInfo();
    }

    ClearDisplay();
    strlcpy(BufGEN,   "Rebooting..", sizeof(BufGEN));
    strlcpy(BufMsg_0, "   Now ! ",   sizeof(BufMsg_0));
    DotMatrixDisplayInfo();

    delay (1000);
    ESP.restart();
    delay (100);
  }
} // == Close WiFi_setup ===



// ======================================================================
// WiFiEvents
// ======================================================================
void WiFiEvents() {
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP & event) {
    Serial.printf_P(PSTR("[WiFiEvent] -> Connected with SSID  : %s\n[WiFiEvent] -> ESP WiFi local IP    : % s\n"),
                    WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    digitalWrite (LED_BUILTIN, LED_ON);
  });

  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected & event) {
    Serial.printf_P(PSTR("\r[WiFiEvent] -> The WiFi status is: %s\n"), WiFiStatusToString( WiFi.status()));
    digitalWrite (LED_BUILTIN, LED_OFF);
  });
} // == Close  WiFiEvents ===



// ======================================================================
// WiFiStatusToString
// ======================================================================
const char *WiFiStatusToString(wl_status_t code) {
  switch (code) {
    case WL_IDLE_STATUS:     return "IDLE Status";          break; // 0,   WiFi is in process of changing between statuses
    case WL_NO_SSID_AVAIL:   return "No ssid available";    break; // 1,   case configured SSID cannot be reached
    case WL_SCAN_COMPLETED:  return "Scan is completed";    break; // 2,   the scan networks is completed
    case WL_CONNECTED:       return "Connected";            break; // 3,   connected to a WiFi network
    case WL_CONNECT_FAILED:  return "CONNECT FAILED";       break; // 4,   password is incorrect
    case WL_CONNECTION_LOST: return "Connection is lost";   break; // 5,   the connection is lost
    case WL_DISCONNECTED:    return "Disconnected";         break; // 6,   disconnected from a network
    case WL_NO_SHIELD:       return "NO SHIELD";            break; // 255, for compatibility with WiFi Shield library
    default:                 return " ???";                 break; //
  }
} // == Close WiFiStatusToString ===




/*********************************
  END Page
**********************************/
