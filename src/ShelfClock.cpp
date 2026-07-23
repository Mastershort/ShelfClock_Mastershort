#define FASTLED_ESP32_FLASH_LOCK 1
#define FASTLED_INTERNAL
#include "../include/globals.h"
#include <WiFi.h>
#include <HTTPUpdateServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <AutoConnect.h>
#include "../include/ShelfClock.h"
#include "../include/ha_integration.h"
#include "../include/mqtt_integration.h"
#include "../include/prefs.h"
#include "../include/display_modes.h"
#include "../include/lightshow.h"
#include "../include/settings_manager.h"
#include "../include/web_handlers.h"
#include "../include/party_games.h"
#include "../include/update_check.h"
Preferences preferences;


WiFiClient   mqttWiFiClient;
WiFiClient   haWiFiClient;
PubSubClient mqttClient(mqttWiFiClient);
bool mqttConnected = false;
unsigned long lastMqttReconnectAttempt = 0;



FS* filesystem =      &LittleFS;

// Digit layout macros (used to initialize FAKE_LEDs arrays)
#define digit0 seg(0), seg(1), seg(2), seg(3), seg(4), seg(5), seg(6)
#define fdigit1 seg(2), seg(7), seg(10), seg(15), seg(8), seg(3), seg(9)
#define digit2 seg(10), seg(11), seg(12), seg(13), seg(14), seg(15), seg(16)
#define fdigit3 seg(12), seg(17), seg(20), seg(25), seg(18), seg(13), seg(19)
#define digit4 seg(20), seg(21), seg(22), seg(23), seg(24), seg(25), seg(26)
#define fdigit5 seg(22), seg(27), seg(30), seg(35), seg(28), seg(23), seg(29)
#define digit6 seg(30), seg(31), seg(32), seg(33), seg(34), seg(35), seg(36)

#if LEDS_PER_SEGMENT == 1
 #define seg(n) n*LEDS_PER_SEGMENT
 #define nseg(n) n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 2
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1
 #define nseg(n) n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 3
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2
 #define nseg(n) n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 4
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3
 #define nseg(n) n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 5
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+4
 #define nseg(n) n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 6
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+5
 #define nseg(n) n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 7
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+6
 #define nseg(n) n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 8
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+7
 #define nseg(n) n*LEDS_PER_SEGMENT+7, n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 9
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+7, n*LEDS_PER_SEGMENT+8
 #define nseg(n) n*LEDS_PER_SEGMENT+8, n*LEDS_PER_SEGMENT+7, n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#elif LEDS_PER_SEGMENT == 10
 #define seg(n) n*LEDS_PER_SEGMENT, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+7, n*LEDS_PER_SEGMENT+8, n*LEDS_PER_SEGMENT+9
 #define nseg(n) n*LEDS_PER_SEGMENT+9, n*LEDS_PER_SEGMENT+8, n*LEDS_PER_SEGMENT+7, n*LEDS_PER_SEGMENT+6, n*LEDS_PER_SEGMENT+5, n*LEDS_PER_SEGMENT+4, n*LEDS_PER_SEGMENT+3, n*LEDS_PER_SEGMENT+2, n*LEDS_PER_SEGMENT+1, n*LEDS_PER_SEGMENT
#else
 #error "Not supported Leds per segment. You need to add definition of seg(n) with needed number of elements according to formula above"
#endif



String softwareVersion = "version-2.1.1";
const char* host = "shelfclock";
const int   daylightOffset_sec = 3600;
const char* ntpServer = "pool.ntp.org";

unsigned long WiFi_startTime = 0;
unsigned long WiFi_elapsedTime = 0;
int WiFi_retryCount = 0;
int WiFi_totalReconnections = 0;
int breakOutSet = 0; //jump out of count
int colorWheelPosition = 255; // COLOR WHEEL POSITION
int colorWheelPositionTwo = 255; // 2nd COLOR WHEEL POSITION
const int colorWheelSpeed = 3;
int sleepTimerCurrent = 0;
int isAsleep = 0;
int previousTimeMin = 0;
int previousTimeHour = 0;
int previousTimeDay = 0;
int previousTimeWeek = 0;
int previousTimeMonth = 0;
byte randomMinPassed = 1;
byte randomHourPassed = 1;
byte randomDayPassed = 1;
byte randomWeekPassed = 1;
byte randomMonthPassed = 1;
bool dotsOn = true;
unsigned long prevTime = 0;
unsigned long prevTime2 = 0;
unsigned long countdownMilliSeconds;
unsigned long endCountDownMillis;
unsigned long countupMilliSeconds;
unsigned long CountUpMillis;
int scoreboardLeft = 0;
int scoreboardRight = 0; 
int currentMode = 0;
int currentReal = 0;
int cylonPosition = 0;
int clearOldLeds = 0; 
byte rain[SPECTRUM_PIXELS];
byte greenMatrix[SPECTRUM_PIXELS];
int lightshowSpeed = 1;
int snakeLastDirection = 0;  //snake's last dirction
int snakePosition = 0;  //snake's position
int foodSpot = random(SPECTRUM_PIXELS-1);  //food spot
int snakeWaiting = 0;  //waiting
int getSlower = 180;
int Level = 1;
int daysUptime = 0;
int hoursUptime = 0;
int minutesUptime = 0;

bool notifyPending = false;
String notifyText = "";
uint32_t notifyColorValue = 0xFFFFFF;
int notifyRepeat = 1;

bool otaInProgress = false;

// --- Boot-loop guard ---
// Survives software resets/panics (RTC memory keeps its value across
// ESP.restart()/crash reboots) but is cleared on power loss - exactly what
// we want: count only *crash* reboots, not normal power cycling.
#define BOOTLOOP_MAGIC 0xA5A51234
RTC_NOINIT_ATTR uint32_t bootLoopMagic;
RTC_NOINIT_ATTR uint8_t bootLoopCount;
bool bootLoopSafeMode = false;
const char* lastResetReason = "unknown";

