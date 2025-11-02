#include "mqtt_manager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "../hardware/hardware_setup.h"
#include "../secrets.h"

static WiFiClientSecure mqttNet;
static PubSubClient mqttClient(mqttNet);

static const char* mqtt_server = "mqtt.ably.io";
static const int mqtt_port = 8883;
static String mqtt_client_id;

// Token cache for Ably token auth (fetched from ABLY_TOKEN_URL)
static String cachedToken = "";
static unsigned long tokenExpiryMs = 0;

// MQTT message callback — payload is NOT null-terminated, use length
static void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length + 1);
  for (unsigned int i = 0; i < length; ++i) msg += (char)payload[i];

  Serial.printf("MQTT msg [%s] len=%u\n", topic, length);

  String urlStr;
  if (msg.startsWith("http://") || msg.startsWith("https://")) {
    urlStr = msg;
  }
  if (urlStr.length() == 0) {
    int idx = msg.indexOf("data:");
    if (idx >= 0) {
      urlStr = msg.substring(idx + 5);
      urlStr.trim();
    }
  }
  if (urlStr.length() == 0) {
    if (msg.indexOf('{') >= 0 && msg.indexOf("\"url\"") >= 0) {
    JsonDocument d;
    DeserializationError err = deserializeJson(d, msg);
        if (!err) {
          const char* jurl = d["url"];
          if (jurl) urlStr = String(jurl);
        }
      }
  }
  if (urlStr.length() == 0) {
    Serial.println("Payload did not contain a playable URL — ignoring");
    return;
  }
  Serial.printf("Received URL to play: %s\n", urlStr.c_str());
  audio.stopSong();
  if (!audio.connecttohost(urlStr.c_str())) {
    Serial.println("Failed to start MP3 from MQTT payload");
  }
}

// Fetch an Ably token from the configured ABLY_TOKEN_URL. Returns token or
// empty string on failure. Also sets tokenExpiryMs using issued/expires when
// available (with a 30s refresh margin).
static String fetchAblyToken(const String &clientId) {
  if (WiFi.status() != WL_CONNECTED) return String();

  WiFiClientSecure https;
  https.setInsecure(); // dev only. Replace with setCACert() in production.
  HTTPClient http;
  String url = String(ABLY_TOKEN_URL) + "?clientId=" + clientId;
  if (!http.begin(https, url)) {
    Serial.println("HTTP begin failed for token URL");
    return String();
  }
  int code = http.GET();
  String body = http.getString();
  http.end();
  if (code != 200) {
    Serial.printf("Token endpoint returned %d\n", code);
    Serial.println(body.substring(0, min((int)body.length(), 256)));
    return String();
  }

  JsonDocument doc;
  DeserializationError derr = deserializeJson(doc, body);
  if (derr) {
    Serial.printf("Token JSON parse error: %s\n", derr.c_str());
    Serial.println(body.substring(0, min((int)body.length(), 256)));
    return String();
  }

  const char* t = doc["token"];
  if (!t) {
    Serial.println("Token not present in response");
    return String();
  }
  String token = String(t);

  unsigned long issued = doc["issued"] | 0UL;
  unsigned long expires = doc["expires"] | 0UL;
  unsigned long ttlMs = 0UL;
  if (issued > 0 && expires > issued) {
    unsigned long diff = expires - issued;
    if (diff < 86400UL) diff *= 1000UL;
    ttlMs = diff;
  }
  if (ttlMs == 0UL) ttlMs = 15UL * 60UL * 1000UL;

  unsigned long margin = 30UL * 1000UL;
  unsigned long now = millis();
  tokenExpiryMs = now + (ttlMs > margin ? ttlMs - margin : ttlMs);

  Serial.printf("Fetched token len=%u ttlMs=%lu\n", token.length(), ttlMs);
  return token;
}

static unsigned long lastMqttAttempt = 0;
static const unsigned long mqttRetryInterval = 5000; // ms

static bool tryMqttConnectOnce() {
  Serial.print("Attempting MQTT connection...");

  if (mqtt_client_id.length() == 0) mqtt_client_id = String("esp32-") + WiFi.macAddress();

  if (cachedToken.length() == 0 || (tokenExpiryMs && millis() > tokenExpiryMs)) {
    Serial.println("Fetching Ably token...");
    cachedToken = fetchAblyToken(mqtt_client_id);
    if (cachedToken.length() == 0) {
      Serial.println("Token fetch failed");
      return false;
    }
  }

  if (mqttClient.connect(mqtt_client_id.c_str(), cachedToken.c_str(), "")) {
    Serial.println("connected");
    if (mqttClient.subscribe("dropcall")) {
      Serial.println("Subscribed to topic: dropcall");
    } else {
      Serial.println("Failed to subscribe to dropcall");
    }
    return true;
  } else {
    int st = mqttClient.state();
    Serial.print("failed, rc=");
    Serial.println(st);
    if (st == 4) {
      Serial.println("Auth failed - clearing cached token");
      cachedToken = "";
      tokenExpiryMs = 0;
    }
    return false;
  }
}

void mqttManager_begin() {
  mqttNet.setInsecure(); // dev only
  mqtt_client_id = String("esp32-") + WiFi.macAddress();
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setCallback(mqttCallback);
}

void mqttManager_loop() {
  if (!mqttClient.connected()) {
    if (millis() - lastMqttAttempt > mqttRetryInterval) {
      lastMqttAttempt = millis();
      tryMqttConnectOnce();
    }
  } else {
    mqttClient.loop();
  }
}
