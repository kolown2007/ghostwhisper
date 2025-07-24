#include "realtime_manager.h"
#include "../config/secrets.h"
#include <WebSocketsClient.h>

WebSocketsClient webSocket;

void realtimeEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.println("[Realtime] Disconnected");
            break;
        case WStype_CONNECTED:
            Serial.println("[Realtime] Connected to server");
            break;
        case WStype_TEXT:
            Serial.print("[Realtime] Received text: ");
            Serial.println((char*)payload);
            // TODO: Handle the incoming message (e.g., parse JSON, update sequence)
            break;
        default:
            break;
    }
}

void setupRealtime(const char* url) {
    String wsUrl(url);
    bool useSSL = wsUrl.startsWith("wss://");
    int protoLen = useSSL ? 6 : 5;
    int colonIdx = wsUrl.indexOf(':', protoLen);
    int slashIdx = wsUrl.indexOf('/', colonIdx);
    String host = wsUrl.substring(protoLen, colonIdx);
    int port = wsUrl.substring(colonIdx + 1, slashIdx).toInt();
    String path = wsUrl.substring(slashIdx);
    if (useSSL) {
        webSocket.beginSSL(host.c_str(), port, path.c_str());
    } else {
        webSocket.begin(host.c_str(), port, path.c_str());
    }
    webSocket.onEvent(realtimeEvent);
    webSocket.setReconnectInterval(5000);
}

void pollRealtime() {
    webSocket.loop();
}
