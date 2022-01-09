
const char* FSconfigFile	= "/config.json";
const char* FSbrightness    = "/FSbrightness.json";


// ======================================================================
// FileSystem_setup
// ======================================================================
void FileSystem_setup() {
  // Mount the filesystem
  bool result = FILE_FS.begin();
  printf(result ? ("\r\n--- OK Files System opened ---\n") : ("\r\n--- ERROR --- Failed to mount file system ---\n"));
  if (result) {
    if (!LoadConfigFile()) {
#ifdef DEBUG2_SERIAL
      printf("\r\n--- ERROR --- Failed to Read Configuration File, using default values \n");
#endif // DEBUG2_SERIAL
      defaultConfigs();
    }
    if (!Load_Brightness()) {
      Brightness = 7;
#ifdef DEBUG2_SERIAL
      printf("\r\n--- ERROR --- Failed to Read Brightness, using default values \n");
      Serial.printf_P(PSTR("\rLoad Default Value -> %s: %d\n"), BRITGHTNS_JS, Brightness);
      PrintShortLines();
#endif // DEBUG2_SERIAL
    }
  } else {
    printf("\r--- Formatting [ %s ] filesystem ---\n", FS_NAME);
    FILE_FS.format();
  }

#ifdef DEBUG_SERIAL
  listDir("/"); // Origin
   delay(200);
#endif // DEBUG_SERIAL  
} //== Close FileSystem_setup ===


// ======================================================================
// defaultConfigs - Using defaul values
// ======================================================================
void defaultConfigs() {
  Serial.printf("\r---------- Start with Default Configuration Value --------------\n\r");
  FlagDHCP = ESPconfig.DHCP ;  // false = Static IP  or  true = DHCP

#ifdef ENABLE_MDNS // use mDNS 
  strlcpy(Hostname, DNS_NAME, sizeof (Hostname));
#endif // ENABLE_MDNS

#ifdef DEBUG2_SERIAL
  if (FlagDHCP == false ) {
    Serial.printf_P(PSTR("\r->  WiFi connection with STATIC IP \n"));
    Serial.printf_P(PSTR("\r->  %s: %s \n"), MY_IP, ESPconfig.stationIP.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), SUB_MASK_JS, ESPconfig.netMaskIP.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), GATEWAY_JS, ESPconfig.gatewayIP.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), DNS_1_JS, ESPconfig.dns1.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), DNS_2_JS, ESPconfig.dns2.toString().c_str());
  }
  else {
    Serial.printf_P(PSTR("\r->  WiFi connection with DHCP IP \n"));
  }
  Serial.printf_P(PSTR("\r->  %s: %s \n"), NTP_0_JS, ESPconfig.ntp1);
  Serial.printf_P(PSTR("\r->  %s: %s \n"), NTP_1_JS, ESPconfig.ntp2);
  Serial.printf_P(PSTR("\r->  %s: %d \n"), HTTP_PORT_JS, HTTP_PORT);

#ifdef ENABLE_MDNS // use mDNS 
  Serial.printf_P(PSTR("->  %s: %s\n"), HOSTNAME_JS, Hostname);
#endif // ENABLE_MDNS
  PrintShortLines();
#endif // DEBUG2_SERIAL
} //== Close defaultConfigs ===



