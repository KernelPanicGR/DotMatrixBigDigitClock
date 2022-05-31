// dot matrix
#define     HARDWARE_TYPE   MD_MAX72XX::ICSTATION_HW
#define     BRIGHT_VAL      7
#define     MAX_BRIGHT      15
#define     CS_PIN          15  // D8 or any pin 
#define     MAX_ZONES       4
#define     MAX_DEVICES     16
#define     SPEED_SCROLL    28  // in milliseconds 
#define     SPEED_TIME      75  // in milliseconds
#define     ZONE_LOWER      0
#define     ZONE_one        1
#define     ZONE_UPPER      2
#define     ZONE_SecUP      3

bool        Flag_Pacman     = true,
            FlagEffe,
            Flag2ZonesB,
            Set_4_Zones,
            Set_2_Zones;

uint8_t     Brightness;
uint8_t     degC[]          = { 6, 3, 3, 56, 68, 68, 68 };  // Deg C
uint16_t    PAUSE_TIME      = 0 ;

char        Temperature[10] = {0};  // Temperature
char        Buf_TimeL[6]    = {0};  // hh:mm LOWER
char        Buf_TimeUp[6]   = {0};  // hh:mm UPPER
char        Buf_Sec_UP[3]   = {0};  // :ss
char        Buf_time[13]    = {0};  // 2 zone Time buffer Zone Upper
char        BufMsg_0[200]   = {0};  // 2 zone message buffer Zone Lower
char        BufMsg_1[20]    = {0};  // 2 zone message buffer Zone Upper
char        BufGEN[100]     = {0};  // 2 zone message, buffer ZONE UPPER


// Sprite Definitions
const uint8_t           F_PMAN1 = 6, W_PMAN1 = 8 ;
const uint8_t PROGMEM   pacman1[F_PMAN1 * W_PMAN1] =    // gobbling pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};

const uint8_t           F_PMAN2 = 6, W_PMAN2 = 18;
const uint8_t PROGMEM   pacman2[F_PMAN2 * W_PMAN2] =    // ghost pursued by a pacman
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
};


// Dot Matrix parameters configuration block
struct cfgParameter_t
{
  uint8_t           speed_1;        // animation frame delay
  uint16_t          pause_1;        // animation pause in milliseconds
  uint8_t           speed_2;        // animation frame delay
  uint16_t          pause_2;        // animation pause in milliseconds
  textPosition_t    align;     // text alignment at pause
  textEffect_t      INeffe_1;    // text animation effect
  textEffect_t      OUTeffe_1;   // text animation effect
  textEffect_t      INeffe_2;    // text animation effect
  textEffect_t      OUTeffe_2;   // text animation effect
  bool invert       = false;      // inverted display
} DMcfg;

// SPI hardware interface
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// ======================================================================
// Display_Booting
// ======================================================================
void Display_Booting() {
  Set2ZonesInfo();

  P.setIntensity(Brightness);
  DMcfg.speed_1     = 50;
  DMcfg.speed_2     = 30;
  DMcfg.pause_1     = 500;
  DMcfg.pause_2     = 500;
  DMcfg.align       = PA_CENTER;
  DMcfg.INeffe_1    = PA_SCROLL_DOWN;
  DMcfg.INeffe_2    = PA_SCROLL_LEFT;
  DMcfg.OUTeffe_1   = PA_NO_EFFECT;
  DMcfg.OUTeffe_2   = PA_NO_EFFECT;
  DMcfg.invert      = false;
  
  strcpy_P(BufGEN,   "Booting Now !");
  strcpy_P(BufMsg_0, "Wait for WiFi");

  DotMatrixDisplayInfo();
} //=== close Display_Booting  ===

// ======================================================================
// DotMatrixDisplayInfo
// ======================================================================
void DotMatrixDisplayInfo() {
  P.displayZoneText(ZONE_one,   BufGEN,   DMcfg.align, DMcfg.speed_1, DMcfg.pause_1, DMcfg.INeffe_1, DMcfg.OUTeffe_1);
  P.displayZoneText(ZONE_LOWER, BufMsg_0, DMcfg.align, DMcfg.speed_2, DMcfg.pause_2, DMcfg.INeffe_2, DMcfg.OUTeffe_2);
  while (!P.getZoneStatus(ZONE_LOWER)) {
    yield();
    P.displayAnimate();
  }
  yield();
}

// ======================================================================
// ClearDisplay
// ======================================================================
void ClearDisplay() {
  P.displayClear();
  P.displayReset();
  strcpy_P(BufGEN,   "");
  strcpy_P(BufMsg_0, "");
  yield();
}