static const char* resetReasonToString(esp_reset_reason_t r) {
  switch (r) {
    case ESP_RST_POWERON:   return "PowerOn (normal)";
    case ESP_RST_EXT:       return "ExternalPin";
    case ESP_RST_SW:        return "Software (ESP.restart)";
    case ESP_RST_PANIC:     return "Panic/Crash";
    case ESP_RST_INT_WDT:   return "InterruptWatchdog";
    case ESP_RST_TASK_WDT:  return "TaskWatchdog";
    case ESP_RST_WDT:       return "OtherWatchdog";
    case ESP_RST_DEEPSLEEP: return "DeepSleep";
    case ESP_RST_BROWNOUT:  return "Brownout (power!)";
    case ESP_RST_SDIO:      return "SDIO";
    default:                return "Unknown";
  }
}

bool updateSettingsRequired = false;
unsigned long settingsSaveTimer = 0;  // Timer for delayed settings save
char lastUpdateBody[256] = "";
unsigned long lastUpdateAt = 0;

// Web Debug Log – fixed-size char buffers to avoid heap fragmentation
#define DEBUG_LOG_SIZE 50
#define DEBUG_LOG_LINE 96
char debugLog[DEBUG_LOG_SIZE][DEBUG_LOG_LINE];
int debugLogIndex = 0;

void webLog(const String& msg) {
  Serial.println(msg);
  snprintf(debugLog[debugLogIndex], DEBUG_LOG_LINE, "%lu: %s", millis(), msg.c_str());
  debugLogIndex = (debugLogIndex + 1) % DEBUG_LOG_SIZE;
}

DynamicJsonDocument jsonScheduleData(8192);  //stores schedules
File fsUploadFile;

struct tm timeinfo; 
CRGB LEDs[NUM_LEDS];
WebServer server(80);
HTTPUpdateServer httpUpdateServer;
AutoConnect      Portal(server);
AutoConnectConfig  Config;

// global settings that get saved to flash via preffs
byte clockMode = 11;   // Clock modes: 0=Clock, 1=Countdown, 3=Scoreboard, 4=Stopwatch, 5=Lightshow, 6=Rainbows, 7=Date, 10=Display Off, 11=Scroll
byte clockDisplayType = 3; //0-Center Times, 1-24-hour Military Time, 2-12-hour Space-Padded, 3-Blinking Center Light
byte dateDisplayType = 5; //0-Zero-Padded (MMDD), 1-Space-Padded (MMDD), 2-Center Dates (1MDD), 3-Just Day of Week (Sun), 4-Just Numeric Day (DD), 5-With "." Separator (MM.DD), 6-Just Year (YYYY)
byte pastelColors = 0;
bool DSTime = 0;
long gmtOffset_sec = -28800;
String timeZone = "CET-1CEST,M3.5.0,M10.5.0/3";  // default: Central Europe with automatic DST

void applyTimeConfig() {
  if (timeZone.length() > 0) {
    configTzTime(timeZone.c_str(), ntpServer);   // DST handled automatically by the TZ rules
  } else {
    configTime(gmtOffset_sec, (daylightOffset_sec * DSTime), ntpServer);
  }
}
byte ClockColorSettings = 0;
byte DateColorSettings = 0;
int colonType = 0;
byte ColorChangeFrequency = 0;
byte brightness = 10;
String scrollText = "dAdS ArE tHE bESt";
bool colorchangeCD = 1;
int realtimeMode = 0;
int spotlightsColorSettings = 0;
bool useSpotlights = 1;
int scrollColorSettings = 0;
bool scrollOverride = 0;
bool scrollOptions1 = 0;   //Military Time (HHMM)
bool scrollOptions2 = 0;   //Day of Week (DOW)
bool scrollOptions3 = 0;   //Today's Date (DD-MM)
bool scrollOptions4 = 0;   //Year (YYYY)
bool scrollOptions7 = 0;   //Text Message
bool scrollOptions8 = 1;   //IP Address of Clock
int scrollFrequency = 1;
int lightshowMode = 0;
int suspendFrequency = 1;  //in minutes
byte suspendType = 0; //0-off, 1-digits-only, 2-everything
bool firstRun = 0;  //will slow down the startup for diagnosing boot issues
uint32_t countdownColorValue = 0x00FF04; 
uint32_t hourColorValue = 0xFF0000;  // default red
uint32_t colonColorValue = 0xFF0000;  // default red
uint32_t minColorValue = 0xFF0000;  // default red
uint32_t spotlightsColorValue = 0xE8EEBA;  
uint32_t scrollColorValue = 0xFFFFFF;  
uint32_t scoreboardColorLeftValue = 0x0091FF;  
uint32_t scoreboardColorRightValue = 0x00FF6E;  
uint32_t dayColorValue = 0xEE00FF; 
uint32_t monthColorValue = 0xEE00FF;  
uint32_t separatorColorValue = 0xEE00FF; 
CRGB lightchaseColorOne = CRGB::Blue;
CRGB lightchaseColorTwo = CRGB::Red;
CRGB alternateColor = CRGB::Black; 
CRGB oldsnakecolor = CRGB::Green;
CRGB spotcolor = CHSV(random(0, 255), 255, 255);
CRGB countdownColor = CRGB(countdownColorValue);
CRGB hourColor = CRGB(hourColorValue);
CRGB colonColor = CRGB(colonColorValue); 
CRGB minColor = CRGB(minColorValue);
CRGB tinyhourColor = CRGB(colonColorValue);
CRGB spotlightsColor = CRGB(spotlightsColorValue);
CRGB scrollColor = CRGB(scrollColorValue);
CRGB scoreboardColorLeft = CRGB(scoreboardColorLeftValue);
CRGB scoreboardColorRight = CRGB(scoreboardColorRightValue);
CRGB dayColor = CRGB(dayColorValue);
CRGB monthColor = CRGB(monthColorValue);
CRGB tinymonthColor = CRGB(monthColorValue);
CRGB separatorColor = CRGB(separatorColorValue);

// HA Display Mode – show a custom number sent from Home Assistant
float haDisplayValue = 0;
bool haDisplayDegree = true; // show ° symbol (index 26 in numbers[])

const uint16_t FAKE_LEDs[FAKE_NUM_LEDS] = {digit0, fdigit1, digit2, fdigit3, digit4, fdigit5, digit6};