// ======================================================================
// LoadConfigFile - Loads the Config from a file
// ======================================================================
bool LoadConfigFile() {
  // this opens the config file in read-mode
  File configFile = FILE_FS.open(FSconfigFile, "r");
  if (!configFile)  {
    printf ("\r--- ERROR --- Failed to open file for Reading: %s  ---\n", FSconfigFile);
    return false;
  } else {
    // Assistant for ArduinoJson  https://arduinojson.org/v6/assistant/
    StaticJsonDocument<800> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      printf("\r--- ERROR --- Deserialization() failed: %s  ---\n", error.f_str());
      return false;
    }
    Serial.printf_P(PSTR("\r--- Load Data from File: %s  ---\n"), FSconfigFile);
    serializeJsonPretty(doc, Serial);
    printf("\r \n");
    FlagDHCP = doc[DHCP_FLAG_JS].as<bool>();

#ifdef ENABLE_MDNS // use mDNS 
    strlcpy(Hostname, doc[HOSTNAME_JS] | DNS_NAME, sizeof(Hostname));
#endif // ENABLE_MDNS

    if (FlagDHCP == false ) {
      const char* ip = doc[MY_IP] | ESPconfig.stationIP.toString().c_str();
      ESPconfig.stationIP.fromString(ip);

      const char* sbm = doc[SUB_MASK_JS] | ESPconfig.netMaskIP.toString().c_str();
      ESPconfig.netMaskIP.fromString(sbm);

      const char* gw = doc[GATEWAY_JS] | ESPconfig.gatewayIP.toString().c_str();
      ESPconfig.gatewayIP.fromString(gw);

      const char* dns1 = doc[DNS_1_JS] | ESPconfig.dns1.toString().c_str();
      ESPconfig.dns1.fromString(dns1);

      const char* dns2 = doc[DNS_2_JS] | ESPconfig.dns2.toString().c_str();
      ESPconfig.dns2.fromString(dns2);
    }

    strlcpy(ESPconfig.ntp1, doc[NTP_0_JS] | NTP_SERVER_1, sizeof(ESPconfig.ntp1));
    strlcpy(ESPconfig.ntp2, doc[NTP_1_JS] | NTP_SERVER_2, sizeof(ESPconfig.ntp2));
  }
  configFile.close();

#ifdef DEBUG2_SERIAL
  Serial.printf_P(PSTR("\r--- The final content is ---\n"));
  if (FlagDHCP == false ) {
    Serial.printf_P(PSTR("\r->  WiFi connection with STATIC IP \n"));
    Serial.printf_P(PSTR("\r->  %s: %s \n"), MY_IP, ESPconfig.stationIP.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), SUB_MASK_JS, ESPconfig.netMaskIP.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), GATEWAY_JS, ESPconfig.gatewayIP.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), DNS_1_JS, ESPconfig.dns1.toString().c_str());
    Serial.printf_P(PSTR("\r->  %s: %s \n"), DNS_2_JS, ESPconfig.dns2.toString().c_str());
  }
  else {
    Serial.printf_P(PSTR("\r->  WiFi connection with DHCP IP \n"));
  }
  Serial.printf_P(PSTR("\r->  %s: %s \n"), NTP_0_JS, ESPconfig.ntp1);
  Serial.printf_P(PSTR("\r->  %s: %s \n"), NTP_1_JS, ESPconfig.ntp2);
  Serial.printf_P(PSTR("\r->  %s: %d \n"), HTTP_PORT_JS, HTTP_PORT);

#ifdef ENABLE_MDNS // use mDNS 
  Serial.printf_P(PSTR("\r->  %s: %s \n"), HOSTNAME_JS, Hostname);
#endif // ENABLE_MDNS
  PrintShortLines();
#endif // DEBUG2_SERIAL

  return true;
} //== Close LoadConfigFile ===


// ======================================================================
// SaveFSconfig -
// ======================================================================
bool SaveFSconfig() {
  StaticJsonDocument<800> doc;
  doc[DHCP_FLAG_JS]   = FlagDHCP; //  DHCP checkbox
#ifdef ENABLE_MDNS // use mDNS 
  doc[HOSTNAME_JS]    = Hostname;
#endif // ENABLE_MDNS
  if (FlagDHCP == false) {
    doc[MY_IP]          = ESPconfig.stationIP.toString();
    doc[SUB_MASK_JS]    = ESPconfig.netMaskIP.toString();
    doc[GATEWAY_JS]     = ESPconfig.gatewayIP.toString();
    doc[DNS_1_JS]       = ESPconfig.dns1.toString();
    doc[DNS_2_JS]       = ESPconfig.dns2.toString();
  }
  doc[NTP_0_JS]         = ESPconfig.ntp1;
  doc[NTP_1_JS]         = ESPconfig.ntp2;


  // Open FSconfigFile.json file for writing
  Serial.printf_P(PSTR("\r--- Open for writing : [ %s ] file ---\n"), FSconfigFile);
  File file = LittleFS.open(FSconfigFile, "w");
  if (!file) {
    Serial.printf_P(PSTR("\r\n--- ERROR --- Failed to open: [ %s ] File for Writing ---\n"), FSconfigFile);
    return false;
  }

  // Write data to file and close it
  if (serializeJsonPretty(doc, file) == 0) {
    Serial.printf_P(PSTR("\r\n--- ERROR --- Failed write to FS: %s --\n"), FSconfigFile);
    PrintShortLines();
    return false;
  }

  //#ifdef DEBUG2_SERIAL
  Serial.printf_P(PSTR("\r--- Saving file on FS file: %s ---\n"), FSconfigFile);
  serializeJsonPretty(doc, Serial);
  printf("\r \n");
  PrintShortLines();
  //#endif // DEBUG2_SERIAL
  file.close();  // Close the file
  return true;
} //== Close SaveFSconfig ===



