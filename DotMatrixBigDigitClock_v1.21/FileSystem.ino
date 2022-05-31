const char* FSconfigFile	= "/FSconfigESP.json";
const char* FSbrightness    = "/FSbrightness.json";


// ======================================================================
// LoadDefaultSettings - Using defaul configuration values
// ======================================================================
void LoadDefaultSettings() {
  strlcpy(sta_ssid, MY_SSID, sizeof(sta_ssid));
  strlcpy(sta_pass, MY_PASS, sizeof(sta_ssid));
  FlagDHCP      = USE_DHCP; // DHCP or Static IP
  staticIP      = STATIC_IP;
  gatewayIP     = GATEWAY_IP;
  netMaskIP     = NETMASK_IP;
  dns1          = DNS1_IP;
  dns2          = DNS2_IP;
  NTPsyncEvery  = 91;      // synchronization with NTP server, in minutes.
  strlcpy(MYTZ, GR_TZ, sizeof(MYTZ));
  strlcpy(ntp1, NTP_SERVER_1, sizeof(ntp1));
  strlcpy(ntp2, NTP_SERVER_2, sizeof(ntp2));
  strlcpy(Hostname, DNS_NAME, sizeof (Hostname));

#if DEBUGLEVEL > 2 //DEBUG SERIAL
  DBGPSTRF("\r---------- Load Default ESP Settings --------------\n\r");
  DBGPSTRF("\r[Default] %s: %s \n", LBL_WIFI_SSID, sta_ssid);
  DBGPSTRF("\r[Default] %s: %s \n", LBL_WIFI_PASS, sta_pass);

  DBGPSTRF("\r[Default] %s", FlagDHCP ? "Connecting with DHCP\n" : "Connecting with STATIC IP \n");
  DBGPSTRF("\r[Default] %s: %s \n", LBL_MY_IP, staticIP.toString().c_str());
  DBGPSTRF("\r[Default] %s: %s \n", LBL_SUB_MASK, netMaskIP.toString().c_str());
  DBGPSTRF("\r[Default] %s: %s \n", LBL_GATEWAY, gatewayIP.toString().c_str());
  DBGPSTRF("\r[Default] %s: %s \n", LBL_DNS_1, dns1.toString().c_str());
  DBGPSTRF("\r[Default] %s: %s \n", LBL_DNS_2, dns2.toString().c_str());

  DBGPSTRF("\r[Default] %s: %s \n", LBL_MY_TZ, MYTZ);
  DBGPSTRF("\r[Default] %s: %d \n", LBL_NTP_SYNC, NTPsyncEvery);
  DBGPSTRF("\r[Default] %s: %s \n", LBL_NTP_1, ntp1);
  DBGPSTRF("\r[Default] %s: %s \n", LBL_NTP_2, ntp2);
  DBGPSTRF("\r[Default] %s: %d \n", LBL_HTTP_PORT, HTTP_PORT);
  DBGPSTRF("\r[Default] %s: %s \n", LBL_HOSTNAME, Hostname);
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 2 
} //== Close LoadDefaultSettings ===


// ======================================================================
// LoadConfigFile - Loads the Config from a file
// ======================================================================
bool LoadESPsettings() {
  static const char* BufCNFG;

  // this opens the config file in read-mode
  File configFile = LittleFS.open(FSconfigFile, "r");
  if (!configFile)  {
    DBGPSTRF("\r---[ERROR]: Failed to open file for Reading: %s  ---\n", FSconfigFile);
    return false;
  }
  else {
    // Assistant for ArduinoJson  https://arduinojson.org/v6/assistant/
    StaticJsonDocument<780> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      DBGPSTRF("\r---[ERROR]: Deserialization() failed: %s  ---\n", error.f_str());
      return false;
    }
#if DEBUGLEVEL > 2 //Serial Debugging
    DBGPSTRF("\r--- Load Data ESP settings from File: %s  ---\n", FSconfigFile);
    serializeJsonPretty(doc, Serial);
    DBGPSTRF("\r \n");
#endif //DEBUGLEVEL > 2

    strlcpy(sta_ssid, doc[LBL_WIFI_SSID] | MY_SSID, sizeof(sta_ssid));
    strlcpy(sta_pass, doc[LBL_WIFI_PASS] | MY_PASS, sizeof(sta_pass));

    FlagDHCP = doc[LBL_DHCP_FLAG].as<bool>();

    BufCNFG = doc[LBL_MY_IP] | staticIP.toString().c_str();
    staticIP.fromString(BufCNFG);

    BufCNFG = doc[LBL_SUB_MASK] | netMaskIP.toString().c_str();
    netMaskIP.fromString(BufCNFG);

    BufCNFG = doc[LBL_GATEWAY] | gatewayIP.toString().c_str();
    gatewayIP.fromString(BufCNFG);

    BufCNFG = doc[LBL_DNS_1] | dns1.toString().c_str();
    dns1.fromString(BufCNFG);

    BufCNFG = doc[LBL_DNS_2] | dns2.toString().c_str();
    dns2.fromString(BufCNFG);

    strlcpy(Hostname, doc[LBL_HOSTNAME] | DNS_NAME, sizeof(Hostname));

    NTPsyncEvery = doc[LBL_NTP_SYNC] | 180;
    strlcpy(MYTZ, doc[LBL_MY_TZ] | GR_TZ, sizeof(MYTZ));
    strlcpy(ntp1, doc[LBL_NTP_1] | NTP_SERVER_1, sizeof(ntp1));
    strlcpy(ntp2, doc[LBL_NTP_2] | NTP_SERVER_2, sizeof(ntp2));
  }
  configFile.close();

