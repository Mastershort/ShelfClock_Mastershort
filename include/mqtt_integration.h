#pragma once
#include "mqtt_integration.h"
#include <PubSubClient.h>
#include <Preferences.h>
#include "ha_integration.h"

extern Preferences preferences;
extern PubSubClient mqttClient;
extern bool mqttConnected;

// MQTT configuration variables
extern String mqttServer;
extern uint16_t mqttPort;
extern String mqttUser;
extern String mqttPassword;
extern String mqttClientId;

void mqttCallback(char* topic, byte* payload, unsigned int length);
void setupMQTT();
bool mqttReconnect();







