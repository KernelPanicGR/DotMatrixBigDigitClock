
// ======================================================================
// SendData_Info
// ======================================================================
void SendData_Info (AsyncWebServerRequest* request) {
  LittleFS.info(fs_info);
  static uint8_t macAddr[6] = {0};
  static char macBuffer[18] = {0};

  WiFi.macAddress(macAddr);
  snprintf_P(macBuffer, sizeof(macBuffer), PSTR("%02x:%02x:%02x:%02x:%02x:%02x"),
             macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);

  // Assistant for ArduinoJson  https://arduinojson.org/v6/assistant/
  StaticJsonDocument<768> doc;
  doc["JsonDST"]            = timeinfo->tm_isdst;
  doc["JsonNTP"]            = NTP_server;
  doc["JsonSSID"]           = WiFi.SSID();
  doc["JsonWiFiCh"]         = WiFi.channel();
  doc["JsonRSSI"]           = WiFi.RSSI();
  doc["JsonIP"]             = WiFi.localIP().toString();
  doc["JsonNetmask"]        = WiFi.subnetMask().toString();
  doc["JsonGateway"]        = WiFi.gatewayIP().toString();
  doc["JsonDNSip"]          = WiFi.dnsIP().toString();
  doc[LBL_HOSTNAME]         = Hostname;
  doc[LBL_HTTP_PORT]        = HTTP_PORT;
  doc["JsonProject"]        = MY_SKETCH; // Project Name;
  doc["JsonPrjDate"]        = __DATE__ " - " __TIME__ ;
  doc["JsonMacAdd"]         = macBuffer;
  doc["JsonSDKver"]         = ESP.getSdkVersion();
  doc["JsonCoreVersion"]    = ESP.getCoreVersion();
  doc["JsonMCUfrq"]         = ESP.getCpuFreqMHz();
  doc["JsonFlashSpeed"]     = ESP.getFlashChipSpeed() / 1000000;
  doc["JsonSketchSize"]     = formatBytes(ESP.getSketchSize());        // the size of the current sketch
  doc["JsonFrSketchSpace"]  = formatBytes(ESP.getFreeSketchSpace());   //  free sketch space
  doc["JsonFStotal"]        = formatBytes(fs_info.totalBytes);
  doc["JsonFSused"]         = formatBytes(fs_info.usedBytes);

  char json[768] = {0};
  serializeJson(doc, json);

#if DEBUGLEVEL > 3 //Serial Debugging
  DBGPSTRF("\r----- Sent info to Web page ---------------------------\n\r");
  serializeJsonPretty(doc, Serial);
  DBGPSTRF("\r \n");
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 3

  request->send_P(200, "application/json", json);
} //== Close SendData_Info ===


// ======================================================================
// SendEvent_Info
// ======================================================================
void SendEvent_Info() {
  FlagEventInfo = false;

  char FreeHeap[20]         = { 0 };
  char Fragmentation[20]    = { 0 };
  char BufSendTime[40]      = { 0 };

  strftime(BufSendTime, sizeof(BufSendTime),  "%F - %T (UTC %z)", timeinfo);
  sprintf(FreeHeap, "%s", formatBytes(ESP.getFreeHeap()));       // free heap size
  sprintf(Fragmentation, "%d %%", ESP.getHeapFragmentation());

  eventinfo.send(BufSendTime,    "JsonTimeNow",      millis());
  eventinfo.send(TimeFromBoot(), "JsonTimeFromBoot", millis());
  eventinfo.send(BufNTPLastSync, "JsonNTPLastSync",  millis());
  eventinfo.send(FreeHeap,       "JsonFreeHeap",     millis());
  eventinfo.send(Fragmentation,  "JsonHeapFrag",     millis());

#if DEBUGLEVEL > 4 //Serial Debugging
  DBGPSTRF("\r----- Sent DataInfo to Web page -----------------------\n\r");
  DBGPSTRF("JsonTimeNow      : %s \n", BufSendTime);
  DBGPSTRF("JsonTimeFromBoot : %s \n", TimeFromBoot());
  DBGPSTRF("JsonNTPLastSync  : %s \n", BufNTPLastSync);
  DBGPSTRF("JsonFreeHeap     : %s \n", FreeHeap);
  DBGPSTRF("JsonHeapFrag     : %s \n", Fragmentation);
#endif //Serial Debugging > 4
}//== Close SendEvent_Info ===