#if DEBUGLEVEL > 2 //DEBUG SERIAL
  DBGPSTRF("\r--- The final content to file: %s  ---\n", FSconfigFile);
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_WIFI_SSID, sta_ssid);
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_WIFI_PASS, sta_pass);

  DBGPSTRF("\r[cfg file] %s", FlagDHCP ? "WiFi connection with DHCP IP \n" : "WiFi connection with STATIC IP \n");

  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_MY_IP, staticIP.toString().c_str());
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_SUB_MASK, netMaskIP.toString().c_str());
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_GATEWAY, gatewayIP.toString().c_str());
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_DNS_1, dns1.toString().c_str());
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_DNS_2, dns2.toString().c_str());

  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_MY_TZ, MYTZ);
  DBGPSTRF("\r[cfg file] %s: %d \n", LBL_NTP_SYNC, NTPsyncEvery);
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_NTP_1, ntp1);
  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_NTP_2, ntp2);

  DBGPSTRF("\r[cfg file] %s: %s \n", LBL_HOSTNAME, Hostname);
  DBGPSTRF("\r[cfg file] %s: %d \n", LBL_HTTP_PORT, HTTP_PORT);
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 2

  return true;
} //== Close LoadESPsettings ===


// ======================================================================
// SaveESPsettings -
// ======================================================================
bool SaveESPsettings() {
  StaticJsonDocument<780> doc;
  doc[LBL_WIFI_SSID]    = sta_ssid;
  doc[LBL_WIFI_PASS]    = sta_pass;
  doc[LBL_DHCP_FLAG]    = FlagDHCP; //
  doc[LBL_MY_IP]        = staticIP.toString();
  doc[LBL_SUB_MASK]     = netMaskIP.toString();
  doc[LBL_GATEWAY]      = gatewayIP.toString();
  doc[LBL_DNS_1]        = dns1.toString();
  doc[LBL_DNS_2]        = dns2.toString();
  doc[LBL_MY_TZ]        = MYTZ;
  doc[LBL_NTP_SYNC]     = NTPsyncEvery;
  doc[LBL_NTP_1]        = ntp1;
  doc[LBL_NTP_2]        = ntp2;
  doc[LBL_HOSTNAME]     = Hostname;

  // Open FSconfigESP.json file for writing
#if DEBUGLEVEL > 2  //DEBUG SERIAL
  DBGPSTRF("\r--- Open for writing : [ %s ] file ---\n", FSconfigFile);
#endif //DEBUGLEVEL > 2

  File file = LittleFS.open(FSconfigFile, "w");
  if (!file) {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL
    DBGPSTRF("\r\n---[ERROR]: Failed to open: [ %s ] File for Writing ---\n", FSconfigFile);
#endif //DEBUGLEVEL >= 1
    return false;
  }

  // Write data to file and close it
  if (serializeJsonPretty(doc, file) == 0) {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL
    DBGPSTRF("\r\n---[ERROR]: Failed write to FS: %s --\n", FSconfigFile);
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL >= 1
    return false;
  }
#if DEBUGLEVEL > 2 // DEBUG SERIAL
  DBGPSTRF("\r--- Saving the file: %s with data -> \n", FSconfigFile);
  serializeJsonPretty(doc, Serial);
  DBGPSTRF("\r \n");
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 2

  file.close();  // Close the file
  return true;
} //== Close SaveESPsettings ===


