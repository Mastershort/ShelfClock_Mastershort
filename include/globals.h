#pragma once
#define FASTLED_ESP32_FLASH_LOCK 1
#define FASTLED_INTERNAL
#include <Arduino.h>
#include <FastLED.h>
#include <FS.h>
#include "WebServer.h"
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ArduinoHA.h>
#include <Preferences.h>
#include <LittleFS.h>

// --- Filesystem ---
#define FileFS        LittleFS
#define FS_Name       "LittleFS"

// --- String helpers ---
#define STRINGIFY_HELPER(x) #x
#define STRINGIFY(x) STRINGIFY_HELPER(x)

// --- LED configuration constants ---
#define LED_TYPE          WS2812B
#define COLOR_ORDER       GRB
#define SEGMENTS_PER_NUMBER 7
#define NUMBER_OF_DIGITS    7
#define SPECTRUM_PIXELS     37
#define LED_PIN             15
#define MILLI_AMPS          2400
#define LEDS_PER_SEGMENT    9
#define LEDS_PER_DIGIT      (LEDS_PER_SEGMENT * SEGMENTS_PER_NUMBER)
#define FAKE_NUM_LEDS       (NUMBER_OF_DIGITS * LEDS_PER_DIGIT)
#define SEGMENTS_LEDS       (SPECTRUM_PIXELS * LEDS_PER_SEGMENT)
#define SPOT_LEDS           (NUMBER_OF_DIGITS * 2)
#define NUM_LEDS            (SEGMENTS_LEDS + SPOT_LEDS)

// --- WiFi ---
#define WiFi_MAX_RETRIES        100
#define WiFi_MAX_RETRY_DURATION 600000

// --- Debug log ---
#define DEBUG_LOG_SIZE 50
#define DEBUG_LOG_LINE 96

// =============================================
// Extern declarations for all shared globals
// (defined in ShelfClock.cpp unless noted)
// =============================================

// --- LED arrays ---
extern CRGB LEDs[];
extern byte numbers[];
extern const uint16_t FAKE_LEDs[];
extern const uint16_t FAKE_LEDs_C_BMUP[];
extern const uint16_t FAKE_LEDs_C_CMOT[];
extern const uint16_t FAKE_LEDs_C_BLTR[];
extern const uint16_t FAKE_LEDs_C_TLBR[];
extern const uint16_t FAKE_LEDs_C_TMDN[];
extern const uint16_t FAKE_LEDs_C_CSIN[];
extern const uint16_t FAKE_LEDs_C_BRTL[];
extern const uint16_t FAKE_LEDs_C_TRBL[];
extern const uint16_t FAKE_LEDs_C_VERT[];
extern const uint16_t FAKE_LEDs_C_VERT2[];
extern const uint16_t FAKE_LEDs_C_OUTS[];
extern const uint16_t FAKE_LEDs_C_OUTS2[];
extern const uint16_t FAKE_LEDs_C_FIRE[];
extern const uint16_t FAKE_LEDs_C_RAIN[];
extern const uint16_t FAKE_LEDs_SNAKE[];

// --- Effect state arrays ---
extern byte rain[];
extern byte greenMatrix[];

// --- Core objects ---
extern Preferences preferences;
extern WebServer server;
extern PubSubClient mqttClient;
extern WiFiClient mqttWiFiClient;
extern WiFiClient haWiFiClient;
extern HAMqtt haMqtt;
extern DynamicJsonDocument jsonScheduleData;
extern File fsUploadFile;
extern struct tm timeinfo;

// --- Network state ---
extern bool mqttConnected;
extern unsigned long lastMqttReconnectAttempt;
extern int WiFi_retryCount;
extern int WiFi_totalReconnections;
extern unsigned long WiFi_startTime;
extern unsigned long WiFi_elapsedTime;

// --- Mode / display settings (saved to flash) ---
extern byte clockMode;
extern byte clockDisplayType;
extern byte dateDisplayType;
extern byte pastelColors;
extern bool DSTime;
extern long gmtOffset_sec;
extern String timeZone;   // POSIX TZ string, e.g. "CET-1CEST,M3.5.0,M10.5.0/3"; empty = manual gmtOffset_sec + DSTime
extern byte ClockColorSettings;
extern byte DateColorSettings;
extern int colonType;
extern byte ColorChangeFrequency;
extern byte brightness;
extern String scrollText;
extern bool colorchangeCD;
extern int realtimeMode;
extern int spotlightsColorSettings;
extern bool useSpotlights;
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

// --- Color values (uint32 + CRGB pairs) ---
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
extern CRGB tinyhourColor;
extern CRGB spotlightsColor;
extern CRGB scrollColor;
extern CRGB scoreboardColorLeft;
extern CRGB scoreboardColorRight;
extern CRGB dayColor;
extern CRGB monthColor;
extern CRGB tinymonthColor;
extern CRGB separatorColor;
extern CRGB lightchaseColorOne;
extern CRGB lightchaseColorTwo;
extern CRGB alternateColor;
extern CRGB oldsnakecolor;
extern CRGB spotcolor;

// --- Runtime state ---
extern int breakOutSet;
extern int isAsleep;
extern int currentMode;
extern int currentReal;
extern int scoreboardLeft;
extern int scoreboardRight;
extern float haDisplayValue;
extern bool haDisplayDegree;
extern unsigned long countdownMilliSeconds;
extern unsigned long endCountDownMillis;
extern unsigned long countupMilliSeconds;
extern unsigned long CountUpMillis;
extern int colorWheelPosition;
extern int colorWheelPositionTwo;
extern const int colorWheelSpeed;
extern bool dotsOn;
extern unsigned long prevTime;
extern unsigned long prevTime2;
extern int lightshowSpeed;
extern int snakeLastDirection;
extern int snakePosition;
extern int foodSpot;
extern int snakeWaiting;
extern int getSlower;
extern int Level;
extern int cylonPosition;
extern int clearOldLeds;
extern bool updateSettingsRequired;
extern unsigned long settingsSaveTimer;
extern byte randomMinPassed;
extern byte randomHourPassed;
extern byte randomDayPassed;
extern byte randomWeekPassed;
extern byte randomMonthPassed;

// --- Timing / NTP ---
extern const int daylightOffset_sec;
extern const char* ntpServer;

// --- Debug log ---
extern char debugLog[DEBUG_LOG_SIZE][DEBUG_LOG_LINE];
extern int debugLogIndex;
extern char lastUpdateBody[256];
extern unsigned long lastUpdateAt;

// --- Uptime ---
extern int daysUptime;
extern int hoursUptime;
extern int minutesUptime;

// --- Notify queue (MQTT shelfclock/notify + HTTP /notify) ---
extern bool notifyPending;
extern String notifyText;
extern uint32_t notifyColorValue;
extern int notifyRepeat;

// --- OTA update state ---
extern bool otaInProgress;       // true while /update or /updatefs uploads run - loop() must not touch LEDs or LittleFS
extern const char* lastResetReason;

// --- Other ---
extern bool firstRun;
extern int sleepTimerCurrent;
extern String softwareVersion;
extern const char* host;
extern int previousTimeMin;
extern int previousTimeHour;
extern int previousTimeDay;
extern int previousTimeWeek;
extern int previousTimeMonth;

// --- Party Games ---
extern int partyGameType;

// --- Logging ---
void webLog(const String& msg);