// ======================================================================
// SendData_ESPsettings
// ======================================================================
void SendData_ESPsettings (AsyncWebServerRequest* request) {
  // Assistant for ArduinoJson  https://arduinojson.org/v6/assistant/
  StaticJsonDocument<512> doc;

  doc[LBL_HOSTNAME]     = Hostname;
  doc["JS_SSID"]        = WiFi.SSID();
  doc[LBL_WIFI_SSID]    = sta_ssid;
  doc[LBL_WIFI_PASS]    = sta_pass;
  doc[LBL_DHCP_FLAG]    = FlagDHCP; //  DHCP checkbox

  if (FlagDHCP) {
    doc[LBL_MY_IP]      = WiFi.localIP().toString();
    doc[LBL_SUB_MASK]   = WiFi.subnetMask().toString();
    doc[LBL_GATEWAY]    = WiFi.gatewayIP().toString();
    doc[LBL_DNS_1]      = WiFi.dnsIP().toString();
    doc[LBL_DNS_2]      = dns2.toString();
  }
  else {
    doc[LBL_MY_IP]      = staticIP.toString();
    doc[LBL_SUB_MASK]   = netMaskIP.toString();
    doc[LBL_GATEWAY]    = gatewayIP.toString();
    doc[LBL_DNS_1]      = dns1.toString();
    doc[LBL_DNS_2]      = dns2.toString();
  }
  doc[LBL_MY_TZ]        = MYTZ;
  doc[LBL_NTP_SYNC]     = NTPsyncEvery;
  doc[LBL_NTP_1]        = ntp1;
  doc[LBL_NTP_2]        = ntp2;

  char json[512]        = {0};

  serializeJson(doc, json);

#if DEBUGLEVEL > 3 //Serial Debugging
  DBGPSTRF("\r--- [info] Sent ESP settings to Configuration Web page ---\n");
  serializeJsonPretty(doc, Serial);
  DBGPSTRF("\r \n");
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 3

  request->send_P( 200, "application/json", json);
} //== Close SendData_ESPsettings ===



// ======================================================================
// SendDefaultSettings
// ======================================================================
void SendDefaultSettings (AsyncWebServerRequest* request) {
  LoadDefaultSettings();

  StaticJsonDocument<512> doc;

  doc[LBL_HOSTNAME]     = Hostname;
  doc["JS_SSID"]        = WiFi.SSID();
  doc[LBL_WIFI_SSID]    = sta_ssid;
  doc[LBL_WIFI_PASS]    = sta_pass;
  doc[LBL_DHCP_FLAG]    = FlagDHCP; //  DHCP checkbox

  doc[LBL_MY_IP]        = staticIP.toString();
  doc[LBL_SUB_MASK]     = netMaskIP.toString();
  doc[LBL_GATEWAY]      = gatewayIP.toString();
  doc[LBL_DNS_1]        = dns1.toString();
  doc[LBL_DNS_2]        = dns2.toString();

  doc[LBL_MY_TZ]        = MYTZ;
  doc[LBL_NTP_SYNC]     = NTPsyncEvery;
  doc[LBL_NTP_1]        = ntp1;
  doc[LBL_NTP_2]        = ntp2;

  char json[512]        = {0};

  serializeJson(doc, json);

#if DEBUGLEVEL > 3 //Serial Debugging
  DBGPSTRF("\r--- [info] Sent Default ESP settings to Configuration Web page ---\n");
  serializeJsonPretty(doc, Serial);
  DBGPSTRF("\r \n");
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 3

  request->send_P( 200, "application/json", json);
} //== Close SendDefaultSettings ===


// ======================================================================
// SendData_APsettings
// ======================================================================
void SendData_APsettings (AsyncWebServerRequest* request) {
  StaticJsonDocument<512> doc;
  doc[LBL_HOSTNAME]     = Hostname;

  doc["JS_SSID"]        = AP_SSID;
  doc[LBL_WIFI_SSID]    = sta_ssid;
  doc[LBL_WIFI_PASS]    = sta_pass;
  doc[LBL_DHCP_FLAG]    = FlagDHCP; //  DHCP checkbox

  doc[LBL_MY_IP]        = staticIP.toString();
  doc[LBL_SUB_MASK]     = netMaskIP.toString();
  doc[LBL_GATEWAY]      = gatewayIP.toString();
  doc[LBL_DNS_1]        = dns1.toString();
  doc[LBL_DNS_2]        = dns2.toString();

  doc[LBL_MY_TZ]        = MYTZ;
  doc[LBL_NTP_SYNC]     = NTPsyncEvery;
  doc[LBL_NTP_1]        = ntp1;
  doc[LBL_NTP_2]        = ntp2;

  char json[512]        = {0};

  serializeJson(doc, json);

#if DEBUGLEVEL > 3 //Serial Debugging
  DBGPSTRF("\r--- [info] Sent ESP settings to Soft Access Point page ---\n");
  serializeJsonPretty(doc, Serial);
  DBGPSTRF("\r \n");
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 3

  request->send_P( 200, "application/json", json);
} //== Close SendData_APsettings ===


