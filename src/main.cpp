
#include "Arduino.h"
#include "config/config.h"
#include "hardware/hardware_setup.h"
#include "managers/connection_manager.h"
#include "managers/debug_manager.h"
#include "secrets.h"
#include <esp_task_wdt.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClientSecure espClient;
PubSubClient client(espClient);

const char* mqtt_server = "mqtt.ably.io";
const int mqtt_port = 8883;
String mqtt_client_id;
const char* mqtt_username = ""; // from include/secrets.h
const char* mqtt_password = "";// from include/secrets.h

// MQTT message callback — payload is NOT null-terminated, use length
static void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  msg.reserve(length + 1);
  for (unsigned int i = 0; i < length; ++i) msg += (char)payload[i];

  Serial.printf("MQTT msg [%s] len=%u\n", topic, length);


  String urlStr;

  // 1) raw URL
  if (msg.startsWith("http://") || msg.startsWith("https://")) {
    urlStr = msg;
  }

  // 2) simple key/value: look for "data:" then take the remainder as URL
  if (urlStr.length() == 0) {
    int idx = msg.indexOf("data:");
    if (idx >= 0) {
      urlStr = msg.substring(idx + 5); // after "data:"
      urlStr.trim();
    }
  }

  // 3) JSON fallback
  if (urlStr.length() == 0) {
    // quick JSON detection: look for '{' and '"url"'
    if (msg.indexOf('{') >= 0 && msg.indexOf("\"url\"") >= 0) {
      DynamicJsonDocument d(1024);
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

// Non-blocking MQTT connect helper. The previous blocking reconnect() used a
// while loop + delay(5000) which prevents `audio.loop()` from running and
// causes playback hiccups. We instead attempt a single connect and return.
static unsigned long lastMqttAttempt = 0;
static const unsigned long mqttRetryInterval = 5000; // ms

static bool tryMqttConnectOnce() {
  Serial.print("Attempting MQTT connection...");
  if (client.connect(mqtt_client_id.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("connected");
    // Subscribe to the dropcall topic after connecting
    if (client.subscribe("dropcall")) {
      Serial.println("Subscribed to topic: dropcall");
    } else {
      Serial.println("Failed to subscribe to dropcall");
    }
    return true;
  } else {
    int st = client.state();
    Serial.print("failed, rc=");
    Serial.println(st);
    return false;
  }
}



void setup() {
    // Basic serial + watchdog setup
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial) { ; }

    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);

    configTime(0, 0, "pool.ntp.org", "time.google.com");

    // Initialize hardware (audio object, SD optional)
    initializeHardware();
    audio.setVolume(DEFAULT_VOLUME);

    // Initialize network (online-only connection manager)
    Serial.println("Initializing network connection...");
    initializeConnection(ONLINE);

    // If Wi‑Fi connected, play an MP3 from a URL once
    {
        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - start) < 10000) {
            delay(100);
        }
        if (WiFi.status() == WL_CONNECTED) {
            const char* url = "https://kolown.net/audiolib/bluetooth_pair.mp3"; // TODO: replace with your MP3 URL
            if (!audio.connecttohost(url)) {
                Serial.println("Failed to start MP3 playback");
            }
        } else {
            Serial.println("Wi‑Fi not connected; skipping initial MP3");
        }


  espClient.setInsecure();

  // Build a unique client id from MAC
  mqtt_client_id = String("esp32-") + WiFi.macAddress();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);






    }

   

    // Reduce watchdog timeout for normal operation
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
}

void loop() {
    // Reset watchdog timer
    esp_task_wdt_reset();

    // Drive audio processing
    audio.loop();

  // Non-blocking MQTT reconnect attempts. Keep audio.loop() running each
  // iteration to avoid playback glitches.
  if (!client.connected()) {
    if (millis() - lastMqttAttempt > mqttRetryInterval) {
      lastMqttAttempt = millis();
      tryMqttConnectOnce();
    }
  } else {
    // Only pump the MQTT client when connected
    client.loop();
  }

  

   
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > DEBUG_INTERVAL_MS) {
        logAudioStatus();
        monitorSystemHealth();
        lastDebugTime = millis();
    }
}