//fake LED layout for spectrum (from the middle out)
const uint16_t FAKE_LEDs_C_BMUP[SEGMENTS_LEDS] = {seg(17), seg(11), seg(21), seg(12), seg(20), seg(19), seg(27), seg(7), seg(22), seg(10), seg(26), seg(16), seg(25), seg(13), seg(18), seg(1), seg(31), seg(2), seg(30), seg(9), seg(29), seg(15), seg(23), seg(14), seg(24), seg(0), seg(32), seg(6), seg(36), seg(3), seg(35), seg(8), seg(28), seg(5), seg(33), seg(4), seg(34)};
//fake LED layout for spectrum (bfrom the outside in)
const uint16_t FAKE_LEDs_C_CMOT[SEGMENTS_LEDS] = {seg(19), seg(26), seg(16), seg(20), seg(13), seg(25), seg(12), seg(17), seg(18), seg(21), seg(14), seg(24), seg(11), seg(29), seg(9), seg(22), seg(15), seg(23), seg(10), seg(28), seg(7), seg(27), seg(8), seg(36), seg(6), seg(30), seg(3), seg(35), seg(2), seg(34), seg(1), seg(31), seg(4), seg(32), seg(5), seg(33), seg(0)};
//fake LED layout for spectrum (bottom-left to right-top)
const uint16_t FAKE_LEDs_C_BLTR[SEGMENTS_LEDS] = {seg(32), seg(31), seg(27), seg(30), seg(36), seg(33), seg(34), seg(35), seg(29), seg(22), seg(21), seg(17), seg(20), seg(26), seg(23), seg(28), seg(24), seg(25), seg(19), seg(12), seg(11), seg(7), seg(10), seg(16), seg(13), seg(18), seg(14), seg(15), seg(9), seg(2), seg(1), seg(0), seg(6), seg(3), seg(8), seg(4), seg(5)};
//fake LED layout for spectrum (top-left to bottom-right) 
const uint16_t FAKE_LEDs_C_TLBR[SEGMENTS_LEDS] = {seg(34), seg(33), seg(32), seg(36), seg(35), seg(28), seg(24), seg(23), seg(29), seg(30), seg(31), seg(27), seg(22), seg(26), seg(25), seg(18), seg(14), seg(13), seg(19), seg(20), seg(21), seg(17), seg(12), seg(16), seg(15), seg(8), seg(4), seg(3), seg(9), seg(10), seg(11), seg(7), seg(2), seg(6), seg(5), seg(0), seg(1)};
//fake LED layout for spectrum (top-middle down)  
const uint16_t FAKE_LEDs_C_TMDN[SEGMENTS_LEDS] = {seg(18), seg(14), seg(24), seg(13), seg(25), seg(19), seg(8), seg(28), seg(15), seg(23), seg(16), seg(26), seg(12), seg(20), seg(17), seg(4), seg(34), seg(3), seg(35), seg(9), seg(29), seg(10), seg(22), seg(11), seg(21), seg(5), seg(33), seg(6), seg(36), seg(2), seg(30), seg(7), seg(27), seg(0), seg(32), seg(1), seg(31)};
//fake LED layout for spectrum (center-sides in)  
const uint16_t FAKE_LEDs_C_CSIN[SEGMENTS_LEDS] = {seg(5), seg(32), seg(0), seg(33), seg(6), seg(36), seg(4), seg(31), seg(1), seg(34), seg(3), seg(30), seg(2), seg(35), seg(9), seg(29), seg(8), seg(27), seg(7), seg(28), seg(15), seg(22), seg(10), seg(23), seg(16), seg(26), seg(14), seg(21), seg(11), seg(24), seg(13), seg(20), seg(12), seg(25), seg(19), seg(18), seg(17)};
//fake LED layout for spectrum (bottom-right to left-top)
const uint16_t FAKE_LEDs_C_BRTL[SEGMENTS_LEDS] = {seg(0), seg(1), seg(7), seg(2), seg(6), seg(5), seg(4), seg(3), seg(9), seg(10), seg(11), seg(17), seg(12), seg(16), seg(15), seg(8), seg(14), seg(13), seg(19), seg(20), seg(21), seg(27), seg(22), seg(26), seg(25), seg(18), seg(24), seg(23), seg(29), seg(30), seg(31), seg(32), seg(36), seg(35), seg(28), seg(34), seg(33)};
//fake LED layout for spectrum (top-right to bottom-left)
const uint16_t FAKE_LEDs_C_TRBL[SEGMENTS_LEDS] = {seg(4), seg(5), seg(0), seg(6), seg(3), seg(8), seg(14), seg(15), seg(9), seg(2), seg(1), seg(7), seg(10), seg(16), seg(13), seg(18), seg(24), seg(25), seg(19), seg(12), seg(11), seg(17), seg(20), seg(26), seg(23), seg(28), seg(34), seg(35), seg(29), seg(22), seg(21), seg(27), seg(30), seg(36), seg(33), seg(32), seg(31)};
//fake LED layout for spectrum (horizontal parts)   
const uint16_t FAKE_LEDs_C_VERT[SEGMENTS_LEDS] = {seg(17), seg(21), seg(11), seg(27), seg(7), seg(31), seg(1), seg(20), seg(12), seg(22), seg(10), seg(30), seg(2), seg(32), seg(0), seg(19), seg(26), seg(16), seg(29), seg(9), seg(36), seg(6), seg(25), seg(13), seg(23), seg(15), seg(35), seg(3), seg(33), seg(5), seg(18), seg(24),seg(14), seg(28), seg(8), seg(34), seg(4)};
const uint16_t FAKE_LEDs_C_VERT2[SEGMENTS_LEDS] = {seg(18), seg(14), seg(24), seg(8), seg(28), seg(4), seg(34), seg(13), seg(25), seg(15), seg(23), seg(3), seg(35), seg(5), seg(33), seg(19), seg(16), seg(26), seg(9), seg(29), seg(6), seg(36), seg(12), seg(20), seg(10), seg(22), seg(2), seg(30), seg(0), seg(32), seg(17), seg(11),seg(21), seg(7), seg(27), seg(1), seg(31)};
//fake LED layout for spectrum (vertical parts)  
const uint16_t FAKE_LEDs_C_OUTS[SEGMENTS_LEDS] = {seg(32), seg(33), seg(36), seg(31), seg(34), seg(30), seg(35), seg(29), seg(27), seg(28), seg(22), seg(23), seg(26), seg(21), seg(24), seg(20), seg(25), seg(19), seg(17), seg(18), seg(12), seg(13), seg(16), seg(11), seg(14), seg(10), seg(15), seg(9), seg(7), seg(8), seg(2), seg(3), seg(6), seg(1), seg(4), seg(0), seg(5)};
const uint16_t FAKE_LEDs_C_OUTS2[SEGMENTS_LEDS] = {seg(0), seg(5), seg(6), seg(1), seg(4), seg(2), seg(3), seg(9), seg(7), seg(8), seg(10), seg(15), seg(16), seg(11), seg(14), seg(12), seg(13), seg(19), seg(17), seg(18), seg(20), seg(25), seg(26), seg(21), seg(24), seg(22), seg(23), seg(29), seg(27), seg(28), seg(30), seg(35), seg(36), seg(31), seg(34), seg(32), seg(33)};