// ======================================================================
// SendWebSocketCurrentTime
// ======================================================================
void SendWebSocketCurrentTime() {
  static uint8_t OLDsec;

  if (Sec != OLDsec) {
    OLDsec = Sec;

    strftime(BufTime, sizeof(BufTime), "%T", timeinfo);

    StaticJsonDocument<128> doc;
    doc["JsonTimeNow"] = BufTime;

    if (DayOfWeek != OLDwday) {
      OLDwday = DayOfWeek;

      JsonArray JsonDate = doc.createNestedArray("JsonDate");
      JsonDate.add(timeinfo->tm_mday);
      JsonDate.add(timeinfo->tm_mon + 1);
      JsonDate.add(timeinfo->tm_year + 1900);
    }

    char json[128];
    serializeJson(doc, json);

#if DEBUGLEVEL > 4 //Serial Debugging
    DBGPSTRF("\r Sent Time to Web page: \n");
    serializeJsonPretty(doc, Serial);
    DBGPSTRF("\r \n");
#endif //DEBUGLEVEL > 4
    yield();

    wsTime.textAll(json);
  }
} //== Close SendWebSocketCurrentTime ===


// ======================================================================
// SendBrightness
// ======================================================================
void SendBrightness() {
  StaticJsonDocument<32> doc;
  doc[LBL_BRIGHTNESS] = Brightness;
  char json[64];
  serializeJson(doc, json);

#if DEBUGLEVEL > 3 //Serial Debugging 
  DBGPSTRF("\rSending WS to Brightness Web page  => \n");
  serializeJsonPretty(doc, Serial);
  printf("\r \n");
#endif //DEBUGLEVEL > 3

  wsBright.textAll(json);
}//== Close SendBrightness ===


// ======================================================================
// getDataParameters
// ======================================================================
void getDataParameters (AsyncWebServerRequest* request) {
  /*
    // --- for trial Receive ----
    int params = request->params();
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isFile()) { //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if (p->isPost()) {
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    //List all parameters (Compatibility)
    int args = request->args();
    for (int i = 0; i < args; i++) {
      Serial.printf("ARG [%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
    } // --- end trial Receive ----
    DBGPSTRF(DASHED_LINES);
  */

  strlcpy (Hostname, (request->getParam(LBL_HOSTNAME,  true)->value().c_str()), sizeof(Hostname));
  strlcpy (sta_ssid, (request->getParam(LBL_WIFI_SSID, true)->value().c_str()), sizeof(sta_ssid));
  strlcpy (sta_pass, (request->getParam(LBL_WIFI_PASS, true)->value().c_str()), sizeof(sta_pass));

  FlagDHCP = request->hasParam(LBL_DHCP_FLAG, true);

  if (FlagDHCP == false) {
    staticIP.fromString(request->getParam(LBL_MY_IP,  true)->value().c_str());
    netMaskIP.fromString(request->getParam(LBL_SUB_MASK, true)->value().c_str());
    gatewayIP.fromString(request->getParam(LBL_GATEWAY, true)->value().c_str());
    dns1.fromString(request->getParam(LBL_DNS_1, true)->value().c_str());
    dns2.fromString(request->getParam(LBL_DNS_2, true)->value().c_str());
  }

  if (FlagDHCP && WiFi.isConnected()) {
    staticIP    = WiFi.localIP();
    netMaskIP   = WiFi.subnetMask();
    gatewayIP   = WiFi.gatewayIP();
    dns1        = WiFi.dnsIP();
    dns2        = DNS2_IP;
  }

  strlcpy (MYTZ,(request->getParam(LBL_MY_TZ,    true)->value().c_str()), sizeof(MYTZ));
  NTPsyncEvery = request->getParam(LBL_NTP_SYNC, true)->value().toInt();
  strlcpy (ntp1,(request->getParam(LBL_NTP_1,    true)->value().c_str()), sizeof(ntp1));
  strlcpy (ntp2,(request->getParam(LBL_NTP_2,    true)->value().c_str()), sizeof(ntp2));

#if DEBUGLEVEL > 2 //Serial Debugging
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_WIFI_SSID, sta_ssid);
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_WIFI_PASS, sta_pass);
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_DHCP_FLAG, FlagDHCP ? "true" : "false");
  DBGPSTRF("\rReceive from HTTP -> %s: %d\n", LBL_HTTP_PORT, HTTP_PORT);
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_HOSTNAME,  Hostname);

  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_MY_IP,    staticIP.toString().c_str());
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_SUB_MASK, netMaskIP.toString().c_str());
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_GATEWAY,  gatewayIP.toString().c_str());
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_DNS_1,    dns1.toString().c_str());
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_DNS_2,    dns2.toString().c_str());

  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_MY_TZ,    MYTZ);
  DBGPSTRF("\rReceive from HTTP -> %s: %d\n", LBL_NTP_SYNC, NTPsyncEvery);
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_NTP_1,    ntp1);
  DBGPSTRF("\rReceive from HTTP -> %s: %s\n", LBL_NTP_2,    ntp2);
  DBGPSTRF(DASHED_LINES);
#endif //Serial Debugging > 2

  if (SaveESPsettings()) {
    FlagSaveSettings = true;
    request->redirect("/success.html"); //
  } else {
    request->redirect("/error.html"); //
  }
}//== Close SaveData ==







/*********************************
  END Page
**********************************/
