// ======================================================================
// SendSensorsData
// ======================================================================
void SendSensorsData() {
  char  Humidity[16];
  char  Pressure[16];
  char  Temp[16];

  if (FlagSensors) {
    dtostrf(bme.readTemperature(), 3, 1,        Temp);
    dtostrf(bme.readHumidity(), 3, 1,           Humidity);
    dtostrf(bme.readPressure() / 100.0F, 4, 0,  Pressure);
  }
  else { // If you don't have a BME280 put only the library and test with random value
    dtostrf(random(1000, 4900) / 100., 3, 1,    Temp);
    dtostrf(random(3000, 9200) / 100., 3, 1,    Humidity);
    dtostrf(random(300,  1045), 4, 0,           Pressure);
  }

  //events.send("ping",           NULL,           millis());
  eventWeather.send(Temp,       "Temperature",  millis());
  eventWeather.send(Humidity,   "Humidity",     millis());
  eventWeather.send(Pressure,   "Pressure",     millis());
  FlagEventWather = false ;

#if DEBUGLEVEL > 3 //Serial Debugging  
  DBGPSTRF("\r----- Sending Sensors Value to weather page -----------\n\r");
  if (!FlagSensors)
    DBGPSTRF("\r--- [Error] : Not find a BME280 sensor. Send Random value for test\n\r");
    
  DBGPSTRF("Temperature : %s *C\n",     Temp );
  DBGPSTRF("Humidity    : %s %%\n",      Humidity );
  DBGPSTRF("Pressure    : %s mbar\n",    Pressure );
#endif //Serial Debugging > 3 
}


/* ======================================================================
  Sensor_setup
  ======================================================================= */
void Sensor_setup() {
#if DEBUGLEVEL >= 1  //DEBUG SERIAL    
  DBGPSTRF("\r\n--------- Start the BME sensor ------------------------\n\r");
#endif //DEBUGLEVEL >= 1
  if (! bme.begin(BME_ADDRES, &Wire)) {
    FlagSensors = false ;
#if DEBUGLEVEL >= 1  //DEBUG SERIAL
    DBGPSTRF("---[ERROR]: Could not find a valid BME280 sensor, check wiring! \n\r");
    DBGPSTRF("--- [info]: Use random valum for test\n\r");
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL >= 1
  }
  else {
    FlagSensors = true ;
#if DEBUGLEVEL >= 1  //DEBUG SERIAL  
    DBGPSTRF("--- [info]: OK find a BME280 sensor\n");
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL >= 1
  }

  dtostrf (bme.readTemperature(), 3, 1, Temperature);
}// == Close Sensor_setup ===


/*********************************
  END Page
**********************************/
