bool FlagUPDT ;
// ======================================================================
// WebServer_setup
// ======================================================================
void WebServer_setup() {
#ifdef ENABLE_MDNS // use mDNS 
  startMDNS();      // Start the mDNS responder
#else
  printf("\r-- Without mDNS --\n");
#endif // ENABLE_MDNS
  server.addHandler(new SPIFFSEditor(http_username, http_password, FILE_FS));

  // send index.html
  server.serveStatic("/", FILE_FS, "/").setDefaultFile("index.html");

  // server.on("/ReadTime",        HTTP_GET,  Read_Time);
  server.on("/ReadMy_info",     HTTP_GET,  My_Info);
  server.on("/ReadMy_Config",   HTTP_GET,  My_Cofig);
  server.on("/SaveConfig",      HTTP_POST, SaveConfigData);
  server.on("/edit", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain",  " # Only for Developer(s) #");
  });

  
  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request) {
    printf ("\r\n\r");
    PrintShortLines();
    printf ("\r-- Command of Rebooting MCU from http Web page --\n\r");
    FlagRestart = true ;
    request-> send(200, "text/plain", "MCU Rebooting now...");
  });


  server.on("/syncntp", HTTP_GET, [](AsyncWebServerRequest * request) {
    // time_is_set_scheduled();
    printf ("\r\n\r");
    PrintShortLines();
    printf ("\r-- Command for new Sync Time From NTP --\n\r");
    NTP_setup();
    request->send(200, "text/plain",  "Έγινε συγχρονισμός με τον NTP server");
  });

  

  // Firmware Update
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest * request) {
    FlagUPDT = !Update.hasError();
    //  AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", FlagUPDT ? "OK" : "FAIL");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", FlagUPDT ? "success" : "error");
    response->addHeader("Connection", "close");
    request->send(response);
  }, [](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      digitalWrite (LED_BUILTIN, LED_OFF);
      Serial.printf("Update Start: %s\n", filename.c_str());
      Update.runAsync(true);
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
        printf("\r\n--- Update ERROR --- Update.begin --- \n");
        // ("/error.html");
        Update.printError(Serial);
      }
    }
    if (!Update.hasError()) {
      if (Update.write(data, len) != len) {
        printf("\r\n--- Update ERROR --- Update.write --- \n");
        // ("/error.html");
        Update.printError(Serial);

      }
    }
    if (final) {
      if (Update.end(true)) {
        digitalWrite (LED_BUILTIN, LED_ON);
        //    ("/success.html");
        Serial.printf("Update Success: %uB\n", index + len);
      } else {
        printf("\r\n--- Update ERROR --- Update.end --- \n");
        Update.printError(Serial);
        //  request->redirect("/error.html");
      }
    }
  });

  // not found
  server.onNotFound([](AsyncWebServerRequest * request) {
    printf ("\n-- NOT_FOUND: ");
    if (request->method() == HTTP_GET )
      printf ("GET");
    else if (request->method() == HTTP_POST )
      printf ("POST");
    else if (request->method() == HTTP_DELETE)
      printf ("DELETE");
    else if (request->method() == HTTP_PUT)
      printf ("PUT");
    else if (request->method() == HTTP_PATCH)
      printf ("PATCH");
    else if (request->method() == HTTP_HEAD)
      printf ("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      printf ("OPTIONS");
    else
      printf ("UNKNOWN");
    printf (" http://%s%s --\n", request->host().c_str(), request->url().c_str());
    request->redirect("/404page.html"); //
  });

  server.begin();

#ifdef ENABLE_MDNS // use mDNS
  MDNS.addService("http", "tcp", HTTP_PORT);   // Add service to MDNS-SD
  printf ("\r-- ENABLE MDNS --\n");
#endif // ENABLE_MDNS
  printf ("\r-- Async Web server is Ready --\n");
} //== Close WebServer_setup ====


// ======================================================================
// startMDNS
// ======================================================================
void startMDNS() {
#ifdef ENABLE_MDNS // use mDNS  
  if (!MDNS.begin(Hostname)) {
    printf("\r\n--- ERROR ---Failed to starting MDNS name responder\n\n");

  } else {
    Serial.printf_P(PSTR("\r-- mDNS is Ready --\n"));
    Serial.printf_P(PSTR("\rFor Open page  ->  http://%s.local:%d/\n"),  Hostname, HTTP_PORT );
    Serial.printf_P(PSTR("\rFor edit page  ->  username : %s\n"), http_username);
    Serial.printf_P(PSTR("\rFor edit page  ->  password : %s\n"), http_password);
  }
#endif // ENABLE_MDNS
}//== Close startMDNS ===



/*********************************
  END Page
**********************************/
