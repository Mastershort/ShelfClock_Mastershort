#include "mqtt_integration.h"
#include "prefs.h"
#include "ha_integration.h" // für setDisplayMode
#include "../include/globals.h"
#include <PubSubClient.h>

extern PubSubClient mqttClient;
extern bool mqttConnected;

// Global MQTT configuration variables
String mqttServer;
uint16_t mqttPort;
String mqttUser;
String mqttPassword;
String mqttClientId;

void setupMQTT() {
    // Load MQTT settings from Preferences
    mqttServer   = prefGetStr("mqttServer", "");
    mqttPort     = (uint16_t)prefGetInt("mqttPort", 1883);
    mqttUser     = prefGetStr("mqttUser", "");
    mqttPassword = prefGetStr("mqttPassword", "");
    mqttClientId = "ShelfClock_" + String((uint32_t)ESP.getEfuseMac(), HEX);

    if (mqttServer.length() > 0) {
        Serial.print("MQTT Server: ");
        Serial.println(mqttServer);
        Serial.print("MQTT Port: ");
        Serial.println(mqttPort);

        mqttClient.setBufferSize(512);  // room for notify JSON payloads
        mqttClient.setServer(mqttServer.c_str(), mqttPort);
        mqttClient.setCallback(mqttCallback);
        mqttReconnect();
    } else {
        Serial.println("MQTT Server not configured, skipping MQTT setup");
    }
}

// Queues a notification for the main loop to scroll. Accepts a JSON payload
// {"text": "...", "color": "FF0000", "repeat": 2} or plain text.
// Returns false when no usable text was found.
bool queueNotify(const String& message) {
  String text = "";
  uint32_t color = 0xFFFFFF;
  int repeat = 1;

  DynamicJsonDocument doc(512);
  if (deserializeJson(doc, message) == DeserializationError::Ok && !doc["text"].isNull()) {
    text = doc["text"].as<String>();
    if (!doc["color"].isNull()) {
      String c = doc["color"].as<String>();
      if (c.startsWith("#")) { c = c.substring(1); }
      color = strtoul(c.c_str(), NULL, 16);
    }
    if (!doc["repeat"].isNull()) {
      repeat = constrain(doc["repeat"].as<int>(), 1, 5);
    }
  } else {
    text = message;  // not JSON - treat the whole payload as the text
  }

  text.trim();
  if (text.length() == 0) { return false; }
  notifyText = text;
  notifyColorValue = color;
  notifyRepeat = repeat;
  notifyPending = true;
  webLog("Notify queued: " + text);
  return true;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "shelfclock/mode/set") {
    setDisplayMode(message);
    mqttClient.publish("shelfclock/mode/state", message.c_str(), true);
  } else if (String(topic) == "shelfclock/notify") {
    queueNotify(message);
  }
}

bool mqttReconnect() {
  if (mqttServer.length() == 0) return false;
  if (!mqttClient.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (mqttUser.length() > 0) {
      mqttConnected = mqttClient.connect(mqttClientId.c_str(),
                                         mqttUser.c_str(),
                                         mqttPassword.c_str());
    } else {
      mqttConnected = mqttClient.connect(mqttClientId.c_str());
    }

    if (mqttConnected) {
      Serial.println("MQTT connected!");
      // Subscribe to command topics
      mqttClient.subscribe("shelfclock/mode/set");
      mqttClient.subscribe("shelfclock/notify");
      Serial.println("Subscribed to: shelfclock/mode/set, shelfclock/notify");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.println(mqttClient.state());
    }
  }
  return mqttConnected;
}
