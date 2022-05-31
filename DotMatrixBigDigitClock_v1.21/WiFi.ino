//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#define POWER_dBm       10.0	// max 20.5 dBm and should be multiples of 0.25.
#define TIMEOUT_WIFI    10		// in seconds, waiting for Wifi connection 
#define ACTIVE_TIME_AP  3       // in minutes, active time Soft Access Point 

WiFiEventHandler gotIpEventHandler, disconnectedEventHandler; // WiFiEvents
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


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


// ======================================================================
// WiFiEvents_setup
// more info -> https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/generic-class.html
// ======================================================================
void WiFiEvents_setup() {
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP & event) {
#if DEBUGLEVEL >= 1  //Serial Debugging
    DBGPSTRF("[WiFiEvent] -> Connected with SSID  : %s\n[WiFiEvent] -> ESP WiFi local IP    : % s\n",
             WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
#endif //DEBUGLEVEL >= 1
    digitalWrite(MY_LED, LED_BUILTIN_ON);
  });

  disconnectedEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected & event) {
#if DEBUGLEVEL > 4 //Serial Debugging
    DBGPSTRF("\r[WiFiEvent] -> The WiFi status is: %s\n",
             WiFiStatusToString(WiFi.status()));
#endif //DEBUGLEVEL > 4
    digitalWrite(MY_LED, LED_BUILTIN_OFF);
  });
} // == Close  WiFiEvents ===


/* ======================================================================
  restartESP
  ======================================================================= */
void restartESP() {
  FlagRestart = false ;
  digitalWrite (MY_LED, LED_BUILTIN_OFF);
  WiFi.disconnect();
  delay(200); // do not remove, need a delay for disconnect to change status()
  WiFi.mode(WIFI_OFF);
  printf("\r\n\n\t===>  R e s t a r t    E S P  <=== \n\n\r");

  // DotMatrix Display
  Set2ZonesInfo();

  DMcfg.speed_1     = 35;
  DMcfg.speed_2     = 35;
  DMcfg.pause_1     = 3000;
  DMcfg.pause_2     = 3000;
  DMcfg.align       = PA_CENTER;
  
  if (FlagSaveSettings) {
    DMcfg.INeffe_1    = PA_GROW_DOWN;
    DMcfg.INeffe_2    = PA_GROW_UP;
    DMcfg.OUTeffe_1   = PA_GROW_UP;
    DMcfg.OUTeffe_2   = PA_GROW_DOWN;

    sprintf(BufGEN,   "Save Data");
    sprintf(BufMsg_0, "to cfg file");
    DotMatrixDisplayInfo();
    delay (1000);
  }

  DMcfg.INeffe_1    = PA_OPENING_CURSOR;
  DMcfg.INeffe_2    = PA_OPENING_CURSOR;
  DMcfg.OUTeffe_1   = PA_OPENING_CURSOR;
  DMcfg.OUTeffe_2   = PA_OPENING_CURSOR;

  //                      Ε   π     α    ν    ε    κ    κ    ί    ν    η    σ    η
  strncpy_P(BufGEN,   "\x084\x0a7\x098\x0a4\x09c\x0a1\x0a1\x0e5\x0a4\x09e\x0a9\x09e",   sizeof(BufGEN));    // Επανεκκίνηση
  //                   Τ   η    ς     σ    υ    σ    κ    ε    υ    ή    ς
  strncpy_P(BufMsg_0, "T\x09e\x0aa \x0a9\x0ac\x0a9\x0a1\x09c\x0ac\x0e3\x0aa",           sizeof(BufMsg_0));  // Της συσκευής

  DotMatrixDisplayInfo();

  delay (1000);
  ESP.restart();
  delay(100);
}// == Close restartESP ===


