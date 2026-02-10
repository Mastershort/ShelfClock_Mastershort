#include "mqtt_integration.h"
#include "prefs.h"
#include "ha_integration.h" // für setDisplayMode
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
    // mqttServer   = prefGetStr("mqttServer", "");
    // mqttPort     = (uint16_t)prefGetInt("mqttPort", 1883);
    // mqttUser     = prefGetStr("mqttUser", "");
    // mqttPassword = prefGetStr("mqttPassword", "");
    mqttServer   = "192.168.50.11";
    mqttPort     = (uint16_t)prefGetInt("mqttPort", 1883);
    mqttUser     = prefGetStr("mqttUser", "mqttUser");
    mqttPassword = prefGetStr("mqttPassword", "mqttPassword");
    mqttClientId = "ShelfClock_" + String((uint32_t)ESP.getEfuseMac(), HEX);

    if (mqttServer.length() > 0) {
        Serial.print("MQTT Server: ");
        Serial.println(mqttServer);
        Serial.print("MQTT Port: ");
        Serial.println(mqttPort);

        mqttClient.setServer(mqttServer.c_str(), mqttPort);
        mqttClient.setCallback(mqttCallback);
        mqttReconnect();
    } else {
        Serial.println("MQTT Server not configured, skipping MQTT setup");
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (String(topic) == "shelfclock/mode/set") {
    setDisplayMode(message);
    mqttClient.publish("shelfclock/mode/state", message.c_str(), true);
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
      Serial.println("Subscribed to: shelfclock/mode/set");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.println(mqttClient.state());
    }
  }
  return mqttConnected;
}
