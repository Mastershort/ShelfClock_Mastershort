#include "../include/globals.h"
#include "../include/ShelfClock.h"
#include "../include/web_handlers.h"
#include "../include/display_modes.h"
#include "../include/settings_manager.h"
#include "../include/lightshow.h"
#include "../include/ha_integration.h"
#include "../include/party_games.h"
#include "../include/mqtt_integration.h"
#include "../include/prefs.h"
#include <Update.h>
#include <HTTPUpdateServer.h>

// Placeholder sent to the browser instead of the real MQTT password;
// receiving it back unchanged means "keep the stored password"
static const char* MQTT_PASSWORD_MASK = "********";

void loadWebPageHandlers() {
  server.on("/goClockMode", HTTP_POST, []() {
    allBlank();
    clockMode = 0;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    printLocalTime();
    breakOutSet = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goDateMode", HTTP_POST, []() {
    allBlank();
    clockMode = 7;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goScrollingMode", HTTP_POST, []() {
    allBlank();
    clockMode = 11;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goDisplayOffMode", HTTP_POST, []() {
    allBlank();
    clockMode = 10;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    breakOutSet = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goLightshowMode", HTTP_POST, []() {
    if (server.hasArg("lightshowMode")) {
      lightshowMode = server.arg("lightshowMode").toInt();
    }
    allBlank();
    oldsnakecolor = CRGB::Green;
    getSlower = 180;
    clockMode = 5;
    realtimeMode = 1;
    updateSettingsRequired = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goCountdownMode", HTTP_POST, []() {
    if (server.hasArg("ms")) {
      countdownMilliSeconds = server.arg("ms").toInt();
    }
    if (countdownMilliSeconds < 1000) {countdownMilliSeconds = 1000;}
    if (countdownMilliSeconds > 86400000) {countdownMilliSeconds = 86400000;}
    endCountDownMillis = millis() + countdownMilliSeconds;
    if (currentMode == 0) {currentMode = clockMode; currentReal = realtimeMode;}
    allBlank();
    clockMode = 1;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goStopwatchMode", HTTP_POST, []() {
    if (server.hasArg("ms")) {
      countdownMilliSeconds = server.arg("ms").toInt();
    }
    if (countdownMilliSeconds < 1000) {countdownMilliSeconds = 1000;}
    if (countdownMilliSeconds > 86400000) {countdownMilliSeconds = 86400000;}
    CountUpMillis = millis();
    endCountDownMillis = millis() + countdownMilliSeconds;
    if (currentMode == 0) {currentMode = clockMode; currentReal = realtimeMode;}
    allBlank();
    clockMode = 4;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/goScoreboardMode", HTTP_POST, []() {
    if (server.hasArg("left")) {
      scoreboardLeft = server.arg("left").toInt();
    }
    if (server.hasArg("right")) {
      scoreboardRight = server.arg("right").toInt();
    }
    if (scoreboardLeft < 0) {scoreboardLeft = 0;}
    if (scoreboardLeft > 99) {scoreboardLeft = 99;}
    if (scoreboardRight < 0) {scoreboardRight = 0;}
    if (scoreboardRight > 99) {scoreboardRight = 99;}
    allBlank();
    clockMode = 3;
    realtimeMode = 0;
    updateSettingsRequired = 1;
    server.send(200, "text/plain", "OK");
  });

  server.on("/gethome", []() {
    DynamicJsonDocument json(1500);
    String output;
    json["scoreboardLeft"] = scoreboardLeft;
    json["scoreboardRight"] = scoreboardRight;
    serializeJson(json, output);
    server.send(200, "application/json", output);
  });

  server.on("/getsettings", []() {
    DynamicJsonDocument json(8192);
    String output;
    json["pastelColors"] = pastelColors;
    json["ColorChangeFrequency"] = ColorChangeFrequency;
    json["rangeBrightness"] = brightness;
    json["suspendType"] = suspendType;
    json["suspendFrequency"] = suspendFrequency;
    json["useSpotlights"] = useSpotlights;
    json["spotlightsColor"] = spotlightsColorValue;
    json["spotlightsColorSettings"] = spotlightsColorSettings;
    json["ClockDisplayType"] = clockDisplayType;
    json["colonType"] = colonType;
    json["gmtOffset_sec"] = gmtOffset_sec;
    json["DSTime"] = DSTime;
    json["timeZone"] = timeZone;
    json["ClockColorSettings"] = ClockColorSettings;
    json["hourColor"] = prefGetU32("hourColor", 0xFF0000);
    json["minColor"] = minColorValue;
    json["colonColor"] = colonColorValue;
    json["dateDisplayType"] = dateDisplayType;
    json["DateColorSettings"] = DateColorSettings;
    json["dayColor"] = dayColorValue;
    json["monthColor"] = monthColorValue;
    json["separatorColor"] = separatorColorValue;
    json["colorchangeCD"] = colorchangeCD;
    json["countdownColor"] = countdownColorValue;
    json["scoreboardColorLeft"] = scoreboardColorLeftValue;
    json["scoreboardColorRight"] = scoreboardColorRightValue;
    json["scrollFrequency"] = scrollFrequency;
    json["scrollOptions1"] = scrollOptions1;
    json["scrollOptions2"] = scrollOptions2;
    json["scrollOptions3"] = scrollOptions3;
    json["scrollOptions4"] = scrollOptions4;
    json["scrollOptions7"] = scrollOptions7;
    json["scrollOptions8"] = scrollOptions8;
    json["scrollText"] = scrollText;
    json["scrollOverride"] = scrollOverride;
    json["scrollColor"] = scrollColorValue;
    json["scrollColorSettings"] = scrollColorSettings;
    serializeJson(json, output);
    server.send(200, "application/json", output);
  });

  server.on("/updateanything", HTTP_POST, []() {
    DynamicJsonDocument json(1500);
    String body = server.arg("plain");
    if (body.length() == 0 && server.args() > 0) {
      body = server.arg(0);
    }
    if (body.length() > 0) {
      Serial.println("----");
      Serial.println(body);
      Serial.println("----");
      strncpy(lastUpdateBody, body.c_str(), sizeof(lastUpdateBody) - 1);
      lastUpdateBody[sizeof(lastUpdateBody) - 1] = '\0';
      lastUpdateAt = millis();
      DeserializationError err = deserializeJson(json, body);
      if (err) {
        Serial.print("JSON parse error: ");
        Serial.println(err.c_str());
        server.send(400, "text/json", "{\"result\":\"error\",\"reason\":\"json\"}");
        return;
      }

      if (!json["pastelColors"].isNull()){
        pastelColors = (int)json["pastelColors"];
        updateSettingsRequired = 1;
        allBlank();
      }

      if (!json["suspendType"].isNull()) {
        suspendType = (int)json["suspendType"];
        updateSettingsRequired = 1;
      }

      if (!json["spotlightsColorSettings"].isNull()) {
        spotlightsColorSettings = (int)json["spotlightsColorSettings"];
        updateSettingsRequired = 1;
        prevTime2 = 0;
        ShelfDownLights();
      }

      if (!json["ClockDisplayType"].isNull()) {
        clockDisplayType = (int)json["ClockDisplayType"];
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
      }

      if (!json["colonType"].isNull()) {
        colonType = (int)json["colonType"];
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
      }

      if (!json["ClockColorSettings"].isNull()) {
        ClockColorSettings = (int)json["ClockColorSettings"];
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
      }

      if (!json["dateDisplayType"].isNull()) {
        dateDisplayType = (int)json["dateDisplayType"];
        updateSettingsRequired = 1;
        if (clockMode == 7) { allBlank(); }
      }

      if (!json["DateColorSettings"].isNull()) {
        DateColorSettings = (int)json["DateColorSettings"];
        updateSettingsRequired = 1;
        if (clockMode == 7) { allBlank(); }
      }

      if (!json["scrollColorSettings"].isNull()) {
        scrollColorSettings = (int)json["scrollColorSettings"];
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["ColorChangeFrequency"].isNull()) {
        ColorChangeFrequency = (int)json["ColorChangeFrequency"];
        updateSettingsRequired = 1;
        randomMinPassed = 1;
        randomHourPassed = 1;
        randomDayPassed = 1;
        randomWeekPassed = 1;
        randomMonthPassed = 1;
        allBlank();
      }

      if (!json["suspendFrequency"].isNull()) {
        suspendFrequency = (int)json["suspendFrequency"];
        updateSettingsRequired = 1;
      }

      if (!json["timeZone"].isNull()) {
        timeZone = json["timeZone"].as<String>();
        applyTimeConfig();
        updateSettingsRequired = 1;
      }
      if (!json["gmtOffset_sec"].isNull()) {
        gmtOffset_sec = (long)json["gmtOffset_sec"];
        timeZone = "";  // picking a manual offset switches off the automatic timezone
        applyTimeConfig();
        if(!getLocalTime(&timeinfo)){Serial.println("Error, no NTP Server found!");}
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
        printLocalTime();
      }

      if (!json["scrollFrequency"].isNull()) {
        scrollFrequency = (int)json["scrollFrequency"];
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["rangeBrightness"].isNull()) {
        brightness = (int)json["rangeBrightness"];
        updateSettingsRequired = 1;
        FastLED.setBrightness(brightness);
        FastLED.show();
        prevTime2 = 0;
      }

      if (!json["spotlightsColor"].isNull()) {
        spotlightsColorValue = json["spotlightsColor"].as<uint32_t>();
        spotlightsColor      = CRGB(spotlightsColorValue);
        updateSettingsRequired = 1;
        prevTime2 = 0;
        ShelfDownLights();
      }

      if (!json["hourColor"].isNull()) {
        hourColorValue = json["hourColor"].as<uint32_t>();
        hourColor      = CRGB(hourColorValue);
        prefPutU32("hourColor", hourColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
      }

      if (!json["minColor"].isNull()) {
        minColorValue = json["minColor"].as<uint32_t>();
        minColor      = CRGB(minColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
      }

      if (!json["colonColor"].isNull()) {
        colonColorValue = json["colonColor"].as<uint32_t>();
        colonColor      = CRGB(colonColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
      }

      if (!json["dayColor"].isNull()) {
        dayColorValue = json["dayColor"].as<uint32_t>();
        dayColor      = CRGB(dayColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 7) { allBlank(); }
      }

      if (!json["monthColor"].isNull()) {
        monthColorValue = json["monthColor"].as<uint32_t>();
        monthColor      = CRGB(monthColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 7) { allBlank(); }
      }

      if (!json["separatorColor"].isNull()) {
        separatorColorValue = json["separatorColor"].as<uint32_t>();
        separatorColor      = CRGB(separatorColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 7) { allBlank(); }
      }

      if (!json["countdownColor"].isNull()) {
        countdownColorValue = json["countdownColor"].as<uint32_t>();
        countdownColor      = CRGB(countdownColorValue);
        updateSettingsRequired = 1;
        if ((clockMode == 1) || (clockMode == 4)) { allBlank(); }
      }

      if (!json["scoreboardColorLeft"].isNull()) {
        scoreboardColorLeftValue = json["scoreboardColorLeft"].as<uint32_t>();
        scoreboardColorLeft      = CRGB(scoreboardColorLeftValue);
        updateSettingsRequired = 1;
        if (clockMode == 3) { allBlank(); }
      }

      if (!json["scoreboardColorRight"].isNull()) {
        scoreboardColorRightValue = json["scoreboardColorRight"].as<uint32_t>();
        scoreboardColorRight      = CRGB(scoreboardColorRightValue);
        updateSettingsRequired = 1;
        if (clockMode == 3) { allBlank(); }
      }

      if (!json["scrollColor"].isNull()) {
        scrollColorValue = json["scrollColor"].as<uint32_t>();
        scrollColor      = CRGB(scrollColorValue);
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["useSpotlights"].isNull()) {
        if ( json["useSpotlights"] == true) {useSpotlights = 1;}
        if ( json["useSpotlights"] == false) {useSpotlights = 0;}
        updateSettingsRequired = 1;
        prevTime2 = 0;
        ShelfDownLights();
      }

      if (!json["DSTime"].isNull()) {
        if ( json["DSTime"] == true) {DSTime = 1;}
        if ( json["DSTime"] == false) {DSTime = 0;}
        applyTimeConfig();
        if(!getLocalTime(&timeinfo)){Serial.println("Error, no NTP Server found!");}
        updateSettingsRequired = 1;
        if (clockMode == 0) { allBlank(); }
        printLocalTime();
      }

      if (!json["colorchangeCD"].isNull()) {
        if ( json["colorchangeCD"] == true) {colorchangeCD = 1;}
        if ( json["colorchangeCD"] == false) {colorchangeCD = 0;}
        updateSettingsRequired = 1;
        if ((clockMode == 1) || (clockMode == 4)) { allBlank(); }
      }

      if (!json["scrollOptions1"].isNull()) {
        if ( json["scrollOptions1"] == true) {scrollOptions1 = 1;}
        if ( json["scrollOptions1"] == false) {scrollOptions1 = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollOptions2"].isNull()) {
        if ( json["scrollOptions2"] == true) {scrollOptions2 = 1;}
        if ( json["scrollOptions2"] == false) {scrollOptions2 = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollOptions3"].isNull()) {
        if ( json["scrollOptions3"] == true) {scrollOptions3 = 1;}
        if ( json["scrollOptions3"] == false) {scrollOptions3 = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollOptions4"].isNull()) {
        if ( json["scrollOptions4"] == true) {scrollOptions4 = 1;}
        if ( json["scrollOptions4"] == false) {scrollOptions4 = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollOptions7"].isNull()) {
        if ( json["scrollOptions7"] == true) {scrollOptions7 = 1;}
        if ( json["scrollOptions7"] == false) {scrollOptions7 = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollOptions8"].isNull()) {
        if ( json["scrollOptions8"] == true) {scrollOptions8 = 1;}
        if ( json["scrollOptions8"] == false) {scrollOptions8 = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollOverride"].isNull()) {
        if ( json["scrollOverride"] == true) {scrollOverride = 1;}
        if ( json["scrollOverride"] == false) {scrollOverride = 0;}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["scrollText"].isNull()) {
        scrollText = json["scrollText"].as<String>();
        if ( scrollText == "") {scrollText = "dAdS ArE tHE bESt";}
        updateSettingsRequired = 1;
        if (clockMode == 11) { allBlank(); }
      }

      if (!json["setdate"].isNull()) {
          int yeararg = json["setdate"]["year"].as<int>();
          int montharg = json["setdate"]["month"].as<int>();
          int dayarg = json["setdate"]["day"].as<int>();
          int hourarg = json["setdate"]["hour"].as<int>();
          int minarg = json["setdate"]["min"].as<int>();
          int secarg = json["setdate"]["sec"].as<int>();
          struct tm tm;
          tm.tm_year = yeararg - 1900;
          tm.tm_mon = montharg - 1;
          tm.tm_mday = dayarg;
          tm.tm_hour = hourarg;
          tm.tm_min = minarg;
          tm.tm_sec = secarg;
          time_t t = mktime(&tm);
          struct timeval now1 = { .tv_sec = t };
          settimeofday(&now1, NULL);
          printLocalTime();
      }

      if (!json["ClockMode"].isNull()) {
        allBlank();
        clockMode = 0;
        updateSettingsRequired = 1;
        realtimeMode = 0;
        printLocalTime();
        breakOutSet = 1;
      }

      if (!json["DateMode"].isNull()) {
        allBlank();
        clockMode = 7;
        updateSettingsRequired = 1;
        realtimeMode = 0;
      }

      if (!json["ScrollingMode"].isNull()) {
          allBlank();
          clockMode = 11;
          realtimeMode = 0;
        updateSettingsRequired = 1;
      };

      if (!json["DisplayOffMode"].isNull()) {
        allBlank();
        clockMode = 10;
        realtimeMode = 0;
        updateSettingsRequired = 1;
        breakOutSet = 1;
      }

      if (!json["CountdownMode"].isNull() || !json["StopwatchMode"].isNull()) {
        CountUpMillis = !json["CountdownMode"].isNull() ? CountUpMillis : millis();
        countdownMilliSeconds = !json["CountdownMode"].isNull() ? json["CountdownMode"].as<int>() : json["StopwatchMode"].as<int>();
        if (countdownMilliSeconds < 1000) {countdownMilliSeconds = 1000;}
        if (countdownMilliSeconds > 86400000) {countdownMilliSeconds = 86400000;}
        endCountDownMillis = millis() + countdownMilliSeconds;
        if (currentMode == 0) {currentMode = clockMode; currentReal = realtimeMode;}
        allBlank();
        clockMode = !json["CountdownMode"].isNull() ? 1 : 4;
        realtimeMode = 0;
      }

      if (!json["ScoreboardMode"].isNull()) {
        scoreboardLeft = json["ScoreboardMode"]["left"].as<int>();
        if (scoreboardLeft < 0) {scoreboardLeft = 0;}
        if (scoreboardLeft > 99) {scoreboardLeft = 99;}
        scoreboardRight = json["ScoreboardMode"]["right"].as<int>();
        if (scoreboardRight < 0) {scoreboardRight = 0;}
        if (scoreboardRight > 99) {scoreboardRight = 99;}
        allBlank();
        clockMode = 3;
        realtimeMode = 0;
        updateSettingsRequired = 1;
      }

      if (!json["haDisplayDegree"].isNull()) {
        haDisplayDegree = json["haDisplayDegree"].as<bool>();
      }
      if (!json["HADisplayMode"].isNull()) {
        haDisplayValue = json["HADisplayMode"].as<float>();
        allBlank();
        clockMode = 8;
        realtimeMode = 0;
        updateSettingsRequired = 1;
      }

      if (!json["lightshowMode"].isNull()) {
        allBlank();
        lightshowMode = json["lightshowMode"].as<int>();
        oldsnakecolor = CRGB::Green;
        getSlower = 180;
        clockMode = 5;
        realtimeMode = 1;
        updateSettingsRequired = 1;
      }

      if (!json["partyGame"].isNull()) {
        partyGameType = json["partyGame"].as<int>();
        allBlank();
        clockMode = 2;
        realtimeMode = 0;
        startPartyGame();
        updateSettingsRequired = 1;
      }

      haMarkDirty();
      server.send(200, "text/json", "{\"result\":\"ok\"}");
      return;
    }
    server.send(400, "text/json", "{\"result\":\"error\",\"reason\":\"empty\"}");
  });

  server.on("/playsong", HTTP_POST, []() {
    DynamicJsonDocument json(1500);
    if(server.args() > 0) {
      String body = server.arg(0);
      Serial.println("----");
      Serial.println(body);
      Serial.println("----");
      deserializeJson(json, server.arg(0));
    }
    server.send(204);
  });

   server.on("/getscheduler", []() {
    DynamicJsonDocument json(16192);
    String output;
    Serial.println("getscheduler running");
    json["jsonScheduleData"] = jsonScheduleData;
    serializeJson(json, output);
    server.send(200, "application/json", output);
  });

  server.on("/schedulerDelete", HTTP_POST, []() {
    DynamicJsonDocument json(1500);
    if(server.args() > 0) {
      deserializeJson(json, server.arg(0));
      Serial.println(server.arg(0));
      if (!json["deleteData"].isNull()) {
        String deleteData = json["deleteData"].as<String>();
        char deleteData_array[(deleteData.length() + 1)];
        deleteData.toCharArray(deleteData_array, (deleteData.length() + 1));
        char XprocessedText[255];
        strcpy(XprocessedText, "/scheduler/");
        strcat(XprocessedText, deleteData_array);
        strcat(XprocessedText, ".json");
      deleteFile(FileFS, XprocessedText);
      Serial.println("Deleting Schedule..");
      Serial.println(deleteData);
      }
      server.send(200, "text/json", "{\"result\":\"ok\"}");
    }
  server.send(401);
  createSchedulesArray();
  });

  server.on("/scheduler", HTTP_POST, []() {
    DynamicJsonDocument json(1500);
    if(server.args() > 0) {
      String body = server.arg(0);
      Serial.println(server.arg(0));
      deserializeJson(json, server.arg(0));
      if (!json["scheduleType"].isNull()) {
        int schedulerScheduleType = (int)json["scheduleType"];
        int schedulerDom = (int)json["dom"];
        int schedulerHours = (int)json["hours"];
        int schedulerMinutes = (int)json["minutes"];
        String schedulerSong = json["song"].as<String>();
        String schedulerTitle = json["title"].as<String>();
        int schedulerMon = (int)json["mon"];
        int schedulerTue = (int)json["tue"];
        int schedulerWed = (int)json["wed"];
        int schedulerThu = (int)json["thu"];
        int schedulerFri = (int)json["fri"];
        int schedulerSat = (int)json["sat"];
        int schedulerSun = (int)json["sun"];
        int schedulerDailyType = (int)json["dailyType"];
        int schedulerHourlyType = (int)json["hourlyType"];
        int schedulerGongmode = (int)json["gongmode"];
        int schedulerDaymode = (int)json["daymode"];
        int schedulerYear = (int)json["year"];
        int schedulerMonth = (int)json["month"];
        int schedulerDay = (int)json["day"];
        int schedulerRecurring = (int)json["recurring"];
        String schedulerUid = json["uid"].as<String>();
        char schedulerUid_array[(schedulerUid.length() + 1)];
        schedulerUid.toCharArray(schedulerUid_array, (schedulerUid.length() + 1));
        char body_array[(body.length() + 1)];
        body.toCharArray(body_array, (body.length() + 1));
        char XprocessedText[255];
        strcpy(XprocessedText, "/scheduler/");
        strcat(XprocessedText, schedulerUid_array);
        strcat(XprocessedText, ".json");
        writeFile(FileFS, XprocessedText, body_array);
      }
      server.send(200, "text/json", "{\"result\":\"ok\"}");
    }
  server.send(401);
  createSchedulesArray();
  });

  server.on("/debugpage", []() {
    const char* page =
        "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>Shelf Clock - Debug</title>"
        "<style>body{font-family:Arial, sans-serif; padding:16px;} pre{white-space:pre-wrap;} button{margin-right:8px;}</style>"
        "</head><body>"
        "<h3>Debug Info</h3>"
        "<div>"
        "<button onclick='sendTest()'>Test: Clock Mode</button>"
        "<button onclick='sendScroll()'>Test: Scroll Mode</button>"
        "<button onclick='loadDebug()'>Refresh</button>"
        "</div>"
        "<div id='status' style='margin-top:8px;'></div>"
        "<pre id='out'>Loading...</pre>"
        "<script>"
        "async function postUpdate(obj){"
        "const r=await fetch('/updateanything',{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(obj)});"
        "const t=await r.text();"
        "document.getElementById('status').textContent='POST status: '+r.status+' body: '+t;"
        "}"
        "function sendTest(){postUpdate({ClockMode:1}); setTimeout(loadDebug,500);}"
        "function sendScroll(){postUpdate({ScrollingMode:1}); setTimeout(loadDebug,500);}"
        "async function loadDebug(){"
        "const r=await fetch('/getdebug');"
        "const j=await r.json();"
        "document.getElementById('out').textContent=JSON.stringify(j,null,2);"
        "}"
        "loadDebug();"
        "</script>"
        "</body></html>";
    server.send(200, "text/html", page);
  });

  server.on("/getleds", []() {
    // Live LED state for the web preview: 2 hex chars brightness,
    // then 6 hex chars (rrggbb) per LED in physical strip order
    static char ledBuf[2 + NUM_LEDS * 6 + 1];
    static const char hexDigits[] = "0123456789abcdef";
    ledBuf[0] = hexDigits[brightness >> 4];
    ledBuf[1] = hexDigits[brightness & 0x0F];
    char* p = ledBuf + 2;
    for (int i = 0; i < NUM_LEDS; i++) {
      *p++ = hexDigits[LEDs[i].r >> 4];
      *p++ = hexDigits[LEDs[i].r & 0x0F];
      *p++ = hexDigits[LEDs[i].g >> 4];
      *p++ = hexDigits[LEDs[i].g & 0x0F];
      *p++ = hexDigits[LEDs[i].b >> 4];
      *p++ = hexDigits[LEDs[i].b & 0x0F];
    }
    *p = '\0';
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "text/plain", ledBuf);
  });

  server.on("/getdebug", []() {
    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    char monthsOfTheYear[12][12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    char tempRTC[64]="";
    char tempRTCE[64]="";
    String output;
    DynamicJsonDocument json(11000);

    json["resetReason"] = lastResetReason;
    json["freeHeap"] = ESP.getFreeHeap();
    json["minFreeHeap"] = ESP.getMinFreeHeap();
    json["heapSize"] = ESP.getHeapSize();
    json["softwareVersion"] = softwareVersion;
    json["ClockColorSettings"] = ClockColorSettings;
    json["COLOR_ORDER"] = STRINGIFY(COLOR_ORDER);
    json["ColorChangeFrequency"] = ColorChangeFrequency;
    json["CountUpMillis"] = CountUpMillis;
    json["DateColorSettings"] = DateColorSettings;
    json["DSTime"] = DSTime;
    sprintf(tempRTCE, "%s, ", daysOfTheWeek[timeinfo.tm_wday]);
    sprintf(tempRTCE + strlen(tempRTCE), "%s ", monthsOfTheYear[timeinfo.tm_mon]);
    sprintf(tempRTCE + strlen(tempRTCE), "%02d ", timeinfo.tm_mday);
    sprintf(tempRTCE + strlen(tempRTCE), "%d ", timeinfo.tm_year+1900);
    sprintf(tempRTCE + strlen(tempRTCE), "%02d:", timeinfo.tm_hour);
    sprintf(tempRTCE + strlen(tempRTCE), "%02d:", timeinfo.tm_min);
    sprintf(tempRTCE + strlen(tempRTCE), "%02d", timeinfo.tm_sec);
    json["ESP32-NTP"] = tempRTCE;
    json["FAKE_NUM_LEDS"] = FAKE_NUM_LEDS;
    json["LEDS_PER_DIGIT"] = LEDS_PER_DIGIT;
    json["LEDS_PER_SEGMENT"] = LEDS_PER_SEGMENT;
    json["LED_PIN"] = LED_PIN;
    json["LED_TYPE"] = STRINGIFY(LED_TYPE);
    json["MILLI_AMPS"] = MILLI_AMPS;
    json["NUMBER_OF_DIGITS"] = NUMBER_OF_DIGITS;
    json["NUM_LEDS"] = NUM_LEDS;
    json["SEGMENTS_LEDS"] = SEGMENTS_LEDS;
    json["SEGMENTS_PER_NUMBER"] = SEGMENTS_PER_NUMBER;
    json["SPECTRUM_PIXELS"] = SPECTRUM_PIXELS;
    json["SPOT_LEDS"] = SPOT_LEDS;
    json["USE_LITTLEFS"] = true;
    json["WiFi IP"] = WiFi.localIP().toString();
    json["WiFi_MAX_RETRIES"] = WiFi_MAX_RETRIES;
    json["WiFi_MAX_RETRY_DURATION"] = WiFi_MAX_RETRY_DURATION;
    json["WiFi_elapsedTime"] = WiFi_elapsedTime;
    json["WiFi_retryCount"] = WiFi_retryCount;
    json["WiFi_startTime"] = WiFi_startTime;
    json["WiFi_totalReconnections"] = WiFi_totalReconnections;
    json["lastUpdateAt"] = lastUpdateAt;
    json["lastUpdateBody"] = lastUpdateBody;
    json["mqtt_connected"] = haMqtt.isConnected();
    json["mqtt_tcp_connected"] = mqttWiFiClient.connected();
    json["breakOutSet"] = breakOutSet;
    json["brightness"] = brightness;
    json["clearOldLeds"] = clearOldLeds;
    json["clockDisplayType"] = clockDisplayType;
    json["clockMode"] = clockMode;
    json["colonType"] = colonType;
    json["colorWheelPosition"] = colorWheelPosition;
    json["colorWheelPositionTwo"] = colorWheelPositionTwo;
    json["colorWheelSpeed"] = colorWheelSpeed;
    json["colorchangeCD"] = colorchangeCD;
    json["countdownMilliSeconds"] = countdownMilliSeconds;
    json["countupMilliSeconds"] = countupMilliSeconds;
    json["currentMode"] = currentMode;
    json["currentReal"] = currentReal;
    json["cylonPosition"] = cylonPosition;
    json["dateDisplayType"] = dateDisplayType;
    json["daylightOffset_sec"] = daylightOffset_sec;
    json["daysUptime"] = daysUptime;
    json["dotsOn"] = dotsOn;
    json["endCountDownMillis"] = endCountDownMillis;
    json["foodSpot"] = foodSpot;
    json["getSlower"] = getSlower;
    json["gmtOffset_sec"] = gmtOffset_sec;
    JsonArray greenMatrixArray = json.createNestedArray("greenMatrix");
    for (int i = 0; i < SPECTRUM_PIXELS; i++) { greenMatrixArray.add(greenMatrix[i]); }
    json["host"] = host;
    json["hoursUptime"] = hoursUptime;
    json["isAsleep"] = isAsleep;
    json["lightshowMode"] = lightshowMode;
    json["partyGameType"] = partyGameType;
    json["lightshowSpeed"] = lightshowSpeed;
    json["millis"] = millis();
  	json["minutesUptime"] = minutesUptime;
    json["ntpServer"] = ntpServer;
    json["pastelColors"] = pastelColors;
    json["prevTime"] = prevTime;
    json["prevTime2"] = prevTime2;
    json["previousTimeDay"] = previousTimeDay;
    json["previousTimeHour"] = previousTimeHour;
    json["previousTimeMin"] = previousTimeMin;
    json["previousTimeMonth"] = previousTimeMonth;
    json["previousTimeWeek"] = previousTimeWeek;
    JsonArray rainArray = json.createNestedArray("rain");
    for (int i = 0; i < SPECTRUM_PIXELS; i++) { rainArray.add(rain[i]); }
    json["randomDayPassed"] = randomDayPassed;
    json["randomHourPassed"] = randomHourPassed;
    json["randomMinPassed"] = randomMinPassed;
    json["randomMonthPassed"] = randomMonthPassed;
    json["randomWeekPassed"] = randomWeekPassed;
    json["realtimeMode"] = realtimeMode;
    json["scoreboardLeft"] = scoreboardLeft;
    json["scoreboardRight"] = scoreboardRight;
    json["scrollColorSettings"] = scrollColorSettings;
    json["scrollFrequency"] = scrollFrequency;
    json["scrollOptions1"] = scrollOptions1;
    json["scrollOptions2"] = scrollOptions2;
    json["scrollOptions3"] = scrollOptions3;
    json["scrollOptions4"] = scrollOptions4;
    json["scrollOptions7"] = scrollOptions7;
    json["scrollOptions8"] = scrollOptions8;
    json["scrollOverride"] = scrollOverride;
    json["scrollText"] = scrollText.c_str();
    json["firstRun"] = firstRun;
    json["sleepTimerCurrent"] = sleepTimerCurrent;
    json["snakeLastDirection"] = snakeLastDirection;
    json["snakePosition"] = snakePosition;
    json["snakeWaiting"] = snakeWaiting;
    json["spotlightsColorSettings"] = spotlightsColorSettings;
    json["suspendFrequency"] = suspendFrequency;
    json["suspendType"] = suspendType;
    json["updateSettingsRequired"] = updateSettingsRequired;
    json["useSpotlights"] = useSpotlights;
      json["countdownColorValue"] = countdownColorValue;
      json["hourColorValue"] = hourColorValue;
      json["colonColorValue"] = colonColorValue;
      json["minColorValue"] = minColorValue;
      json["spotlightsColorValue"] = spotlightsColorValue;
      json["scrollColorValue"] = scrollColorValue;
      json["scoreboardColorLeftValue"] = scoreboardColorLeftValue;
      json["scoreboardColorRightValue"] = scoreboardColorRightValue;
      json["dayColorValue"] = dayColorValue;
      json["monthColorValue"] = monthColorValue;
      json["separatorColorValue"] = separatorColorValue;

    serializeJson(json, output);
    server.send(200, "application/json", output);
  });

  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    if (Update.hasError()) {
      // Old firmware is still intact - report the error, no restart needed
      server.send(200, "text/plain", String("FAIL: ") + Update.errorString());
    } else {
      server.send(200, "text/plain", "OK - Update successful, restarting...");
      server.client().flush();  // make sure the browser gets the answer before we reboot
      delay(750);
      ESP.restart();
    }
  }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        otaInProgress = true;   // stop loop() from driving LEDs/FS during flash write
        allBlank();
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        otaInProgress = false;
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.abort();
        otaInProgress = false;
        webLog("Firmware update aborted");
      }
  });

  server.on("/updatefs", HTTP_GET, []() {
    const char* page =
        "<!doctype html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>Shelf Clock - Update Filesystem</title></head><body>"
        "<h3>Update Filesystem (LittleFS)</h3>"
        "<form method='POST' action='/updatefs' enctype='multipart/form-data'>"
        "<input type='file' name='update'>"
        "<input type='submit' value='Upload'>"
        "</form>"
        "<p>Use the Filesystem image (.bin from PlatformIO Upload Filesystem).</p>"
        "</body></html>";
    server.send(200, "text/html", page);
  });

  server.on("/updatefs", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    if (Update.hasError()) {
      server.send(200, "text/plain", String("FAIL: ") + Update.errorString());
      FileFS.begin();  // try to remount whatever filesystem is left
    } else {
      server.send(200, "text/plain", "OK - Filesystem updated, restarting...");
      server.client().flush();  // make sure the browser gets the answer before we reboot
      delay(750);
      ESP.restart();
    }
  }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("UpdateFS: %s\n", upload.filename.c_str());
        otaInProgress = true;   // stop loop() from driving LEDs/FS during flash write
        allBlank();
        FileFS.end();           // unmount before the partition gets overwritten
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(Serial);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
          Serial.printf("UpdateFS Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        otaInProgress = false;
      } else if (upload.status == UPLOAD_FILE_ABORTED) {
        Update.abort();
        FileFS.begin();         // remount the (unchanged) filesystem
        otaInProgress = false;
        webLog("Filesystem update aborted");
      }
  });

  server.on("/getMQTTSettings", []() {
    DynamicJsonDocument json(512);
    String output;
    json["mqtt_server"] = prefGetStr("mqttServer", "");
    json["mqtt_port"] = prefGetInt("mqttPort", 1883);
    json["mqtt_user"] = prefGetStr("mqttUser", "");
    // Never expose the real password - send a mask placeholder if one is set
    json["mqtt_password"] = prefGetStr("mqttPassword", "").length() > 0 ? MQTT_PASSWORD_MASK : "";
    serializeJson(json, output);
    server.send(200, "application/json", output);
  });

  server.on("/setMQTTSettings", HTTP_POST, []() {
    DynamicJsonDocument json(512);
    String body = server.arg("plain");
    if (body.length() == 0 && server.args() > 0) {
      body = server.arg(0);
    }
    if (body.length() > 0) {
      DeserializationError err = deserializeJson(json, body);
      if (err) {
        server.send(400, "application/json", "{\"result\":\"error\",\"reason\":\"json\"}");
        return;
      }
      if (!json["mqtt_server"].isNull()) prefPutStr("mqttServer", json["mqtt_server"].as<String>());
      if (!json["mqtt_port"].isNull()) prefPutInt("mqttPort", json["mqtt_port"].as<int>());
      if (!json["mqtt_user"].isNull()) prefPutStr("mqttUser", json["mqtt_user"].as<String>());
      // Keep the stored password when the client sends back the unchanged mask
      if (!json["mqtt_password"].isNull() && json["mqtt_password"].as<String>() != MQTT_PASSWORD_MASK) prefPutStr("mqttPassword", json["mqtt_password"].as<String>());

      Serial.println("MQTT settings saved to Preferences");

      setupMQTT();
      if (mqttServer.length() > 0) {
        haMqtt.begin(mqttServer.c_str(), mqttPort, mqttUser.c_str(), mqttPassword.c_str());
      }

      server.send(200, "application/json", "{\"result\":\"ok\"}");
    } else {
      if (server.hasArg("mqttServer")) {
        prefPutStr("mqttServer", server.arg("mqttServer"));
        prefPutInt("mqttPort", server.arg("mqttPort").toInt());
        prefPutStr("mqttUser", server.arg("mqttUser"));
        if (server.arg("mqttPassword") != MQTT_PASSWORD_MASK) prefPutStr("mqttPassword", server.arg("mqttPassword"));
        setupMQTT();
        if (mqttServer.length() > 0) {
          haMqtt.begin(mqttServer.c_str(), mqttPort, mqttUser.c_str(), mqttPassword.c_str());
        }
        server.send(200, "text/plain", "OK");
      } else {
        server.send(400, "application/json", "{\"result\":\"error\",\"reason\":\"empty\"}");
      }
    }
  });

  server.on("/testMQTT", []() {
    DynamicJsonDocument json(256);
    json["connected"] = mqttConnected || haMqtt.isConnected();
    json["server"] = mqttServer;
    json["port"] = mqttPort;

    String output;
    serializeJson(json, output);
    server.send(200, "application/json", output);
  });

  server.on("/debuglog", []() {
    String html = "<!DOCTYPE html><html><head>"
                  "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                  "<title>Debug Log</title>"
                  "<style>body{font-family:monospace;padding:10px;background:#000;color:#0f0;}"
                  "pre{white-space:pre-wrap;word-wrap:break-word;font-size:12px;}"
                  ".refresh{position:fixed;top:10px;right:10px;padding:10px;background:#333;color:#fff;"
                  "border:none;cursor:pointer;border-radius:5px;}</style>"
                  "<script>function refresh(){location.reload();}"
                  "setInterval(refresh,5000);</script></head><body>"
                  "<button class='refresh' onclick='refresh()'>Refresh</button>"
                  "<h2>ShelfClock Debug Log</h2><pre>";

    for (int i = 0; i < DEBUG_LOG_SIZE; i++) {
      int idx = (debugLogIndex + i) % DEBUG_LOG_SIZE;
      if (debugLog[idx][0] != '\0') {
        html += debugLog[idx];
        html += "\n";
      }
    }

    html += "</pre></body></html>";
    server.send(200, "text/html", html);
  });

  // --- Settings Export (download as JSON file) ---
  server.on("/exportsettings", []() {
    DynamicJsonDocument doc(16384);

    // Clock settings (same keys as saveclockSettings)
    JsonObject settings = doc.createNestedObject("settings");
    settings["gmtOffset_sec"] = gmtOffset_sec;
    settings["timeZone"] = timeZone;
    settings["DSTime"] = DSTime;
    settings["countdownColor"] = countdownColorValue;
    settings["spotlightsColor"] = spotlightsColorValue;
    settings["hourColor"] = hourColorValue;
    settings["minColor"] = minColorValue;
    settings["colonColor"] = colonColorValue;
    settings["dayColor"] = dayColorValue;
    settings["monthColor"] = monthColorValue;
    settings["separatorColor"] = separatorColorValue;
    settings["scoreboardColorLeft"] = scoreboardColorLeftValue;
    settings["scoreboardColorRight"] = scoreboardColorRightValue;
    settings["scrollColor"] = scrollColorValue;
    settings["clockMode"] = clockMode;
    settings["pastelColors"] = pastelColors;
    settings["ClockColorSettings"] = ClockColorSettings;
    settings["DateColorSettings"] = DateColorSettings;
    settings["colonType"] = colonType;
    settings["ColorChangeFrequency"] = ColorChangeFrequency;
    settings["scrollText"] = scrollText;
    settings["clockDisplayType"] = clockDisplayType;
    settings["dateDisplayType"] = dateDisplayType;
    settings["colorchangeCD"] = colorchangeCD;
    settings["realtimeMode"] = realtimeMode;
    settings["spotlightsColorSettings"] = spotlightsColorSettings;
    settings["brightness"] = brightness;
    settings["useSpotlights"] = useSpotlights;
    settings["scrollColorSettings"] = scrollColorSettings;
    settings["scrollFrequency"] = scrollFrequency;
    settings["scrollOverride"] = scrollOverride;
    settings["scrollOptions1"] = scrollOptions1;
    settings["scrollOptions2"] = scrollOptions2;
    settings["scrollOptions3"] = scrollOptions3;
    settings["scrollOptions4"] = scrollOptions4;
    settings["scrollOptions7"] = scrollOptions7;
    settings["scrollOptions8"] = scrollOptions8;
    settings["lightshowMode"] = lightshowMode;
    settings["suspendFrequency"] = suspendFrequency;
    settings["suspendType"] = suspendType;
    settings["partyGameType"] = partyGameType;

    // MQTT settings from NVS
    JsonObject mqtt = doc.createNestedObject("mqtt");
    mqtt["server"] = prefGetStr("mqttServer", "");
    mqtt["port"] = prefGetInt("mqttPort", 1883);
    mqtt["user"] = prefGetStr("mqttUser", "");
    mqtt["password"] = prefGetStr("mqttPassword", "");

    // Scheduler data from /scheduler/ files
    JsonArray schedules = doc.createNestedArray("schedules");
    File dir = FileFS.open("/scheduler");
    if (dir && dir.isDirectory()) {
      File entry = dir.openNextFile();
      while (entry) {
        String fname = entry.name();
        if (fname.endsWith(".json")) {
          String content = entry.readString();
          DynamicJsonDocument schedDoc(1024);
          if (!deserializeJson(schedDoc, content)) {
            schedules.add(schedDoc.as<JsonObject>());
          }
        }
        entry.close();
        entry = dir.openNextFile();
      }
      dir.close();
    }

    // Metadata
    doc["_backup"] = "ShelfClock";
    doc["_version"] = softwareVersion;
    doc["_timestamp"] = millis();

    String output;
    serializeJsonPretty(doc, output);
    server.sendHeader("Content-Disposition", "attachment; filename=\"shelfclock-backup.json\"");
    server.send(200, "application/json", output);
    webLog("Settings exported (" + String(output.length()) + " bytes)");
  });

  // --- Settings Import (restore from JSON file) ---
  server.on("/importsettings", HTTP_POST, []() {
    String body = server.arg("plain");
    if (body.length() == 0 && server.args() > 0) {
      body = server.arg(0);
    }
    if (body.length() == 0) {
      server.send(400, "application/json", "{\"result\":\"error\",\"reason\":\"empty\"}");
      return;
    }

    DynamicJsonDocument doc(16384);
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
      server.send(400, "application/json", "{\"result\":\"error\",\"reason\":\"json parse failed\"}");
      return;
    }

    // Verify it's a ShelfClock backup
    if (!doc.containsKey("_backup") || doc["_backup"] != "ShelfClock") {
      server.send(400, "application/json", "{\"result\":\"error\",\"reason\":\"not a ShelfClock backup file\"}");
      return;
    }

    // Restore clock settings (only keys that exist in the backup)
    if (doc.containsKey("settings")) {
      JsonObject s = doc["settings"];
      if (s.containsKey("gmtOffset_sec")) gmtOffset_sec = s["gmtOffset_sec"].as<long>();
      if (s.containsKey("DSTime")) DSTime = s["DSTime"].as<bool>();
      if (s.containsKey("timeZone")) timeZone = s["timeZone"].as<String>();
      if (s.containsKey("countdownColor")) { countdownColorValue = s["countdownColor"].as<uint32_t>(); countdownColor = CRGB(countdownColorValue); }
      if (s.containsKey("spotlightsColor")) { spotlightsColorValue = s["spotlightsColor"].as<uint32_t>(); spotlightsColor = CRGB(spotlightsColorValue); }
      if (s.containsKey("hourColor")) { hourColorValue = s["hourColor"].as<uint32_t>(); hourColor = CRGB(hourColorValue); prefPutU32("hourColor", hourColorValue); }
      if (s.containsKey("minColor")) { minColorValue = s["minColor"].as<uint32_t>(); minColor = CRGB(minColorValue); }
      if (s.containsKey("colonColor")) { colonColorValue = s["colonColor"].as<uint32_t>(); colonColor = CRGB(colonColorValue); }
      if (s.containsKey("dayColor")) { dayColorValue = s["dayColor"].as<uint32_t>(); dayColor = CRGB(dayColorValue); }
      if (s.containsKey("monthColor")) { monthColorValue = s["monthColor"].as<uint32_t>(); monthColor = CRGB(monthColorValue); }
      if (s.containsKey("separatorColor")) { separatorColorValue = s["separatorColor"].as<uint32_t>(); separatorColor = CRGB(separatorColorValue); }
      if (s.containsKey("scoreboardColorLeft")) { scoreboardColorLeftValue = s["scoreboardColorLeft"].as<uint32_t>(); scoreboardColorLeft = CRGB(scoreboardColorLeftValue); }
      if (s.containsKey("scoreboardColorRight")) { scoreboardColorRightValue = s["scoreboardColorRight"].as<uint32_t>(); scoreboardColorRight = CRGB(scoreboardColorRightValue); }
      if (s.containsKey("scrollColor")) { scrollColorValue = s["scrollColor"].as<uint32_t>(); scrollColor = CRGB(scrollColorValue); }
      if (s.containsKey("clockMode")) clockMode = s["clockMode"].as<byte>();
      if (s.containsKey("pastelColors")) pastelColors = s["pastelColors"].as<byte>();
      if (s.containsKey("ClockColorSettings")) ClockColorSettings = s["ClockColorSettings"].as<byte>();
      if (s.containsKey("DateColorSettings")) DateColorSettings = s["DateColorSettings"].as<byte>();
      if (s.containsKey("colonType")) colonType = s["colonType"].as<int>();
      if (s.containsKey("ColorChangeFrequency")) ColorChangeFrequency = s["ColorChangeFrequency"].as<byte>();
      if (s.containsKey("scrollText")) scrollText = s["scrollText"].as<String>();
      if (s.containsKey("clockDisplayType")) clockDisplayType = s["clockDisplayType"].as<byte>();
      if (s.containsKey("dateDisplayType")) dateDisplayType = s["dateDisplayType"].as<byte>();
      if (s.containsKey("colorchangeCD")) colorchangeCD = s["colorchangeCD"].as<bool>();
      if (s.containsKey("realtimeMode")) realtimeMode = s["realtimeMode"].as<int>();
      if (s.containsKey("spotlightsColorSettings")) spotlightsColorSettings = s["spotlightsColorSettings"].as<int>();
      if (s.containsKey("brightness")) { brightness = s["brightness"].as<byte>(); FastLED.setBrightness(brightness); }
      if (s.containsKey("useSpotlights")) useSpotlights = s["useSpotlights"].as<bool>();
      if (s.containsKey("scrollColorSettings")) scrollColorSettings = s["scrollColorSettings"].as<int>();
      if (s.containsKey("scrollFrequency")) scrollFrequency = s["scrollFrequency"].as<int>();
      if (s.containsKey("scrollOverride")) scrollOverride = s["scrollOverride"].as<bool>();
      if (s.containsKey("scrollOptions1")) scrollOptions1 = s["scrollOptions1"].as<bool>();
      if (s.containsKey("scrollOptions2")) scrollOptions2 = s["scrollOptions2"].as<bool>();
      if (s.containsKey("scrollOptions3")) scrollOptions3 = s["scrollOptions3"].as<bool>();
      if (s.containsKey("scrollOptions4")) scrollOptions4 = s["scrollOptions4"].as<bool>();
      if (s.containsKey("scrollOptions7")) scrollOptions7 = s["scrollOptions7"].as<bool>();
      if (s.containsKey("scrollOptions8")) scrollOptions8 = s["scrollOptions8"].as<bool>();
      if (s.containsKey("lightshowMode")) lightshowMode = s["lightshowMode"].as<int>();
      if (s.containsKey("suspendFrequency")) suspendFrequency = s["suspendFrequency"].as<int>();
      if (s.containsKey("suspendType")) suspendType = s["suspendType"].as<byte>();
      if (s.containsKey("partyGameType")) partyGameType = s["partyGameType"].as<int>();
    }

    // Restore MQTT settings to NVS
    if (doc.containsKey("mqtt")) {
      JsonObject m = doc["mqtt"];
      if (m.containsKey("server")) prefPutStr("mqttServer", m["server"].as<String>());
      if (m.containsKey("port")) prefPutInt("mqttPort", m["port"].as<int>());
      if (m.containsKey("user")) prefPutStr("mqttUser", m["user"].as<String>());
      if (m.containsKey("password")) prefPutStr("mqttPassword", m["password"].as<String>());
      setupMQTT();
    }

    // Restore scheduler files
    if (doc.containsKey("schedules")) {
      // Ensure scheduler directory exists
      if (!FileFS.exists("/scheduler")) {
        FileFS.mkdir("/scheduler");
      }
      JsonArray schedArray = doc["schedules"].as<JsonArray>();
      for (JsonObject sched : schedArray) {
        if (sched.containsKey("uid")) {
          String uid = sched["uid"].as<String>();
          String path = "/scheduler/" + uid + ".json";
          String content;
          serializeJson(sched, content);
          File f = FileFS.open(path, FILE_WRITE);
          if (f) {
            f.print(content);
            f.close();
          }
        }
      }
      createSchedulesArray();
    }

    // Save clock settings and apply
    applyTimeConfig();
    saveclockSettings("generic");
    haMarkDirty();
    allBlank();

    webLog("Settings imported successfully");
    server.send(200, "application/json", "{\"result\":\"ok\",\"message\":\"Settings restored successfully. Page will reload.\"}");
  });

  server.on("/formatfs", HTTP_GET, []() {
    // Show confirmation page only - the actual format requires a POST with confirm=yes
    String html = "<!DOCTYPE html><html><head>"
                  "<title>Format LittleFS?</title></head><body>"
                  "<h2>Format LittleFS?</h2>"
                  "<p><b>Warning:</b> This erases all files (web interface, settings, schedules) and restarts the clock.</p>"
                  "<form method='POST' action='/formatfs'>"
                  "<input type='hidden' name='confirm' value='yes'>"
                  "<button type='submit'>Yes, format now</button> "
                  "<a href='/settings.html'>Cancel</a>"
                  "</form></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/formatfs", HTTP_POST, []() {
    if (server.arg("confirm") != "yes") {
      server.send(400, "text/plain", "Missing confirm=yes");
      return;
    }
    webLog("Format LittleFS requested via web interface");
    String html = "<!DOCTYPE html><html><head>"
                  "<meta http-equiv='refresh' content='15;url=/settings.html'>"
                  "<title>Formatting...</title></head><body>"
                  "<h2>Formatting LittleFS...</h2>"
                  "<p>Please wait, ESP32 will restart...</p></body></html>";
    server.send(200, "text/html", html);
    delay(1000);

    FileFS.end();
    if (FileFS.format()) {
      webLog("LittleFS formatted successfully");
    } else {
      webLog("LittleFS format failed");
    }
    delay(1000);
    ESP.restart();
  });

}