//fake LED layout for fire display  
const uint16_t FAKE_LEDs_C_FIRE[SEGMENTS_LEDS] = {seg(17), seg(11), seg(21), seg(12), seg(20), seg(19), seg(27), seg(7), seg(22), seg(10), seg(26), seg(16), seg(25), seg(13), seg(18), seg(1), seg(31), seg(2), seg(30), seg(9), seg(29), seg(15), seg(23), seg(14), seg(24), seg(0), seg(32), seg(6), seg(36), seg(3), seg(35), seg(8), seg(28), seg(5), seg(33), seg(4), seg(34)};
//fake LED layout for Rain display  
const uint16_t FAKE_LEDs_C_RAIN[SEGMENTS_LEDS] = {seg(30), seg(35), seg(1), seg(6), seg(4), seg(22), seg(23), seg(31), seg(36), seg(34), seg(12), seg(13), seg(7), seg(9), seg(8), seg(0), seg(5), seg(17), seg(19), seg(18), seg(10), seg(15), seg(21), seg(26), seg(24), seg(2), seg(3), seg(27), seg(29), seg(28), seg(20), seg(25), seg(11), seg(16), seg(14), seg(32), seg(33)};
//fake LED layout for Snake display 
const uint16_t FAKE_LEDs_SNAKE[SEGMENTS_LEDS] = {seg(0), seg(1), seg(7), seg(2), seg(6), seg(5), seg(4), seg(3), seg(9), seg(10), seg(11), seg(17), seg(12), seg(16), seg(15), seg(8), seg(14), seg(13), seg(19), seg(20), seg(21), seg(27), seg(22), seg(26), seg(25), seg(18), seg(24), seg(23), seg(29), seg(30), seg(31), seg(32), seg(36), seg(35), seg(28), seg(34), seg(33)};

//  real6     fake5     real4     fake3     real2     fake1     real0
// RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR 
//R       R F       F R       R F       F R       R F       F R       R
//R   S   R F   S   F R   S   R F   S   F R   S   R F   S   F R   S   R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
// RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR 
//R       R F       F R       R F       F R       R F       F R       R
//R   S   R F   S   F R   S   R F   S   F R   S   R F   S   F R   S   R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
//R       R F       F R       R F       F R       R F       F R       R
// RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR   RRRRRRR 

//digit wiring order
//  4444444 
// 3       5
// 3       5
// 3       5
// 3       5
// 3       5
// 3       5
// 3       5
//  6666666 
// 2       0
// 2       0
// 2       0
// 2       0
// 2       0
// 2       0
// 2       0
//  1111111 
// byte numbers digit order 0b6543210
byte numbers[] = { 
   0b0111111,   //  [0] 0
   0b0100001,   //  [1] 1
   0b1110110,   //  [2] 2
   0b1110011,   //  [3] 3
   0b1101001,   //  [4] 4
   0b1011011,   //  [5] 5
   0b1011111,   //  [6] 6
   0b0110001,   //  [7] 7
   0b1111111,   //  [8] 8
   0b1111011,   //  [9] 9
   0b0000000,   //  [10] all off, space
   0b0100001,   //  [11] !
   0b0101000,   //  [12] "
   0b1101111,   //  [13] #
   0b1011011,   //  [14] $
   0b1100100,   //  [15] %
   0b1100001,   //  [16] &
   0b0001000,   //  [17] '
   0b0011010,   //  [18] (
   0b0110010,   //  [19] )
   0b0011000,   //  [20] *
   0b1001100,   //  [21] +
   0b0000100,   //  [22] ,
   0b1000000,   //  [23] -
   0b0000010,   //  [24] .
   0b1100100,   //  [25] /
   0b1111000,   //  [26] degrees symbol
   0b0010010,   //  [27] :
   0b0010011,   //  [28] ;
   0b1011000,   //  [29] <
   0b1000010,   //  [30] =
   0b1110000,   //  [31] >
   0b1110100,   //  [32] ?
   0b1110111,   //  [33] @
   0b1111101,   //  [34] A
   0b1001111,   //  [35] B
   0b0011110,   //  [36] C(elsius)
   0b1100111,   //  [37] D
   0b1011110,   //  [38] E
   0b1011100,   //  [39] F(ahrenheit)
   0b0011111,   //  [40] G
   0b1101101,   //  [41] H(umidity)
   0b0001100,   //  [42] I
   0b0100111,   //  [43] J
   0b1011101,   //  [44] K
   0b0001110,   //  [45] L
   0b0010101,   //  [46] M
   0b0111101,   //  [47] N
   0b0111111,   //  [48] O
   0b1111100,   //  [49] P
   0b1111010,   //  [50] Q
   0b0111100,   //  [51] R
   0b1011011,   //  [52] S
   0b1001110,   //  [53] T
   0b0101111,   //  [54] U
   0b0101111,   //  [55] V
   0b0101010,   //  [56] W
   0b1101101,   //  [57] X
   0b1101011,   //  [58] Y
   0b1110110,   //  [59] Z
   0b0011110,   //  [60] left bracket
   0b1001001,   //  [61] backslash
   0b0110011,   //  [62] right bracket
   0b0111000,   //  [63] carrot
   0b0000010,   //  [64] underscore
   0b0100000,   //  [65] aposhtophe
   0b1110111,   //  [66] a
   0b1001111,   //  [67] b
   0b1000110,   //  [68] c
   0b1100111,   //  [69] d
   0b1111110,   //  [70] e
   0b1011100,   //  [71] f
   0b1111011,   //  [72] g
   0b1001101,   //  [73] h
   0b0000100,   //  [74] i
   0b0000011,   //  [75] j
   0b1011101,   //  [76] k
   0b0001100,   //  [77] l
   0b0000101,   //  [78] m
   0b1000101,   //  [79] n
   0b1000111,   //  [80] o
   0b1111100,   //  [81] p
   0b1111001,   //  [82] q
   0b1000100,   //  [83] r
   0b1011011,   //  [84] s
   0b1001110,   //  [85] t
   0b0000111,   //  [86] u
   0b0000111,   //  [87] v
   0b0000101,   //  [88] w
   0b1101101,   //  [89] x
   0b1101011,   //  [90] y
   0b1110110,   //  [91] z
   0b1100001,   //  [92] left squigly bracket
   0b0001100,   //  [93] pipe
   0b1001100,   //  [94] right squigly bracket
   0b0010000,   //  [95] tilde
   0b1010010    //  [96] 3-lines (special pad)
   };

 

