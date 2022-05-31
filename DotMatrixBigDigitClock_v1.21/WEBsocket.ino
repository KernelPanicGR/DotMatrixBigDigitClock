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
#if DEBUGLEVEL > 2 // DEBUG SERIAL
    if (client->lastId()) {
      DBGPSTRF("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
#endif //DEBUGLEVEL > 2
  });

  // event WiFi info
  eventinfo.onConnect([](AsyncEventSourceClient * client) {
    FlagEventInfo = true ;
#if DEBUGLEVEL > 2 // DEBUG SERIAL
    if (client->lastId()) {
      DBGPSTRF("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
#endif //DEBUGLEVEL > 2
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
#if DEBUGLEVEL > 2 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] connected from: %s\n", client->id(), server->url(), client->remoteIP().toString().c_str());
#endif //DEBUGLEVEL > 2
      FlagWsTime = true;
       client->ping();
      break;

    case WS_EVT_DISCONNECT: //client disconnected
#if DEBUGLEVEL > 2 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] disconnected\n", client->id(), server->url());
#endif //DEBUGLEVEL > 2
      OLDwday = 8;
      FlagWsTime = false ;
      break;

    case WS_EVT_DATA:
      WebSocketCurrentTime(arg, data, len);
      break;

    case WS_EVT_PONG: //pong message was received (in response to a ping request maybe)
#if DEBUGLEVEL > 2 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] pong[%u]: %s\n", client->id(),  server->url(), len, (len) ? (char*)data : "");
#endif //DEBUGLEVEL > 2
      break;

    case WS_EVT_ERROR:
#if DEBUGLEVEL >= 1 // DEBUG SERIAL
      DBGPSTRF("\r\n--- WS client #%u [%s] Error (%u): %s ---\n", client->id(), server->url(), *((uint16_t*)arg), (char*)data);
#endif //DEBUGLEVEL >= 1L
      OLDwday = 8;
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
#if DEBUGLEVEL > 2 // DEBUG SERIAL
    DBGPSTRF("\rReceive data: \n%s\n\n", ( char *)data);
#endif //DEBUGLEVEL > 2  
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
#if DEBUGLEVEL > 2 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] connected from: %s\n", client->id(), server->url(), client->remoteIP().toString().c_str());
#endif //DEBUGLEVEL > 2 
      SendBrightness();
      break;

    case WS_EVT_DISCONNECT: //client disconnected
#if DEBUGLEVEL > 2 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] disconnected\n", client->id(), server->url());
#endif //DEBUGLEVEL > 2
      break;

    case WS_EVT_DATA:
      WebSocketBrightness(arg, data, len);
      break;

    case WS_EVT_PONG: //pong message was received (in response to a ping request maybe)
#if DEBUGLEVEL > 2 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] pong[%u]: %s\n", client->id(),  server->url(), len, (len) ? (char*)data : "");
#endif //DEBUGLEVEL > 2
      break;

    case WS_EVT_ERROR:
#if DEBUGLEVEL >=1 // DEBUG SERIAL
      DBGPSTRF("WS client #%u [%s] error(%u): %s\n", client->id(), server->url(), *((uint16_t*)arg), (char*)data);
#endif //DEBUGLEVEL >= 1
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
#if DEBUGLEVEL > 2 // DEBUG SERIAL
    DBGPSTRF("\r----- Receive data from Brightness page: \n%s\n", ( char *)data);
    DBGPSTRF(DASHED_LINES);
#endif //DEBUGLEVEL > 2

    StaticJsonDocument<128> doc;
    deserializeJson(doc, (char*)data);

    if (doc.containsKey(LBL_BRIGHTNESS)) {
      Brightness = doc[LBL_BRIGHTNESS] | BRIGHT_VAL;

      if (oldBrightness != Brightness) {
        oldBrightness = Brightness;
        Save_FS_Brightness();
      }
    }
  }
  else
    DBGPSTRF("\r\n--- Received a ws message, but it didn't fit into one frame ---\n");
}//== Close WebSocketBrightness ===






/*********************************
  END Page
**********************************/
