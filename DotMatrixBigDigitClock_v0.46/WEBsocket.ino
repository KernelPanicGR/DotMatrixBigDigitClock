// ======================================================================
// WebSocket_setup
// ======================================================================
void WebSocket_setup() {
  // attach AsyncWebSocket
  wsBright.onEvent(onWebSocketBrightness);
  wsTime.onEvent(onWebSocketCurrentTime);
  server.addHandler(&wsBright);
  server.addHandler(&wsTime);

  // event weather
  eventWeather.onConnect([](AsyncEventSourceClient * client) {
    FlagEventWather = true ;
#ifdef DEBUG_SERIAL
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
#endif // DEBUG_SERIAL
  });

  // event WiFi info
  eventinfo.onConnect([](AsyncEventSourceClient * client) {
    FlagEventInfo = true ;
#ifdef DEBUG_SERIAL
    if (client->lastId()) {
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
#endif // DEBUG_SERIAL
  });

  // attach AsyncEventSource
  server.addHandler(&eventWeather); // event weather
  server.addHandler(&eventinfo);    // event WiFi info
}


// ======================================================================
// onWebSocketCurrentTime
// ======================================================================
void onWebSocketCurrentTime(AsyncWebSocket       *server,
                            AsyncWebSocketClient *client,
                            AwsEventType          type,
                            void                 *arg,
                            uint8_t              *data,
                            size_t                len) {

  switch (type) {
    case WS_EVT_CONNECT:  //client connected
#ifdef DEBUG2_SERIAL
      Serial.printf("WS client #%u [%s] connected from: %s\n", client->id(), server->url(), client->remoteIP().toString().c_str());
#endif // DEBUG2_SERIAL
      FlagWsTime = true ;
      break;

    case WS_EVT_DISCONNECT: //client disconnected
#ifdef DEBUG2_SERIAL
      Serial.printf("WS client #%u [%s] disconnected\n", client->id(), server->url());
#endif // DEBUG2_SERIAL
      FlagWsTime = false ;
      break;

    case WS_EVT_DATA:
      WebSocketCurrentTime(arg, data, len);
      break;

    case WS_EVT_PONG: //pong message was received (in response to a ping request maybe)
#ifdef DEBUG2_SERIAL
      Serial.printf("WS client #%u [%s] pong[%u]: %s\n", client->id(),  server->url(), len, (len) ? (char*)data : "");
#endif // DEBUG2_SERIAL
      break;

    case WS_EVT_ERROR:
      printf("\r\n--- WS client #%u [%s] Error (%u): %s ---\n", client->id(), server->url(), *((uint16_t*)arg), (char*)data);
      FlagWsTime = false ;
      break;
  }
}//== Close onWebSocketCurrentTime ===


// ======================================================================
// WebSocketCurrentTime
// ======================================================================
void WebSocketCurrentTime(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
#ifdef DEBUG2_SERIAL
    Serial.printf_P(PSTR("\rReceive data: \n%s\n\n"), ( char *)data);
#endif // DEBUG2_SERIAL
  } else
    printf("\r\n--- Received a ws message, but it didn't fit into one frame ---\n");
}//== Close WebSocketCurrentTime ===


// ======================================================================
// onWebSocketBrightness
// ======================================================================
void onWebSocketBrightness(AsyncWebSocket       *server,
                           AsyncWebSocketClient *client,
                           AwsEventType          type,
                           void                 *arg,
                           uint8_t              *data,
                           size_t                len)   {
  switch (type) {

    case WS_EVT_CONNECT:  //client connected
#ifdef DEBUG2_SERIAL
      Serial.printf("WS client #%u [%s] connected from: %s\n", client->id(), server->url(), client->remoteIP().toString().c_str());
#endif // DEBUG2_SERIAL
      SendBrightness();
      break;

    case WS_EVT_DISCONNECT: //client disconnected
#ifdef DEBUG2_SERIAL
      Serial.printf("WS client #%u [%s] disconnected\n", client->id(), server->url());
#endif // DEBUG2_SERIAL
      break;

    case WS_EVT_DATA:
      WebSocketBrightness(arg, data, len);
      break;

    case WS_EVT_PONG: //pong message was received (in response to a ping request maybe)
#ifdef DEBUG2_SERIAL
      Serial.printf("WS client #%u [%s] pong[%u]: %s\n", client->id(),  server->url(), len, (len) ? (char*)data : "");
#endif // DEBUG2_SERIAL
      break;

    case WS_EVT_ERROR:
      Serial.printf("WS client #%u [%s] error(%u): %s\n", client->id(), server->url(), *((uint16_t*)arg), (char*)data);
      break;
  }
}//== Close onWebSocketBrightness ===


// ======================================================================
// WebSocketBrightness
// ======================================================================
void WebSocketBrightness(void *arg, uint8_t *data, size_t len) {
  static uint8_t oldBrightness;

  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
#ifdef DEBUG2_SERIAL
    Serial.printf_P(PSTR("\rReceive data: \n%s\n\n"), ( char *)data);
#endif // DEBUG2_SERIAL

    StaticJsonDocument<1024> doc;
    deserializeJson(doc, (char*)data);

    if (doc.containsKey(BRITGHTNS_JS)) {
      Brightness = doc[BRITGHTNS_JS] | 8;
      if (oldBrightness != Brightness) {
        oldBrightness = Brightness;

        if (Save_FS_Brightness())  {
#ifdef DEBUG2_SERIAL
          printf("\r \n");
          PrintShortLines();
#endif // DEBUG2_SERIAL 
          SendBrightness();
        } else {
          printf("\r \n");
          Serial.printf_P(PSTR("\r\n--- ERROR --- Failed to write to file: %s \n"), BRITGHTNS_JS);
        }
      }
    }
  } else
    printf("\r\n--- Received a ws message, but it didn't fit into one frame ---\n");
}//== Close WebSocketBrightness ===






/*********************************
  END Page
**********************************/