void setup() {
  
  Serial.begin(112500);
  lastResetReason = resetReasonToString(esp_reset_reason());
  webLog(String("Boot - reset reason: ") + lastResetReason);

  // Boot-loop guard: count crash reboots since the last stable run (loop()
  // clears this after 20s uptime). Too many quick crash-reboots in a row ->
  // force a safe display mode for this boot instead of restoring whatever
  // state (e.g. a Lightshow mode) may have triggered the crash.
  if (bootLoopMagic != BOOTLOOP_MAGIC) {
    bootLoopMagic = BOOTLOOP_MAGIC;
    bootLoopCount = 0;
  }
  bootLoopCount++;
  if (bootLoopCount >= 4) {
    bootLoopSafeMode = true;
    bootLoopCount = 0;
    webLog("Boot loop detected - forcing safe Clock mode for this boot");
  }

  setupPrefs();   // Initialize Preferences with "ShelfClock" namespace
  setupHA();      // Home Assistant Integration
  // Note: preferences.begin() is already called in setupPrefs() - do NOT call it again!
  
  //setup LEDs
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(LEDs,NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(255);
  fill_solid(LEDs, NUM_LEDS, CRGB::Black);
  FastLED.show();
  if (firstRun) {
  allTest();
  }
  fakeClock(2);  // blink 12:00 like old clocks once did


  //display "LoAd" while activating the websever
   allBlank();
  displayNumber(45,6,CRGB::Red);  // L
  displayNumber(80,4,CRGB::Red);  // o
  displayNumber(34,2,CRGB::Red); // A
  displayNumber(69,0,CRGB::Red);  // d
  FastLED.show();
  if (firstRun) {delay(500);}
  loadWebPageHandlers();  //load about 900 webpage handlers from the bottom of this sketch

  // Initialize FileFS 
  //display "file" while activating the filesystem
  allBlank();
  displayNumber(39,6,CRGB::Purple);  // F
  displayNumber(42,4,CRGB::Purple);  // I
  displayNumber(45,2,CRGB::Purple); // L
  displayNumber(38,0,CRGB::Purple);  // E
  FastLED.show();
  if (firstRun) {delay(500);}
  webLog("Initializing FileSystem...");
  if (!FileFS.begin()){
      webLog("FileFS Mount FAILED! Trying to format...");
      // Try to format the filesystem on first boot
      if (FileFS.format()) {
          webLog("FileFS formatted successfully. Mounting...");
          if (FileFS.begin()) {
              webLog("FileFS mounted after format.");
          } else {
              webLog("ERROR: FileFS mount failed even after format");
              //display "Err" if error
              allBlank();
              displayNumber(38,6,CRGB::Purple);  // E
              displayNumber(83,4,CRGB::Purple);  // r
              displayNumber(83,2,CRGB::Purple); // r
              FastLED.show();
              delay(500);
          }
      } else {
          Serial.println(F("!FileFS format failed"));
          //display "Err" if error
          allBlank();
          displayNumber(38,6,CRGB::Purple);  // E
          displayNumber(83,4,CRGB::Purple);  // r
          displayNumber(83,2,CRGB::Purple); // r
          FastLED.show();
          delay(500);
      }
  } else {
      Serial.println(F("FileFS mounted correctly."));
      Serial.println("Total Bytes");
      Serial.println(FileFS.totalBytes());
      Serial.println("Used Bytes");
      Serial.println(FileFS.usedBytes());
      listDir(FileFS, "/", 0);
      listDir(FileFS, "/css/", 0);
      listDir(FileFS, "/js/", 0);
  }

  //display "SEtS"
  allBlank();
  displayNumber(52,6,CRGB::Red);  // S
  displayNumber(38,4,CRGB::Red);  // E
  displayNumber(85,2,CRGB::Red); // t
  displayNumber(52,0,CRGB::Red);  // S
  FastLED.show();
  if (firstRun) {delay(500);}

  // Ensure /settings/ directory exists
  if (!FileFS.exists("/settings")) {
    Serial.println("Creating /settings/ directory...");
    if (FileFS.mkdir("/settings")) {
      Serial.println("/settings/ directory created");
    } else {
      Serial.println("Failed to create /settings/ directory");
    }
  }

  Serial.println("list setting folder files");
  listDir(FileFS, "/settings/", 0);
  //load settings from nvram and flash
  Serial.println("load all settings from json file");
  getclockSettings("generic"); //load all settings from json file

  if (bootLoopSafeMode) {
    // Override whatever mode was restored - Clock mode has no blocking
    // effects and can't be the cause of a crash loop. Persist it so the
    // guard doesn't just get re-triggered by the same saved state.
    clockMode = 0;
    updateSettingsRequired = 1;
  }

  // If settings file doesn't exist (first boot), save default values
  if (!FileFS.exists("/settings/clockSettings-generic.json")) {
    Serial.println("No settings file found - saving defaults");
    updateSettingsRequired = 1;
    // The auto-save timer will save after 3 seconds in loop()
  }



  
  //display "no AP" while activating the wifi

  allBlank();
  displayNumber(79,6,CRGB::Red);  // n
  displayNumber(80,4,CRGB::Red);  // o
  displayNumber(34,2,CRGB::Red); // A
  displayNumber(49,0,CRGB::Red);  // P
  FastLED.show();
  Config.autoReconnect = true; // Enable auto-reconnect. 
  Config.portalTimeout = 20000; // Sets timeout value for the captive portal 
  Config.retainPortal = true; // Retains the portal function after timed-out 
  Config.autoRise = true; // False for disabling the captive portal
  Config.reconnectInterval = 6;
  Portal.config(Config);      

  //display "Conn" while connecting to WiFi
  allBlank();
  displayNumber(36,6,CRGB::Cyan);  // C
  displayNumber(80,4,CRGB::Cyan);  // o
  displayNumber(79,2,CRGB::Cyan);  // n
  displayNumber(79,0,CRGB::Cyan);  // n
  // Light spotlights blue as connecting indicator
  for (int i = SEGMENTS_LEDS; i < NUM_LEDS; i++) {
    LEDs[i] = CRGB::Blue;
  }
  FastLED.show();
  if (firstRun) {delay(500);}
  Serial.println("Wifi Starting");
  WiFi.hostname(host); //set hostname
  WiFi.macAddress(mac);
  // setup AutoConnect to control WiFi
  if (Portal.begin()) {
    Serial.println("WiFi Connected: " + WiFi.localIP().toString());
    WiFi_startTime = millis();
    WiFi_retryCount = 0;
    allBlank();  //clear "no AP" from screen once wifi is online
   }  else {
    Serial.println("Wifi Failed");
    //display "Err" while activating the wifi
    allBlank();
    displayNumber(38,6,CRGB::Red);  // E
    displayNumber(83,4,CRGB::Red);  // r
    displayNumber(83,2,CRGB::Red); // r
    FastLED.show();
    delay(1000);
    }
    // Load MQTT settings and initialize both MQTT systems
    setupMQTT();  // Loads MQTT settings from Preferences into mqttServer, mqttPort, mqttUser, mqttPassword

    // "Visit device" link on the HA device page -> opens the clock's web UI
    static char haConfigUrl[40];
    if (WiFi.status() == WL_CONNECTED) {
        snprintf(haConfigUrl, sizeof(haConfigUrl), "http://%s", WiFi.localIP().toString().c_str());
        device.setConfigurationUrl(haConfigUrl);
    }

    // Initialize Home Assistant MQTT with loaded credentials
    if (mqttServer.length() > 0) {
        Serial.println("Initializing Home Assistant MQTT...");
        haMqtt.begin(mqttServer.c_str(), mqttPort, mqttUser.c_str(), mqttPassword.c_str());
    } else {
        Serial.println("MQTT not configured - HA integration disabled");
    }

    

  //display "dnS" while activating the mdns
  allBlank();
  displayNumber(69,6,CRGB::Orange);  // d
  displayNumber(79,4,CRGB::Orange);  // n
  displayNumber(52,2,CRGB::Orange); // S
  FastLED.show();
  if (firstRun) {delay(500);}
  //use mdns for host name resolution
  if (!MDNS.begin(host)) { //http://shelfclock
    webLog("Error setting up MDNS responder - continuing without mDNS");
    allBlank();
    displayNumber(38,6,CRGB::Orange);  // E
    displayNumber(83,4,CRGB::Orange);  // r
    displayNumber(83,2,CRGB::Orange); // r
    FastLED.show();
    delay(2000);
  } else {
    Serial.println("mDNS responder started");
  }

  //init and set the time of the internal RTC from NTP server
  //display "ntP" while activating the ntp
  allBlank();
  displayNumber(79,6,CRGB::Blue);  // n
  displayNumber(85,4,CRGB::Blue);  // t
  displayNumber(49,2,CRGB::Blue);  // P
  FastLED.show();
  if (firstRun) {delay(500);}
  Serial.println("set the time of the internal RTC from NTP server");
  applyTimeConfig();
    if(!getLocalTime(&timeinfo)){ 
      allBlank();
      displayNumber(38,6,CRGB::Blue);  // E
      displayNumber(83,4,CRGB::Blue);  // r
      displayNumber(83,2,CRGB::Blue); // r
      FastLED.show();
      delay(1000);
      timeinfo.tm_hour = 0;
      timeinfo.tm_min = 0;
      timeinfo.tm_sec = 0;
      timeinfo.tm_mday = 18;
      timeinfo.tm_mon = 4;
      timeinfo.tm_year = 125;
      time_t t = mktime(&timeinfo);
      struct timeval now = { .tv_sec = t};
      settimeofday(&now, NULL);
    }
    
    


  Serial.println("print time");
  printLocalTime(); 

  //create something to know if now is not then
  previousTimeMin = timeinfo.tm_min;
  previousTimeHour = timeinfo.tm_hour;
  previousTimeDay = timeinfo.tm_mday;
  previousTimeWeek = ((timeinfo.tm_yday + 7 - (timeinfo.tm_wday ? (timeinfo.tm_wday - 1) : 6)) / 7);
  previousTimeMonth = timeinfo.tm_mon;
  
  // I assume this starts the OTA stuff
  //display "OtA" 
  allBlank();
  displayNumber(48,6,CRGB::Red);  // O
  displayNumber(85,4,CRGB::Red);  // t
  displayNumber(34,2,CRGB::Red); // A
  FastLED.show();
  if (firstRun) {delay(500);}
  httpUpdateServer.setup(&server);

  initGreenMatrix();   //setup lightshow functions
  raininit();          //setup lightshow functions
    
  //display "SErv" while activating the websever
   allBlank();
  displayNumber(52,6,CRGB::Red);  // S
  displayNumber(38,4,CRGB::Red);  // E
  displayNumber(83,2,CRGB::Red); // r
  displayNumber(87,0,CRGB::Red);  // v
  FastLED.show();
   server.enableCrossOrigin(true);
  server.enableCORS(true);

  //Webpage Handlers for FileFS access to flash
  server.serveStatic("/", FileFS, "/index.html");  //send default webpage from root request
  server.serveStatic("/", FileFS, "/", "max-age=86400");
  // server.serveStatic("/", FileFS, "/", "no-cache, max-age=0"); //for debugging

  //OTA firmware Upgrade Webpage Handlers
  Serial.println("OTA Available");



  
  //display "Schd" while loading the schedules from flash
  allBlank();
  displayNumber(52,6,CRGB::Red);  // S
  displayNumber(68,4,CRGB::Red);  // c
  displayNumber(73,2,CRGB::Red); // h
  displayNumber(69,0,CRGB::Red);  // d
  FastLED.show();
  if (firstRun) {delay(500);}

  // Ensure /scheduler/ directory exists
  if (!FileFS.exists("/scheduler")) {
    Serial.println("Creating /scheduler/ directory...");
    if (FileFS.mkdir("/scheduler")) {
      Serial.println("/scheduler/ directory created");
    } else {
      Serial.println("Failed to create /scheduler/ directory");
    }
  }

  listDir(FileFS, "/scheduler/", 0);
  Serial.println("load array of schedule files on drive ");
  createSchedulesArray();  //load array of schedule files on drive
  Serial.println("print out schedule array");
  processSchedules(0);  //print out schedule array

  //display "donE" after setup
  allBlank();
  displayNumber(69,6,CRGB::Red);  // d
  displayNumber(80,4,CRGB::Red);  // o
  displayNumber(79,2,CRGB::Red); // n
  displayNumber(38,0,CRGB::Red);  // E
  FastLED.show();
  applyBrightness();
  if (firstRun) {delay(500);}
  allBlank();

  //display IP adresss 2x after setup
  if (firstRun) {
  char processedTextIP[16] = {0};
  if (WiFi.status() == WL_CONNECTED) {
      snprintf(processedTextIP, sizeof(processedTextIP), "%s", WiFi.localIP().toString().c_str());
  } else {
      snprintf(processedTextIP, sizeof(processedTextIP), "no AP, no IP");
  }
  scroll(processedTextIP);
  scroll(processedTextIP);
}
  allBlank();
  Serial.println("end of Setup");
}    //end of Setup()

void loop(){

  server.handleClient();
  Portal.handleRequest();
  if (otaInProgress) {
    // An OTA upload is running: keep hands off LEDs, LittleFS and MQTT so
    // the flash write is not disturbed (FastLED.show() blocks interrupts)
    return;
  }
  scrollTick();  // advance a running scroll (non-blocking, one column per step)

  // Boot-loop guard: 20s without a crash counts as a stable boot
  static bool bootMarkedStable = false;
  if (!bootMarkedStable && millis() > 20000) {
    bootLoopCount = 0;
    bootMarkedStable = true;
  }

  // GitHub update check: first 60s after boot, then every 24h. Runs on its
  // own task (see checkForUpdateAsync()) so it can never block rendering.
  #define UPDATE_CHECK_INTERVAL_MS (24UL * 60 * 60 * 1000)
  if (millis() > 60000 && (millis() - lastUpdateCheckMs) > UPDATE_CHECK_INTERVAL_MS) {
    checkForUpdateAsync();
  }
  if (WiFi.status() != WL_CONNECTED) {
    WiFi_elapsedTime = millis() - WiFi_startTime;
    if (WiFi_elapsedTime >= WiFi_MAX_RETRY_DURATION) {
      // Reboot if maximum retry duration exceeded
        Serial.println("maximum retry duration exceeded ");
      ESP.restart();
    } else {
      if (WiFi_retryCount >= WiFi_MAX_RETRIES) {
        // Reboot if maximum retry attempts exceeded
        Serial.println("maximum retry attemps exceeded ");
        ESP.restart();
      } else if (WiFi.status() == WL_IDLE_STATUS) {
        // Attempt to reconnect to Wi-Fi
        WiFi.disconnect();
        delay(1000);
        WiFi.begin();
        WiFi_retryCount++;
        WiFi_totalReconnections++;
        Serial.print("Retry ");
        Serial.print(WiFi_retryCount);
        Serial.println(" - Reconnecting...");
      }
    }
  } else {
    // Wi-Fi connection is successful
    //Serial.println("Connected!");
    // Reset retry count and start time
    WiFi_retryCount = 0;
    WiFi_startTime = millis();
     //   Serial.println("Wi-Fi connection is successful ");
    // Your other code logic can go here
  }
  haMqtt.loop();
  haSyncState();

  // Handle MQTT connection and messages
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastMqttReconnectAttempt > 5000) {  // Try reconnecting every 5 seconds
      lastMqttReconnectAttempt = now;
      mqttReconnect();
    }
  } else {
    mqttClient.loop();  // Process MQTT messages
  }

  // Scroll queued notifications (from MQTT shelfclock/notify or HTTP /notify)
  // - waits until a possibly running scroll has finished
  if (notifyPending && !scrollActive) {
    notifyPending = false;
    startScrollColored(notifyText, CRGB(notifyColorValue), notifyRepeat);
  }

  // Auto-save settings after 3 seconds of last change to prevent data loss
  if (updateSettingsRequired == 1) {
    unsigned long now = millis();
    if (settingsSaveTimer == 0) {
      settingsSaveTimer = now;
    } else if ((now - settingsSaveTimer) >= 3000) {
      saveclockSettings("generic");
      updateSettingsRequired = 0;
      settingsSaveTimer = 0;
      webLog("Settings saved. Heap: " + String(ESP.getFreeHeap()));
    }
  } else {
    settingsSaveTimer = 0;
  }
  //Change Frequency so as to not use hard-coded delays
  unsigned long currentMillis = millis();  
  //run everything inside here every second
  if (((unsigned long)(currentMillis - prevTime) >= 500) || ((unsigned long)(currentMillis - prevTime) <= 0)) {  //deals with millis() having a rollover period after approximately 49.7 days
    prevTime = currentMillis;
    if(!getLocalTime(&timeinfo)){ 
      Serial.println("Failed to obtain time");
    }

    //setup time-passage trackers
    int secs = timeinfo.tm_sec;
    int currentTimeMin = timeinfo.tm_min;
    byte m1 = currentTimeMin / 10;
    byte m2 = currentTimeMin % 10;
    int currentTimeHour = timeinfo.tm_hour;
    int currentTimeDay = timeinfo.tm_mday;
    int currentTimeWeek = ((timeinfo.tm_yday + 7 - (timeinfo.tm_wday ? (timeinfo.tm_wday - 1) : 6)) / 7);
    int currentTimeMonth = timeinfo.tm_mon;

    checkSleepTimer();  //time to sleep?
    // MQTT handled by ArduinoHA
    
    //if ((currentTimeHour == 23 && currentTimeMin == 11 && timeinfo.tm_sec == 0 && clockDisplayType != 1) || (currentTimeHour == 11 && currentTimeMin == 11 && timeinfo.tm_sec == 0)) { scroll("MAkE A WISH"); }  //at 1111 make a wish
 
    if (abs(currentTimeMin - previousTimeMin) >= 1) { //run every minute
      previousTimeMin = currentTimeMin; 
      randomMinPassed = 1;
      minutesUptime += 1; 
      applyBrightness();
      processSchedules(0);  //see if there's a timer
      if (scrollFrequency == 1 && (suspendType == 0 || isAsleep == 0) && scrollOverride == 1 && ((clockMode != 11) && (clockMode != 1) && (clockMode != 4))) {displayScrollMode();}
      } //end of run every minute

    if ((m2 == 0 || m2 == 5) && (secs == 0)) { //run every 5 minutes
      if (scrollFrequency == 2 && (suspendType == 0 || isAsleep == 0) && scrollOverride == 1 && ((clockMode != 11) && (clockMode != 1) && (clockMode != 4))) {displayScrollMode();}
      } //end of run every 5 minutes

    if ((m2 == 0) && (secs == 0)) { //run every 10 minutes
      if (scrollFrequency == 3 && (suspendType == 0 || isAsleep == 0) && scrollOverride == 1 && ((clockMode != 11) && (clockMode != 1) && (clockMode != 4))) {displayScrollMode();}
      } //end of run every 10 minutes

    if (((m1 == 0 && m2 == 0) || (m1 == 1 && m2 == 5) || (m1 == 3 && m2 == 0) || (m1 == 4 && m2 == 5)) && (secs == 0)) { //run every 15 minutes
      if (scrollFrequency == 4 && (suspendType == 0 || isAsleep == 0) && scrollOverride == 1 && ((clockMode != 11) && (clockMode != 1) && (clockMode != 4))) {displayScrollMode();}
      } //end of run every 15 minutes

    if (((m1 == 0 && m2 == 0) || (m1 == 3 && m2 == 0)) && (secs == 0)) { //run every 30 minutes
      if (scrollFrequency == 5 && (suspendType == 0 || isAsleep == 0) && scrollOverride == 1 && ((clockMode != 11) && (clockMode != 1) && (clockMode != 4))) {displayScrollMode();}
      } //end of run every 30 minutes
      
    if (abs(currentTimeHour - previousTimeHour) >= 1) { //run every hour
      previousTimeHour = currentTimeHour;
      randomHourPassed = 1;
      hoursUptime += 1;
      if (scrollFrequency == 6 && (suspendType == 0 || isAsleep == 0) && scrollOverride == 1 && ((clockMode != 11) && (clockMode != 1) && (clockMode != 4))) {displayScrollMode();}
      } //end of run every hour
      
    if (abs(currentTimeDay - previousTimeDay) >= 1) { 
        previousTimeDay = currentTimeDay;
        randomDayPassed = 1;
        applyTimeConfig();  //daily NTP resync
        daysUptime = daysUptime + 1;
     }
    
    if (abs(currentTimeWeek - previousTimeWeek) >= 1) { previousTimeWeek = currentTimeWeek; randomWeekPassed = 1;}
    if (abs(currentTimeMonth - previousTimeMonth) >= 1) { previousTimeMonth = currentTimeMonth; randomMonthPassed = 1;}

    if (realtimeMode == 0) {    //give autodim sensors some CPU time, update display
       //applyBrightness();        
       //FastLED.show();
    }
    //give the various clock modes CPU time every 1 seconds
    //(skipped while a scroll is running - scrollTick() owns the display then)
    if (!scrollActive) {
    if ((suspendType == 0 || isAsleep == 0) && clockMode == 0) {
       displayTimeMode();
    }  else if (clockMode == 1) {
       displayCountdownMode();
    }  else if ((suspendType == 0 || isAsleep == 0) && clockMode == 2) {
       displayPartyMode();
    }  else if ((suspendType == 0 || isAsleep == 0) && clockMode == 3) {
       displayScoreboardMode();  
    }  else if (clockMode == 4) {
       displayStopwatchMode();           
    }  else if ((suspendType == 0 || isAsleep == 0) && clockMode == 5) {
       displayLightshowMode();
    }  else if ((suspendType == 0 || isAsleep == 0) && clockMode == 8) {
       displayHAValueMode();
    }  else if ((suspendType == 0 || isAsleep == 0) && clockMode == 7) {
       displayDateMode();         
    }  else if (clockMode == 10) {
      //display off
    }  else if ((suspendType == 0 || isAsleep == 0) && clockMode == 11) {
       displayScrollMode();
    }
    }  //end of !scrollActive
    ShelfDownLights();
    randomMinPassed = 0; 
    randomHourPassed = 0; 
    randomDayPassed = 0; 
    randomWeekPassed = 0; 
    randomMonthPassed = 0;
    
    applyBrightness();
  }

  displayRealtimeMode();  //always run outside time loop for speed, but only really show when it's needed
}  // end of main loop