// ======================================================================
// DisplayBigTime - Big Digit Code for reading clock time
// ======================================================================
void DisplayBigTime(char *psz, bool f = true) {
  sprintf(Buf_Sec_UP, "%02d", Sec);
  if (FlagIncorrectTime) {
    sprintf(psz, "%02d%c%02d", Hour, (':'), Minute);
  } else {
    sprintf(psz, "%02d%c%02d", Hour, (f ? ':' : ' '), Minute);
  }
} //=== close DisplayBigTime ===

// ======================================================================
// DisplayTime - Code for reading clock time
// ======================================================================
void DisplayTime(char *psz, bool f = true) {
  if (FlagIncorrectTime) {
    sprintf (psz, "%02d%c%02d%c%02d", Hour, (':'), Minute, (':'), Sec);
  } else {
    sprintf (psz, "%02d%c%02d%c%02d", Hour, (f ? ':' : ' '), Minute, (f ? ':' : ' '), Sec);
  }
} //=== close DisplayTime ===



// ======================================================================
// Day2str - Display name of Day
// ======================================================================
char *Day2str(uint8_t code, char *psz, uint8_t len) {
  static const char str[][10] PROGMEM =
  {
    { 137, 172, 168, 160, 152, 161, 227 },          // Κυριακή      "Sunday"
    { 131, 156, 172, 171, 226, 168, 152 },          // Δευτέρα      "Monday"
    { 146, 168, 229, 171, 158 },                    // Τρίτη        "Tuesday"
    { 146, 156, 171, 225, 168, 171, 158 },          // Τετάρτη      "Wednesday"
    { 143, 226, 163, 167, 171, 158 },               // Πέμπτη       "Thursday"
    { 143, 152, 168, 152, 169, 161, 156, 172, 227 },// Παρασκευή    "Friday"
    { 145, 225, 153, 153, 152, 171, 166 },          // Σάββατο      "Saturday"
  };

  *psz = '\0';
  if (code < 7)  {
    strncpy_P(psz, str[code], len);
    psz[len] = '\0';
  }
  return (psz);
}//=== close Day2str ======

// ======================================================================
// createHString -
// ======================================================================
void createHString(char *pH, char *pL) {
  for (; *pL != '\0'; pL++)
    *pH++ = *pL | 0x80; // offset character
  *pH = '\0';           // terminate the string
} //=== close createHString =====