// ======================================================================
// Load_Brightness -
// ======================================================================
bool Load_Brightness() {
  File configFile = FILE_FS.open(FSbrightness, "r");
  if (!configFile)  {
    printf ("\r--- ERROR --- Failed to open file: %s \n", FSconfigFile);
    return false;
  } else {
    StaticJsonDocument<200> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      Serial.printf_P(PSTR("\r--- ERROR --- Deserialization() failed: %s \n"), error.f_str());
      return false;
    }
    Serial.printf_P(PSTR("\r--- Load Data from File: %s ---\n"), FSbrightness);
    serializeJsonPretty(doc, Serial);
    printf("\r \n");
    Brightness = doc[BRITGHTNS_JS] | 7;
    if (Brightness > maxBrightness) // Brightness Failed Value
      Brightness = maxBrightness;
  }
  configFile.close();
#ifdef DEBUG2_SERIAL
  Serial.printf_P(PSTR("\r--- The final content is --- \n-> %s: %d \n"), BRITGHTNS_JS, Brightness);
  PrintShortLines();
#endif // DEBUG2_SERIAL
  return true;
} //== Close  Load_Brightness ===


// =================================================================
// Save_FS_Brightness
// =================================================================
bool Save_FS_Brightness() {
  StaticJsonDocument<64> doc;
  doc[BRITGHTNS_JS] = Brightness;

  File file = LittleFS.open(FSbrightness, "w");
  if (!file) {
    Serial.printf_P(PSTR("\r--- ERROR --- Failed to open: %s File for writing ---\n"), FSbrightness);
    return false ;
  }

  // Serialize JSON to file
  if (serializeJsonPretty(doc, file) == 0) {
    Serial.printf_P(PSTR("\r--- ERROR --- Failed to write to file: %s  ---\n"), BRITGHTNS_JS);
    return false ;
  }

#ifdef DEBUG2_SERIAL
  Serial.printf_P(PSTR("\r--- Saving file on FS: %s ---\n"), FSbrightness);
  serializeJsonPretty(doc, Serial);
#endif // DEBUG2_SERIAL 

  file.close();  // Close the file
  return true;
}


// ======================================================================
// listDir -
// ======================================================================
void listDir(const char * dirname) {
#ifdef DEBUG_SERIAL
  printf ("Listing FileSystem directory: %s\n", dirname);

  Dir root = FILE_FS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    String fileName = root.fileName();
    size_t fileSize = file.size();
    Serial.printf ("FS File: %s, size: %s ", fileName.c_str(), formatBytes(fileSize).c_str());
    time_t cr = file.getCreationTime();
    time_t lw = file.getLastWrite();
    file.close();
    struct tm * tmstruct = localtime(&cr);
    printf ("    CREATION: %d/%02d/%02d - %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
    tmstruct = localtime(&lw);
    printf ("         LAST WRITE: %d/%02d/%02d - %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
  }
  //  PrintShortLines();
  //  Serial.println();
#endif // DEBUG_SERIAL  
} //== Close listDir ===



// ======================================================================
// formatBytes -
// ======================================================================

const String formatBytes(size_t const& bytes) {
#ifdef DEBUG_SERIAL
  return bytes < 1024 ? static_cast<String>(bytes) + " Bytes" : bytes < 1048576 ? static_cast<String>(bytes / 1024.0) + " KB" : static_cast<String>(bytes / 1048576.0) + " MB";
#endif // DEBUG_SERIAL  
}

//== Close formatBytes ===



/*********************************
  END Page
**********************************/
