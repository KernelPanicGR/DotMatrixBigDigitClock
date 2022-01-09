
// ======================================================================
// My_Info
// ======================================================================
void My_Info (AsyncWebServerRequest* request) {
  static uint8_t mac[6];
  char macStr[18] = { 0 };
  char SdkVer[20];
  // char CoreVer[20];
  char CpuFreq[20];
  char FlashSpeed[20];
  char Channel[4];
  char rssi[10];

  sprintf(SdkVer, "%s", ESP.getSdkVersion());
  sprintf(CpuFreq, "%d MHz", ESP.getCpuFreqMHz());
  sprintf(FlashSpeed, "%d MHz", (ESP.getFlashChipSpeed() / 1000000));
  sprintf(Channel, "%d", WiFi.channel());
  sprintf(rssi, "%d dBm", WiFi.RSSI());

  WiFi.macAddress(mac);
  snprintf_P(macStr, sizeof(macStr), PSTR("%02X:%02X:%02X:%02X:%02X:%02X"),
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Assistant for ArduinoJson  https://arduinojson.org/v6/assistant/
  StaticJsonDocument<800> doc;
  doc["JsonNTP"]            = NTP_server;
  doc["JsonProject"]        = ProjectName;
  doc["JsonPrjDate"]        = __DATE__ " - " __TIME__ ;
  doc["JsonMacAdd"]         = macStr;
  doc["JsonSDKver"]         = SdkVer;
  doc["JsonCoreVersion"]    = String (ESP.getCoreVersion());
  // doc["JsonCoreVersion"]    = CoreVer;
  doc["JsonMCUfrq"]         = CpuFreq;
  doc["JsonFlashSpeed"]     = FlashSpeed;
  doc["JsonSSID"]           = WiFi.SSID();
  doc["JsonWiFiCh"]         = Channel;
  doc["JsonRSSI"]           = rssi;
  doc["JsonIP"]             = WiFi.localIP().toString();
  doc["JsonNetmask"]        = WiFi.subnetMask().toString();
  doc["JsonGateway"]        = WiFi.gatewayIP().toString();
  doc["JsonDNSip"]          = WiFi.dnsIP().toString();
  doc[HTTP_PORT_JS]         = HTTP_PORT;

  char json[800] = {0};
  serializeJson(doc, json);
  request->send_P(200, "application/json", json);
} //== Close My_Info ===


// ======================================================================
// SendDataInfo
// ======================================================================
void SendDataInfo() {
  char FreeHeap[20];
  char Fragmentation[20];

  sprintf(FreeHeap, "%d KB", (ESP.getFreeHeap() / 1024));
  sprintf(Fragmentation, "%d %%", ESP.getHeapFragmentation());
  strftime(BufTime, sizeof(BufTime), "%d/%m/%Y - %T", getCurrentTime()); //

  TimeFromBoot();

  eventinfo.send(BufTime, "JsonTimeNow", millis());
  eventinfo.send(Time_From_Boot, "JsonTimeFromBoot", millis());
  eventinfo.send(BufNTPLastSync, "JsonNTPLastSync", millis());
  eventinfo.send(FreeHeap, "JsonFreeHeap", millis());
  eventinfo.send(Fragmentation, "JsonHeapFrag", millis());
  FlagEventInfo = false ;
}


// ======================================================================
// My_Cofig
// ======================================================================
void My_Cofig (AsyncWebServerRequest* request) {
  // Assistant for ArduinoJson  https://arduinojson.org/v6/assistant/
  StaticJsonDocument<512> doc;
  doc["JS_SSID"]        = WiFi.SSID();
  doc[DHCP_FLAG_JS]   = FlagDHCP; //  DHCP checkbox
#ifdef ENABLE_MDNS // use mDNS 
  doc[HOSTNAME_JS]    = Hostname;
#else
  doc[HOSTNAME_JS]    = "Without mDNS" ;
#endif // ENABLE_MDNS

  if (FlagDHCP == false) {
    doc[MY_IP]          = ESPconfig.stationIP.toString();
    doc[SUB_MASK_JS]    = ESPconfig.netMaskIP.toString();
    doc[GATEWAY_JS]     = ESPconfig.gatewayIP.toString();
    doc[DNS_1_JS]       = ESPconfig.dns1.toString();
    doc[DNS_2_JS]       = ESPconfig.dns2.toString();
  } else {
    doc[MY_IP]          = WiFi.localIP().toString();
    doc[SUB_MASK_JS]    = WiFi.subnetMask().toString();
    doc[GATEWAY_JS]     = WiFi.gatewayIP().toString();
    doc[DNS_1_JS]       = WiFi.dnsIP().toString();
    doc[DNS_2_JS]       = ESPconfig.dns2.toString();
  }

  doc[NTP_0_JS]         = ESPconfig.ntp1;
  doc[NTP_1_JS]         = ESPconfig.ntp2;
  char json[512]   = {0};
  serializeJson(doc, json);
  request->send_P( 200, "application/json", json);
} //== Close My_Cofig ===


// ======================================================================
// SendWebSocketCurrentTime
// ======================================================================
void SendWebSocketCurrentTime() {
  static uint8_t OLDsec;
  if (Sec != OLDsec) {
    OLDsec = Sec;
    
    strftime(BufTime, sizeof(BufTime), "%T", getCurrentTime());
    StaticJsonDocument<64> doc;
    doc["JsonTimeNow"] = BufTime;
    char json[64];
    serializeJson(doc, json);
    //  printf("\rSending Web Socket Current Time: \n");
    //  serializeJsonPretty(doc, Serial);
    //  printf("\r \n");
    yield();

    wsTime.textAll(json);
  }
} //== Close SendWebSocketCurrentTime ===


// ======================================================================
// SendBrightness
// ======================================================================
void SendBrightness() {
  StaticJsonDocument<64> doc;
  doc[BRITGHTNS_JS] = Brightness;
  char json[64];
  serializeJson(doc, json);

#ifdef DEBUG2_SERIAL
  printf("\rSending Brightness to WS: \n");
  serializeJsonPretty(doc, Serial);
  printf("\r \n");
#endif // DEBUG2_SERIAL

  wsBright.textAll(json);
}//== Close SendBrightness ===


// ======================================================================
// SendSensorsData
// ======================================================================
void SendSensorsData() {
  char  Humidity[20];
  char  Pressure[20];
  char  Temp[20];

  if (FlagSensors) {
    dtostrf(bme.readTemperature(), 3, 1, Temp);
    dtostrf(bme.readHumidity(), 3, 1, Humidity);
    dtostrf(bme.readPressure() / 100.0F, 4, 0, Pressure);
  }
  else { // If you don't have a BME280 put only the library and test with random valum
    dtostrf(random(1000, 4900) / 100., 3, 1, Temp);
    dtostrf(random(3000, 9200) / 100., 3, 1, Humidity);
    dtostrf(random(300, 1045), 4, 0, Pressure);
  }

  // events.send("ping", NULL, millis());
  eventWeather.send(Temp,  "Temperature", millis());
  eventWeather.send(Humidity, "Humidity", millis());
  eventWeather.send(Pressure, "Pressure", millis());
  FlagEventWather = false ;

#ifdef DEBUG_SERIAL
  printf ("\r--------------- Sensors Value -------------------------\n\r");
  Serial.printf_P(PSTR("Temperature : %s\n"), Temp );
  Serial.printf_P(PSTR("Humidity    : %s\n"), Humidity );
  Serial.printf_P(PSTR("Pressure    : %s\n"), Pressure );
#endif // DEBUG_SERIAL  
}


// ======================================================================
// SaveConfigData
// ======================================================================
void SaveConfigData (AsyncWebServerRequest* request) {
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
    PrintshortLines();
  */

#ifdef ENABLE_MDNS // use mDNS 
  strlcpy (Hostname, (request->getParam(HOSTNAME_JS, true)->value().c_str()), sizeof(Hostname));
#endif // ENABLE_MDNS

  if (request->hasParam(DHCP_FLAG_JS, true)) {
    FlagDHCP = true ;
  } else {
    FlagDHCP = false ;
  }

  if (FlagDHCP == false) {
    ESPconfig.stationIP.fromString(request->getParam(MY_IP, true)->value().c_str());
    ESPconfig.netMaskIP.fromString(request->getParam(SUB_MASK_JS, true)->value().c_str());
    ESPconfig.gatewayIP.fromString(request->getParam(GATEWAY_JS, true)->value().c_str());
    ESPconfig.dns1.fromString(request->getParam(DNS_1_JS, true)->value().c_str());
    ESPconfig.dns2.fromString(request->getParam(DNS_2_JS, true)->value().c_str());
  }

  strlcpy (ESPconfig.ntp1, (request->getParam(NTP_0_JS, true)->value().c_str()), sizeof(ESPconfig.ntp1));
  strlcpy (ESPconfig.ntp2, (request->getParam(NTP_1_JS, true)->value().c_str()), sizeof(ESPconfig.ntp2));

#ifdef DEBUG_SERIAL
  Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), DHCP_FLAG_JS, FlagDHCP ? "true" : "false");
  Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %d\n"), HTTP_PORT_JS, HTTP_PORT);
#ifdef ENABLE_MDNS // use mDNS 
  Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), HOSTNAME_JS, Hostname);
#endif // ENABLE_MDNS
  if (FlagDHCP == false) {
    Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), MY_IP, ESPconfig.stationIP.toString().c_str());
    Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), SUB_MASK_JS, ESPconfig.netMaskIP.toString().c_str());
    Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), GATEWAY_JS, ESPconfig.gatewayIP.toString().c_str());
    Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), DNS_1_JS, ESPconfig.dns1.toString().c_str());
    Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), DNS_2_JS, ESPconfig.dns2.toString().c_str());
  }
  Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), NTP_0_JS, ESPconfig.ntp1);
  Serial.printf_P(PSTR("\rReceive from HTTP -> %s: %s\n"), NTP_1_JS, ESPconfig.ntp2);
  PrintShortLines();
#endif // DEBUG_SERIAL

  if (SaveFSconfig()) {
    request->redirect("/success.html"); //
  } else {
    request->redirect("/error.html"); //
  }
}//== Close SaveData ==







/*********************************
  END Page
**********************************/
