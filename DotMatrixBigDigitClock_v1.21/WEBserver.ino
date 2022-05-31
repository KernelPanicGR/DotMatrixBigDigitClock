
// ======================================================================
// WebServer_setup
// ======================================================================
void WebServer_setup() {
  // Start the mDNS responder
  if (!MDNS.begin(Hostname)) {
    DBGPSTRF("\r\n---[ERROR]: setting up MDNS responder! \n\n");
  } else {
#if DEBUGLEVEL > 1 // DEBUG SERIAL
    DBGPSTRF("\r---[info]: mDNS responder started --\n");
#endif //DEBUGLEVEL > 1 
  }

  server.addHandler(new SPIFFSEditor(HTTP_USERNAME, HTTP_PASS, LittleFS));

  // send index.htm
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

  server.on("/ReadMy_info",         HTTP_GET,   SendData_Info);
  server.on("/Read_ESPsettings",    HTTP_GET,   SendData_ESPsettings);
  server.on("/SaveConfig",          HTTP_POST,  getDataParameters);
  server.on("/LoadDefault_cfg",     HTTP_GET,   SendDefaultSettings);
   server.on("/edit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, PSTR("text/plain"), " # Only for Developer(s) #");
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
#if DEBUGLEVEL > 1 // DEBUG SERIAL
    DBGPSTRF("\r\n---[info] ESP Reboot Command from Web page \n\r");
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 1

    request-> send(200);
    FlagRestart = true ;
  });

  // Firmware Update
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    bool FlagUPDT = !Update.hasError();
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", FlagUPDT ? "OK" : "FAIL");
    response->addHeader("Connection", "close");
    request->send(response);
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

    if (!index) {
#if DEBUGLEVEL > 1 // DEBUG SERIAL
      DBGPSTRF("[info]: Update Start: %s\n", filename.c_str());
#endif //DEBUGLEVEL > 1
      Update.runAsync(true);
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
        DBGPSTRF("\r\n---[ERROR] Update.begin ERROR: \n");
        Update.printError(Serial);
        DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 1  
      }
    }

    if (!Update.hasError()) {
      if (Update.write(data, len) != len) {
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
        DBGPSTRF("\r\n---[ERROR] Update.write ERROR: ");
        Update.printError(Serial);
        DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 1 
      }
    }
    if (final) {
      if (Update.end(true)) {
#if DEBUGLEVEL > 1 // DEBUG SERIAL
        DBGPSTRF("---[info]: Update Success: %u B \n", index + len);
#endif //DEBUGLEVEL > 1 
      } else {
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
        DBGPSTRF("\r\n---[ERROR] Update end ERROR: ");
        Update.printError(Serial);
        DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 1  
      }
    }
  });

  // not found
  server.onNotFound([](AsyncWebServerRequest * request) {
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
    DBGPSTRF("\n---[Error] Not found, method: ");
    if (request->method() == HTTP_GET )
      DBGPSTRF("GET");
    else if (request->method() == HTTP_POST )
      DBGPSTRF("POST");
    else if (request->method() == HTTP_DELETE)
      DBGPSTRF("DELETE");
    else if (request->method() == HTTP_PUT)
      DBGPSTRF("PUT");
    else if (request->method() == HTTP_PATCH)
      DBGPSTRF("PATCH");
    else if (request->method() == HTTP_HEAD)
      DBGPSTRF("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      DBGPSTRF("OPTIONS");
    else
      DBGPSTRF("UNKNOWN");

    DBGPSTRF(" From url: http://%s - File Not found: %s \n", request->host().c_str(), request->url().c_str());
#endif //DEBUGLEVEL >= 1

    request->redirect("/404page.html"); //
  });

  server.begin();

  MDNS.addService("http", "tcp", HTTP_PORT);   // Add service to MDNS-SD
#if DEBUGLEVEL > 2 // DEBUG SERIAL
  DBGPSTRF("\r---[info]: -- ENABLE MDNS --\n");

  if ( HTTP_PORT != 80 )
    DBGPSTRF("\rOpen a browser to ->  http://%s.local:%d/\n",  Hostname, HTTP_PORT);
  else
    DBGPSTRF("\rOpen a browser to ->  http://%s.local\n",  Hostname);
#endif //DEBUGLEVEL > 2 


#if DEBUGLEVEL > 2 // DEBUG SERIAL
  DBGPSTRF("\r---[info]: Async Web server is Ready --\n");
  DBGPSTRF("\rFor edit page  ->  username : %s\n", HTTP_USERNAME);
  DBGPSTRF("\rFor edit page  ->  password : %s\n", HTTP_PASS);
#endif //DEBUGLEVEL > 2
} //== Close WebServer_setup ====





/*********************************
  END Page
**********************************/