// ======================================================================
// SoftAP_setup
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/soft-access-point-examples.html
// ======================================================================
void SoftAP_setup() {
  WiFi.mode(WIFI_AP);
  delay(100);

  // Begin SoftAP mode. Configuring Soft Access Point
  // You can remove the password parameter if you want the AP to be open.
  bool ReadySoftAP = WiFi.softAP(AP_SSID, AP_PASS);
  delay(100);

  if (ReadySoftAP) {
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
    DBGPSTRF("[info]: Configures the ESP as SoftAP is Ready \n");
    DBGPSTRF("Access Point: %s\n", AP_SSID);
    DBGPSTRF("AP Password : %s\n", AP_PASS);
    DBGPSTRF("Go to IP    : %s in a web browser \n", WiFi.softAPIP().toString().c_str());
#endif //DEBUGLEVEL >= 1

    Set2ZonesInfo();

    DMcfg.speed_1     = 30;
    DMcfg.speed_2     = 30;
    DMcfg.pause_1     = 4000;
    DMcfg.pause_2     = 4000;
    DMcfg.align       = PA_CENTER;
    DMcfg.INeffe_1    = PA_SCROLL_LEFT;
    DMcfg.INeffe_2    = PA_SCROLL_LEFT;
    DMcfg.OUTeffe_1   = PA_SCROLL_LEFT;
    DMcfg.OUTeffe_2   = PA_SCROLL_LEFT;

    strlcpy(BufGEN, "Connect using AP: ", sizeof(BufGEN));
    strcat(BufGEN, AP_SSID);

    sprintf(BufMsg_0, "Go to web browser IP: %d.%d.%d.%d",
            WiFi.softAPIP()[0], WiFi.softAPIP()[1], WiFi.softAPIP()[2], WiFi.softAPIP()[3]);

    DotMatrixDisplayInfo();

  } else {
    DBGPSTRF("[Error]: Failed! to Configures the ESP as SoftAP\n");
    DMcfg.INeffe_1    = PA_PRINT;
    strlcpy(BufGEN,   "Error AP", sizeof(BufGEN));
    strlcpy(BufMsg_0,  AP_SSID, sizeof(BufMsg_0));
    DotMatrixDisplayInfo();
    restartESP();
  }

  // server for Ap
  server.serveStatic("/", LittleFS, "/").setDefaultFile("config.html"); // ESP Configuration web page
  server.on("/Read_ESPsettings",    HTTP_GET,  SendData_APsettings);    // Read ESP settings
  server.on("/SaveConfig",          HTTP_POST, getDataParameters);      // Save ESP settings
  server.on("/LoadDefault_cfg",     HTTP_GET,  SendDefaultSettings);    // Load Default settings

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
#if DEBUGLEVEL > 1 // DEBUG SERIAL
    DBGPSTRF("\r\n---[info] ESP Reboot Command from SoftAP mode Web page \n\r");
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 1
    request-> send(200);
    restartESP();
  });

  // not found
  server.onNotFound([](AsyncWebServerRequest * request) {
    request->redirect("/404page.html"); //
  });

  server.begin();

  DMcfg.speed_1     = 50;
  DMcfg.speed_2     = 0;
  DMcfg.pause_1     = 2000;
  DMcfg.pause_2     = 0;
  DMcfg.align       = PA_LEFT;
  DMcfg.INeffe_1    = PA_PRINT;
  DMcfg.INeffe_2    = PA_NO_EFFECT;
  DMcfg.OUTeffe_1   = PA_PRINT;
  DMcfg.OUTeffe_2   = PA_NO_EFFECT;

  static uint8_t APactiveStations, OLDactiveStations = 8;

  uint32_t softAPtimeout = millis() + (ACTIVE_TIME_AP * 60000L);

  // AP loop
  while ((millis() < softAPtimeout) || (APactiveStations > 0)) {
    if (FlagRestart)
      restartESP();

    digitalWrite(MY_LED, millis() % (TIME_OFF + TIME_ON) >= TIME_ON);

    APactiveStations =  WiFi.softAPgetStationNum();

    if (OLDactiveStations != APactiveStations ) {
      OLDactiveStations = APactiveStations;
      sprintf(BufGEN, "%d.%d.%d.%d",
              WiFi.softAPIP()[0], WiFi.softAPIP()[1], WiFi.softAPIP()[2], WiFi.softAPIP()[3]);

      sprintf(BufMsg_0, "%s", APactiveStations ? "Connected" : "Disconnected");
      DotMatrixDisplayInfo();
    }

    yield();
  }// while

  WiFi.softAPdisconnect(true);

  restartESP();

}// == Close SoftAP_setup ==


// ======================================================================
// WiFi_setup
// ======================================================================
void WiFi_setup() {
  // WiFi.mode(WIFI_AP);        // ESP8266 works in Access Point
  // WiFi.mode(WIFI_AP_STA);    // ESP8266 works in both AP mode and station mode
  WiFi.mode(WIFI_STA);          // ESP8266 works in station mode
  delay(20);

#if DEBUGLEVEL >= 1  //Serial Debugging
  DBGPSTRF ("\r\n----------- WiFi setup ---------------------------------\n\r");
  DBGPSTRF("--- Try to Connecting  WiFi SSID    : %s\n", sta_ssid);
#endif //DEBUGLEVEL >= 1

  if (FlagDHCP == false ) {
    WiFi.config (staticIP, gatewayIP, netMaskIP, dns1, dns2);
#if DEBUGLEVEL >= 1  //Serial Debugging
    DBGPSTRF("--- Wait for WiFi connection with STATIC IP\n");
#endif //DEBUGLEVEL >= 1
  } else {
#if DEBUGLEVEL >= 1  //Serial Debugging
    DBGPSTRF("--- Wait for WiFi connection with DHCP IP\n");
#endif //DEBUGLEVEL >= 1
  }

  // Begin WiFi
  WiFi.begin((char*)sta_ssid, (char*)sta_pass);

  // Sets the max transmit power, in dBm
  WiFi.setOutputPower(POWER_dBm);
  delay(2);

  uint32_t TimeoutWiFi = millis() + (TIMEOUT_WIFI * 1000L);

  while (WiFi.status() != WL_CONNECTED && millis() < TimeoutWiFi) yield();

  if (WiFi.isConnected()) {
#if DEBUGLEVEL >= 1  //Serial Debugging    
    DBGPSTRF("--- [info]: Time waiting for Wifi connection    : %d sec. \n", millis() / 1000);
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL >= 1
  } else {
#if DEBUGLEVEL >= 1  //Serial Debugging    
    DBGPSTRF("--- [ERROR]: Cannot connect!\n");
    DBGPSTRF(" After waiting: %d millis --> The WiFi status is: %s\n", millis(),
             WiFiStatusToString(WiFi.status()));
    DBGPSTRF("--- [info]: Try to connect with AP : %s \n", AP_SSID);
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL >= 1
    digitalWrite (MY_LED, LED_BUILTIN_OFF);

    ClearDisplay();
    DMcfg.pause_2  = 1500;
    DMcfg.OUTeffe_2 = PA_SCROLL_LEFT;

    strlcpy(BufGEN,   "ERROR connect", sizeof(BufGEN));
    sprintf(BufMsg_0, "%s%s  %s  ", "The WiFi Status is: ",  WiFiStatusToString(WiFi.status()), " => Go into AP mode " );

    DotMatrixDisplayInfo();

    SoftAP_setup();
  }
} // == Close WiFi_setup ===





/*********************************
  END Page
**********************************/