// ======================================================================
// Load_Brightness -
// ======================================================================
bool Load_Brightness() {
  File BRFile = LittleFS.open(FSbrightness, "r");
  if (!BRFile)  {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL
    DBGPSTRF("\r---[ERROR]: Failed to open file: %s \n", FSbrightness);
#endif //DEBUGLEVEL >= 1
    return false;
  } else {
    StaticJsonDocument<64> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, BRFile);
    if (error) {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL
      DBGPSTRF("\r---[ERROR]: Deserialization() failed: %s \n", error.f_str());
#endif //DEBUGLEVEL >= 1
      return false;
    }
#if DEBUGLEVEL > 2  // DEBUG SERIAL 
    DBGPSTRF("\r--- Load Data from File: %s ---\n", FSbrightness);
    serializeJsonPretty(doc, Serial);
    DBGPSTRF("\r \n");
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 2

    Brightness = doc[LBL_BRIGHTNESS] | BRIGHT_VAL;
    if (Brightness > MAX_BRIGHT)    // Failed Brightness  Value
      Brightness = BRIGHT_VAL;      // Using defaul Brightness values
  }
  BRFile.close();

#if DEBUGLEVEL > 2 // DEBUG SERIAL
  DBGPSTRF("\r--- The content Brightness file is: \n[cfg Br file] %s: %d \n", LBL_BRIGHTNESS, Brightness);
  DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 2

  return true;
} //== Close  Load_Brightness ===


// =================================================================
// Save_FS_Brightness
// =================================================================
bool Save_FS_Brightness() {
  StaticJsonDocument<64> doc;
  doc[LBL_BRIGHTNESS] = Brightness;

  File BRFile = LittleFS.open(FSbrightness, "w");
  if (!BRFile) {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL
    DBGPSTRF("\r---[ERROR] Failed to open: %s File for writing ---\n", FSbrightness);
#endif //DEBUGLEVEL >= 1
    return false ;
  }

  // Serialize JSON to file
  if (serializeJsonPretty(doc, BRFile) == 0) {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL  
    DBGPSTRF("\r---[ERROR] Failed to write to file: %s  ---\n", LBL_BRIGHTNESS);
#endif //DEBUGLEVEL >= 1
    return false ;
  }

#if DEBUGLEVEL > 2 // DEBUG SERIAL 
  DBGPSTRF("\r---[info] Saving in FS the file: %s ---\n", FSbrightness);
  serializeJsonPretty(doc, Serial);
  DBGPSTRF("\r \n");
#endif //DEBUGLEVEL > 2 

  BRFile.close();  // Close the file
  return true;
}


// ======================================================================
// listDir -
// ======================================================================
void listDir(const char * dirname) {
  DBGPSTRF("\rListing FileSystem directory: %s\n", dirname);

  Dir root = LittleFS.openDir(dirname);

  while (root.next()) {
    File file = root.openFile("r");
    String fileName = root.fileName();
    size_t fileSize = file.size();
    DBGPSTRF("\rFS File: %s, size: %s \n", fileName.c_str(), formatBytes(fileSize).c_str());
    file.close();
  }
} //== Close listDir ===


// ======================================================================
// FileSystem_setup | File System init
// ======================================================================
void FileSystem_setup() {
  fileSystemConfig.setAutoFormat(false);
  fileSystem->setConfig(fileSystemConfig);

  fsOK = LittleFS.begin();
  DBGPSTRF("Mounting the File system: %s\n", fsOK ? "OK " : "[ERROR] Failed to mount file system!");

  if (!LoadESPsettings()) {
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
    DBGPSTRF("\r\n---[ERROR]: Failed to Read Configuration File, use Default values \n");
#endif //DEBUGLEVEL >= 1
    LoadDefaultSettings();
    SaveESPsettings();
  }

  if (!Load_Brightness()) {
    Brightness = BRIGHT_VAL; // Using defaul Brightness values
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
    DBGPSTRF("\r\n---[ERROR]: Failed to Read Brightness, use Default values \n");
    DBGPSTRF("\r[Default] %s: %d\n", LBL_BRIGHTNESS, Brightness);
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL >= 1
    Save_FS_Brightness();
  }

#if DEBUGLEVEL > 4 // DEBUG SERIAL 
  listDir("/"); // Origin
#endif //DEBUGLEVEL > 4
  yield();
} //== Close FileSystem_setup ===


// ======================================================================
// formatBytes -
// ======================================================================
const String formatBytes(size_t const& bytes) {
  return bytes < 1024 ? static_cast<String>(bytes) +
         " Byte" : bytes < 1048576 ? static_cast<String>(bytes / 1024.0) +
         " KB" : static_cast<String>(bytes / 1048576.0) + " MB";
} //== Close formatBytes ===




/*********************************
  END Page
**********************************/
