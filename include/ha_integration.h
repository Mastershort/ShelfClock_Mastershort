// ha_integration.h
#ifndef HA_INTEGRATION_H
#define HA_INTEGRATION_H

#include <ArduinoHA.h>
#include <WiFiClient.h>
#include <FastLED.h>
#include <time.h>
#include "ShelfClock.h"

extern byte mac[];
extern WiFiClient haWiFiClient;
extern HAMqtt haMqtt;

extern byte clockMode;
extern byte clockDisplayType;
extern byte dateDisplayType;
extern byte pastelColors;
extern bool DSTime;
extern long gmtOffset_sec;
extern byte ClockColorSettings;
extern byte DateColorSettings;
extern int colonType;
extern byte ColorChangeFrequency;
extern byte randomMinPassed;
extern byte randomHourPassed;
extern byte randomDayPassed;
extern byte randomWeekPassed;
extern byte randomMonthPassed;
extern byte brightness;
extern bool colorchangeCD;
extern int scrollColorSettings;
extern bool scrollOverride;
extern bool scrollOptions1;
extern bool scrollOptions2;
extern bool scrollOptions3;
extern bool scrollOptions4;
extern bool scrollOptions7;
extern bool scrollOptions8;
extern int scrollFrequency;
extern int lightshowMode;
extern int suspendFrequency;
extern byte suspendType;
extern bool useSpotlights;
extern int spotlightsColorSettings;
extern int realtimeMode;
extern bool updateSettingsRequired;
extern int breakOutSet;
extern int isAsleep;
extern int currentMode;
extern int currentReal;
extern int scoreboardLeft;
extern int scoreboardRight;
extern unsigned long countdownMilliSeconds;
extern unsigned long endCountDownMillis;
extern unsigned long CountUpMillis;
extern uint32_t countdownColorValue;
extern uint32_t hourColorValue;
extern uint32_t colonColorValue;
extern uint32_t minColorValue;
extern uint32_t spotlightsColorValue;
extern uint32_t scrollColorValue;
extern uint32_t scoreboardColorLeftValue;
extern uint32_t scoreboardColorRightValue;
extern uint32_t dayColorValue;
extern uint32_t monthColorValue;
extern uint32_t separatorColorValue;
extern CRGB countdownColor;
extern CRGB hourColor;
extern CRGB colonColor;
extern CRGB minColor;
extern CRGB spotlightsColor;
extern CRGB scrollColor;
extern CRGB scoreboardColorLeft;
extern CRGB scoreboardColorRight;
extern CRGB dayColor;
extern CRGB monthColor;
extern CRGB separatorColor;
extern CRGB oldsnakecolor;
extern int getSlower;
extern const int daylightOffset_sec;
extern const char* ntpServer;
extern struct tm timeinfo;

void setupHA();
void setDisplayMode(String mode);
void allBlank();
void printLocalTime();
void ShelfDownLights();
void getclockSettings(String fileType);
void saveclockSettings(String fileType);
void haSyncState();

#endif // HA_INTEGRATION_H