// ======================================================================
// Setup4Zones -
// ======================================================================
void Setup4Zones() {
  Set_4_Zones = true ;
  Set_2_Zones = false ;
  P.setZone(ZONE_LOWER, 2, 7);
  P.setZone(ZONE_one, 0, 1);
  P.setZone(ZONE_UPPER, 10, 15);
  P.setZone(ZONE_SecUP, 8, 9);
  P.setFont(ZONE_LOWER, Double_Num);
  P.setFont(ZONE_UPPER, Double_Num);
  P.setFont(ZONE_one, LowerNum3x5);
  P.setFont(ZONE_SecUP, Dig6x8);

  P.setCharSpacing(P.getCharSpacing() * 2); // double height --> double spacing
  P.displayZoneText(ZONE_LOWER, Buf_TimeL,  PA_LEFT,    SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(ZONE_UPPER, Buf_TimeUp, PA_LEFT,    SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(ZONE_one, Temperature,  PA_CENTER,  SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(ZONE_SecUP, Buf_Sec_UP, PA_CENTER,  SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT);
  P.setCharSpacing( ZONE_one, 1); // spacing is built into the font definition
}//=== close Setup4Zones =====

// ======================================================================
// Setup2Zones -
// ======================================================================
void Setup2Zones() {
  Set_4_Zones = false ;
  Set_2_Zones = true ;
  P.setCharSpacing(1);  // spacing is built into the font definition
  P.setZone(ZONE_LOWER, 0,  MAX_DEVICES - 9);
  P.setZone(ZONE_one,       MAX_DEVICES - 8, MAX_DEVICES - 1);
  P.setFont(ZONE_LOWER, Fonts_5x7_gr);
  P.setFont(ZONE_one, Digit_6x7);   //
  P.displayClear(ZONE_LOWER);

  P.displayZoneText(ZONE_one, Buf_time, PA_CENTER, SPEED_TIME, PAUSE_TIME, PA_PRINT, PA_NO_EFFECT); // zone 1 time

  if (Flag_Pacman) {
    P.displayZoneText(ZONE_LOWER, "Kernel Panic", PA_CENTER, 50, 3000, PA_SPRITE, PA_SPRITE); // Kernel Panic
  }

  else if (Flag2ZonesB)   { // zone 0 date, temperature, humidity
    P.displayZoneText(ZONE_LOWER, BufMsg_0, PA_CENTER, SPEED_SCROLL, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  }

  else { // humidity, temperature
    P.displayZoneText(ZONE_LOWER, BufMsg_1, PA_CENTER, 40, 2000, PA_SCROLL_UP, PA_NO_EFFECT);
    FlagEffe = true ;
  }
}//=== close Setup2Zones =====


// ======================================================================
// Set2ZonesInfo -
// ======================================================================
void Set2ZonesInfo() {
  P.setCharSpacing(1);        // spacing is built into the font definition
  P.setZone(ZONE_LOWER, 0,  MAX_DEVICES - 9);
  P.setZone(ZONE_one,       MAX_DEVICES - 8, MAX_DEVICES - 1);
  P.setFont(Fonts_5x7_gr); //
  ClearDisplay();
}//=== close


// ======================================================================
// DisplayMyIntro -
// ======================================================================
void DisplayMyIntro() {
  ClearDisplay();

  DMcfg.speed_1     = 44;
  DMcfg.speed_2     = 50;
  DMcfg.pause_1     = 1400;
  DMcfg.pause_2     = 700;
  DMcfg.align       = PA_CENTER;
  DMcfg.INeffe_1    = PA_SPRITE;
  DMcfg.INeffe_2    = PA_SPRITE;
  DMcfg.OUTeffe_1   = PA_SPRITE;
  DMcfg.OUTeffe_2   = PA_SPRITE;

  strncpy_P(BufGEN,   "Kernel", sizeof(BufGEN));
  strncpy_P(BufMsg_0, "Panic", sizeof(BufMsg_0));

  DotMatrixDisplayInfo();
}//=== close DisplayMyIntro ===

// ======================================================================
// DisplayConnectInfo -
// ======================================================================
void DisplayConnectInfo() {
  ClearDisplay();
  //--------------------------->  =    Σ    υ    ν    δ    έ    θ    η    κ    ε     μ    ε
  static char txtWifiConnected[]  = "\x091\x0ac\x0a4\x09b\x0e2\x09f\x09e\x0a1\x09c \x0a3\x09c : "; // Συδέθηκε με

  DMcfg.speed_1     = 30;
  DMcfg.speed_2     = 30;
  DMcfg.pause_1     = 3000;
  DMcfg.pause_2     = 3000;
  DMcfg.align       = PA_CENTER;
  DMcfg.INeffe_1    = PA_SCROLL_LEFT;
  DMcfg.INeffe_2    = PA_SCROLL_LEFT;
  DMcfg.OUTeffe_1   = PA_SCROLL_LEFT;
  DMcfg.OUTeffe_2   = PA_SCROLL_LEFT;

  sprintf(BufGEN, "%s" "%s", txtWifiConnected, WiFi.SSID().c_str());
  sprintf(BufMsg_0, "%d.%d.%d.%d",
          WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);

  DotMatrixDisplayInfo();

  if (FlagIncorrectTime ) // if NTP service provides incorrect time
    IncorrectTime();

  ClearDisplay();
}//=== close DisplayConnectInfo ===


// ======================================================================
// IncorrectTime
// if NTP service provides incorrect time
// ======================================================================
void IncorrectTime() {
  ClearDisplay();
  P.setIntensity(14);
  strncpy_P (BufGEN,   "***      Be Careful    ***   ", sizeof(BufGEN));
  strncpy_P (BufMsg_0, "<<<< NTP service provides incorrect time >>>>  ", sizeof(BufMsg_0));
  DotMatrixDisplayInfo();
  ClearDisplay();
}


// ======================================================================
// DotMatrixDisplay
// ======================================================================
void DotMatrixDisplay() {
  static uint8_t    oldBrightness, PreviousSec;
  static bool       Dots, FlagSec;                   // seconds passing Dots
  static bool       Flag_Small_Clock    = true; // Start with 2 lines display
  const static char GRtxtTEMP[]     = " \x007  \x087\x09C\x0A8\x0A3\x0A6\x0A1\x0A8\x098\x0A9\x0E5\x098:";  // Θερμοκρασία
  const static char GRtxtHumidity[] = " \x007  Y\x09A\x0A8\x098\x0A9\x0E5\x098:";  // Υγρασία
  const static char GRtxtPressure[] = " \x007  A\x0AB\x0A3\x0A6\x0A9\x0AD\x098\x0A0\x0A8\x0A0\x0A1\x0E3 \x08F\x0E5\x09C\x0A9\x09E:";  // Ατμοσφαιρική Πίεση
  //const static char GRtxtAltitudey[] = " \x007  Y\x0AF\x0E6\x0A3\x09C\x0AB\x0A8\x0A6:";  // Υψόμετρο
  const static char Incorrect[]     = " *** NTP service provides incorrect time *** ";  //
  char              strTemperature[20];
  char              strHumidity[20];
  char              strPressure[24];
  //char              strAltitudey[20];
  char              strDate[11];
  char              strDay[11];

  GetLocalTime();

  if (Brightness != oldBrightness) {
    oldBrightness = Brightness;
    P.setIntensity(Brightness);
  }

  if (PreviousSec != Sec) {                 // the seconds changed
    PreviousSec = Sec;
    FlagSec = true;
    Dots = !Dots;

    if (Minute == 0 && Sec == 5) {          // every 1 Hour
      Flag_Pacman = true ;
      Flag_Small_Clock = true ;
    }

    if ((Sec == 30) && (Minute % 5 == 0))   // every 5 min
      Flag_Small_Clock = true ;
  }

  if (P.displayAnimate()) { // animates and returns true when an animation is completed
    if (Flag_Small_Clock) {
      if (!Set_2_Zones) {
        Setup2Zones();
      }

      if (P.getZoneStatus(ZONE_LOWER)) {
        if (Flag_Pacman) {
          Flag_Small_Clock = false ;
          Flag_Pacman  = false ;
        }
        else {
          Day2str(DayOfWeek, strDay, sizeof(strDay));
          strftime(strDate, sizeof(strDate), "%d-%m-%Y", timeinfo); // %F = 2019-08-19
          dtostrf (bme.readTemperature(), 3, 1, strTemperature);
          strcat (strTemperature, "$");

          if (Flag2ZonesB) {
            dtostrf (bme.readHumidity(), 3, 1, strHumidity);
            strcat (strHumidity, " % RH");
            dtostrf (bme.readPressure() / 100.0F, 4, 0, strPressure);
            strcat (strPressure, " mbar");

            if (FlagIncorrectTime ) { // if NTP service provides incorrect time
              sprintf(BufMsg_0, "%s %s %s %s %s %s %s %s %s   ", Incorrect, strDay, strDate, GRtxtTEMP,
                      strTemperature, GRtxtHumidity, strHumidity, GRtxtPressure, strPressure);
            }
            else {
              sprintf(BufMsg_0, "%s %s %s %s %s %s %s %s   ", strDay, strDate, GRtxtTEMP,
                      strTemperature, GRtxtHumidity, strHumidity, GRtxtPressure, strPressure);
            }
            Flag_Small_Clock = false ;
          }
          else {
            dtostrf (bme.readHumidity(), 3, 0, strHumidity);
            strcat (strHumidity, " %");
            sprintf(BufMsg_1, "%s   %s", strHumidity, strTemperature );
            if (FlagEffe)
              P.setTextEffect(ZONE_LOWER, PA_PRINT, PA_NO_EFFECT);
          }

          if (Sec > 42 && !Flag2ZonesB) {
            P.setTextEffect(ZONE_LOWER, PA_PRINT, PA_SCROLL_DOWN);
            P.setPause(ZONE_LOWER, 1200);
          }

          if (Sec > 44 && !Flag2ZonesB) {
            Flag_Small_Clock = false ;
            FlagEffe = false ;
          }
        }
        yield();
        P.displayReset(ZONE_LOWER);
      }

      // Small Digit Display Time
      if (FlagSec) {
        FlagSec = false;
        DisplayTime(Buf_time, Dots);
        P.displayReset(ZONE_one);   // Small Digit Display Time
      }
      yield();
    }

    // Display Big Digit Time
    else {
      if (!Set_4_Zones) {
        Setup4Zones();
        Flag2ZonesB = !Flag2ZonesB;
      }

      if (P.getZoneStatus(ZONE_LOWER) && P.getZoneStatus(ZONE_UPPER)) {

        if (FlagSec) {
          FlagSec = false;

          if (Sec % 5 == 0) {
            dtostrf (bme.readTemperature(), 3, 1, Temperature);
          }

          DisplayBigTime(Buf_TimeL, Dots);
          createHString(Buf_TimeUp, Buf_TimeL);

          P.displayReset();
          P.synchZoneStart();  // synchronize the start and run the display
        }
      }
      yield();
    }
    yield();
  }
} // == Close DotMatrixDisplay ==


// ======================================================================
// DotMatrix_setup
// ======================================================================
void DotMatrix_setup() {
  P.begin(MAX_ZONES);
  P.addChar('$', degC);
  P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
  Brightness = BRIGHT_VAL;  // Using defaul Brightness values
  yield();                  // -- Yield should not be necessary, but cannot hurt either.
} //=== close  DotMatrix_setup ===




/*********************************
  END Page
**********************************/
