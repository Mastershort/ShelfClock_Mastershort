#include "../include/ha_integration.h"
#include <ArduinoHA.h>
#include <FastLED.h>
#include <time.h>
#include "WiFi.h"

extern WiFiClient haWiFiClient;

byte mac[] = {0x00, 0x10, 0xFA, 0x6E, 0x38, 0x5B};
HADevice device(mac, sizeof(mac));
HAMqtt haMqtt(haWiFiClient, device, 120);

// --- Entities ---
HASelect haMode("sc_mode");
HASelect haLightShowMode("sc_lightshow_mode");
HASelect haPalette("sc_palette");
HASelect haColorChangeFreq("sc_color_change_freq");
HASelect haSuspendType("sc_suspend_type");
HASelect haSuspendFreq("sc_suspend_freq");
HASelect haClockDisplay("sc_clock_display");
HASelect haColonType("sc_colon_type");
HANumber haGmtOffset("sc_gmt_offset");
HASwitch haDST("sc_dst");
HASelect haClockColorSettings("sc_clock_color_settings");
HASelect haDateDisplay("sc_date_display");
HASelect haDateColorSettings("sc_date_color_settings");
HASelect haScrollFrequency("sc_scroll_frequency");
HASwitch haScrollOpt1("sc_scroll_opt1");
HASwitch haScrollOpt2("sc_scroll_opt2");
HASwitch haScrollOpt3("sc_scroll_opt3");
HASwitch haScrollOpt4("sc_scroll_opt4");
HASwitch haScrollOpt7("sc_scroll_opt7");
HASwitch haScrollOpt8("sc_scroll_opt8");
HASelect haScrollColorSettings("sc_scroll_color_settings");
HASwitch haScrollOverride("sc_scroll_override");
HASwitch haUseSpotlights("sc_use_spotlights");
HASelect haSpotlightsColorSettings("sc_spotlights_color_settings");
HASwitch haColorChangeCD("sc_colorchange_cd");
HANumber haBrightness("sc_brightness");

// Colors as RGB lights
HALight haHourColor("sc_hour_color", HALight::RGBFeature);
HALight haMinColor("sc_min_color", HALight::RGBFeature);
HALight haColonColor("sc_colon_color", HALight::RGBFeature);
HALight haDayColor("sc_day_color", HALight::RGBFeature);
HALight haMonthColor("sc_month_color", HALight::RGBFeature);
HALight haSeparatorColor("sc_separator_color", HALight::RGBFeature);
HALight haCountdownColor("sc_countdown_color", HALight::RGBFeature);
HALight haScoreLeftColor("sc_score_left_color", HALight::RGBFeature);
HALight haScoreRightColor("sc_score_right_color", HALight::RGBFeature);
HALight haScrollColor("sc_scroll_color", HALight::RGBFeature);
HALight haSpotlightsColor("sc_spotlights_color", HALight::RGBFeature);

// Mode helpers
HANumber haCountdownSeconds("sc_countdown_seconds");
HANumber haStopwatchSeconds("sc_stopwatch_seconds");
HANumber haScoreboardLeft("sc_score_left");
HANumber haScoreboardRight("sc_score_right");
HAButton haCountdownStart("sc_countdown_start");
HAButton haStopwatchStart("sc_stopwatch_start");
HAButton haScoreboardApply("sc_score_apply");


static unsigned long lastHaSyncMs = 0;
static uint32_t pendingCountdownMs = 60000;
static uint32_t pendingStopwatchMs = 60000;
static bool haDirty = false;

static int indexOfValue(const int* values, size_t count, int value, int fallbackIndex = 0) {
    for (size_t i = 0; i < count; i++) {
        if (values[i] == value) {
            return (int)i;
        }
    }
    return fallbackIndex;
}

static int modeToIndex(byte mode) {
    switch (mode) {
        case 0:  return 0; // Clock
        case 1:  return 1; // Countdown
        case 3:  return 2; // Scoreboard
        case 4:  return 3; // Stopwatch
        case 5:  return 4; // Lightshow
        case 6:  return 5; // Rainbows
        case 7:  return 6; // Date
        case 10: return 7; // Display Off
        case 11: return 8; // Scroll
        default: return 0;
    }
}

static void markSettingsChanged() {
    updateSettingsRequired = 1;
    haDirty = true;
}

static void applyClockMode(byte mode) {
    allBlank();
    clockMode = mode;
    realtimeMode = 0;
    markSettingsChanged();
}

static void applyCountdownOrStopwatch(uint32_t ms, bool isStopwatch) {
    if (ms < 1000) ms = 1000;
    if (ms > 86400000UL) ms = 86400000UL;
    if (isStopwatch) {
        CountUpMillis = millis();
    }
    countdownMilliSeconds = ms;
    endCountDownMillis = millis() + countdownMilliSeconds;
    if (currentMode == 0) { currentMode = clockMode; currentReal = realtimeMode; }
    allBlank();
    clockMode = isStopwatch ? 4 : 1;
    realtimeMode = 0;
    markSettingsChanged();
}

static void applyScoreboard() {
    if (scoreboardLeft < 0) scoreboardLeft = 0;
    if (scoreboardLeft > 99) scoreboardLeft = 99;
    if (scoreboardRight < 0) scoreboardRight = 0;
    if (scoreboardRight > 99) scoreboardRight = 99;
    allBlank();
    clockMode = 3;
    realtimeMode = 0;
    markSettingsChanged();
}

// --- Callbacks ---
void onModeCommand(int8_t index, HASelect* sender) {
    switch (index) {
        case 0: // Clock
            applyClockMode(0);
            printLocalTime();
            breakOutSet = 1;
            break;
        case 1: // Countdown
            applyCountdownOrStopwatch(pendingCountdownMs, false);
            break;
        case 2: // Scoreboard
            applyScoreboard();
            break;
        case 3: // Stopwatch
            applyCountdownOrStopwatch(pendingStopwatchMs, true);
            break;
        case 4: // Lightshow
            allBlank();
            clockMode = 5;
            realtimeMode = 1;
            markSettingsChanged();
            break;
        case 5: // Rainbows
            applyClockMode(6);
            break;
        case 6: // Date
            applyClockMode(7);
            break;
        case 7: // Display Off
            applyClockMode(10);
            breakOutSet = 1;
            break;
        case 8: // Scroll
            applyClockMode(11);
            break;
        default:
            return;
    }
    sender->setState(index);
}

void onLightShowCommand(int8_t index, HASelect* sender) {
    allBlank();
    lightshowMode = index;
    oldsnakecolor = CRGB::Green;
    getSlower = 180;
    clockMode = 5;
    realtimeMode = 1;
    markSettingsChanged();
    sender->setState(index);
}

void onPaletteCommand(int8_t index, HASelect* sender) {
    pastelColors = (index == 1) ? 1 : 0;
    markSettingsChanged();
    allBlank();
    sender->setState(index);
}

void onColorChangeFreqCommand(int8_t index, HASelect* sender) {
    ColorChangeFrequency = index;
    randomMinPassed = 1;
    randomHourPassed = 1;
    randomDayPassed = 1;
    randomWeekPassed = 1;
    randomMonthPassed = 1;
    markSettingsChanged();
    allBlank();
    sender->setState(index);
}

void onSuspendTypeCommand(int8_t index, HASelect* sender) {
    suspendType = index;
    markSettingsChanged();
    sender->setState(index);
}

void onSuspendFreqCommand(int8_t index, HASelect* sender) {
    static const int values[] = {1, 5, 10, 15, 30, 45, 60};
    suspendFrequency = values[index < 0 ? 0 : index];
    markSettingsChanged();
    sender->setState(index);
}

void onClockDisplayCommand(int8_t index, HASelect* sender) {
    clockDisplayType = index;
    markSettingsChanged();
    if (clockMode == 0) { allBlank(); }
    sender->setState(index);
}

void onColonTypeCommand(int8_t index, HASelect* sender) {
    colonType = index;
    markSettingsChanged();
    if (clockMode == 0) { allBlank(); }
    sender->setState(index);
}

void onGmtOffsetCommand(HANumeric value, HANumber* sender) {
    gmtOffset_sec = (long)value.toFloat();
    configTime(gmtOffset_sec, (daylightOffset_sec * DSTime), ntpServer);
    if (!getLocalTime(&timeinfo)) { Serial.println("Error, no NTP Server found!"); }
    markSettingsChanged();
    if (clockMode == 0) { allBlank(); }
    printLocalTime();
    sender->setState(value);
}

void onDSTCommand(bool state, HASwitch* sender) {
    DSTime = state ? 1 : 0;
    configTime(gmtOffset_sec, (daylightOffset_sec * DSTime), ntpServer);
    if (!getLocalTime(&timeinfo)) { Serial.println("Error, no NTP Server found!"); }
    markSettingsChanged();
    if (clockMode == 0) { allBlank(); }
    printLocalTime();
    sender->setState(state);
}

void onClockColorSettingsCommand(int8_t index, HASelect* sender) {
    ClockColorSettings = index;
    markSettingsChanged();
    if (clockMode == 0) { allBlank(); }
    sender->setState(index);
}

void onDateDisplayCommand(int8_t index, HASelect* sender) {
    dateDisplayType = index;
    markSettingsChanged();
    if (clockMode == 7) { allBlank(); }
    sender->setState(index);
}

void onDateColorSettingsCommand(int8_t index, HASelect* sender) {
    DateColorSettings = index;
    markSettingsChanged();
    if (clockMode == 7) { allBlank(); }
    sender->setState(index);
}

void onScrollFrequencyCommand(int8_t index, HASelect* sender) {
    static const int values[] = {1, 2, 3, 4, 5, 6};
    scrollFrequency = values[index < 0 ? 0 : index];
    markSettingsChanged();
    if (clockMode == 11) { allBlank(); }
    sender->setState(index);
}

void onScrollColorSettingsCommand(int8_t index, HASelect* sender) {
    scrollColorSettings = index;
    markSettingsChanged();
    if (clockMode == 11) { allBlank(); }
    sender->setState(index);
}

void onScrollOverrideCommand(bool state, HASwitch* sender) {
    scrollOverride = state ? 1 : 0;
    markSettingsChanged();
    if (clockMode == 11) { allBlank(); }
    sender->setState(state);
}

void onScrollOptCommand(bool state, HASwitch* sender) {
    bool v = state ? 1 : 0;
    if (sender == &haScrollOpt1) scrollOptions1 = v;
    if (sender == &haScrollOpt2) scrollOptions2 = v;
    if (sender == &haScrollOpt3) scrollOptions3 = v;
    if (sender == &haScrollOpt4) scrollOptions4 = v;
    if (sender == &haScrollOpt7) scrollOptions7 = v;
    if (sender == &haScrollOpt8) scrollOptions8 = v;
    markSettingsChanged();
    if (clockMode == 11) { allBlank(); }
    sender->setState(state);
}

void onUseSpotlightsCommand(bool state, HASwitch* sender) {
    useSpotlights = state ? 1 : 0;
    markSettingsChanged();
    ShelfDownLights();
    sender->setState(state);
}

void onSpotlightsColorSettingsCommand(int8_t index, HASelect* sender) {
    spotlightsColorSettings = index;
    markSettingsChanged();
    ShelfDownLights();
    sender->setState(index);
}

void onColorChangeCDCommand(bool state, HASwitch* sender) {
    colorchangeCD = state ? 1 : 0;
    markSettingsChanged();
    if ((clockMode == 1) || (clockMode == 4)) { allBlank(); }
    sender->setState(state);
}

void onBrightnessCommand(HANumeric value, HANumber* sender) {
    brightness = (byte)value.toFloat();
    markSettingsChanged();
    ShelfDownLights();
    sender->setState(value);
}

static HALight::RGBColor rgbFromValue(uint32_t value) {
    return HALight::RGBColor(
        (uint8_t)((value >> 16) & 0xFF),
        (uint8_t)((value >> 8) & 0xFF),
        (uint8_t)(value & 0xFF)
    );
}

static uint32_t valueFromRGB(const HALight::RGBColor& color) {
    return ((uint32_t)color.red << 16) | ((uint32_t)color.green << 8) | (uint32_t)color.blue;
}

static void setColorFromRGB(const HALight::RGBColor& rgb, uint32_t &store, CRGB &color, int blankMode) {
    store = valueFromRGB(rgb);
    color = CRGB(rgb.red, rgb.green, rgb.blue);
    markSettingsChanged();
    if (blankMode >= 0 && clockMode == (byte)blankMode) {
        allBlank();
    }
}

void onLightStateCommand(bool state, HALight* sender) {
    sender->setState(state);
}

void onLightRGBCommand(HALight::RGBColor color, HALight* sender) {
    if (!color.isSet) return;
    if (sender == &haHourColor) { setColorFromRGB(color, hourColorValue, hourColor, 0); }
    else if (sender == &haMinColor) { setColorFromRGB(color, minColorValue, minColor, 0); }
    else if (sender == &haColonColor) { setColorFromRGB(color, colonColorValue, colonColor, 0); }
    else if (sender == &haDayColor) { setColorFromRGB(color, dayColorValue, dayColor, 7); }
    else if (sender == &haMonthColor) { setColorFromRGB(color, monthColorValue, monthColor, 7); }
    else if (sender == &haSeparatorColor) { setColorFromRGB(color, separatorColorValue, separatorColor, 7); }
    else if (sender == &haCountdownColor) { setColorFromRGB(color, countdownColorValue, countdownColor, 1); }
    else if (sender == &haScoreLeftColor) { setColorFromRGB(color, scoreboardColorLeftValue, scoreboardColorLeft, 3); }
    else if (sender == &haScoreRightColor) { setColorFromRGB(color, scoreboardColorRightValue, scoreboardColorRight, 3); }
    else if (sender == &haScrollColor) { setColorFromRGB(color, scrollColorValue, scrollColor, 11); }
    else if (sender == &haSpotlightsColor) {
        setColorFromRGB(color, spotlightsColorValue, spotlightsColor, -1);
        ShelfDownLights();
    }
    sender->setRGBColor(color);
    sender->setState(true);
}

void onCountdownSecondsCommand(HANumeric value, HANumber* sender) {
    pendingCountdownMs = (uint32_t)value.toFloat() * 1000UL;
    sender->setState(value);
}

void onStopwatchSecondsCommand(HANumeric value, HANumber* sender) {
    pendingStopwatchMs = (uint32_t)value.toFloat() * 1000UL;
    sender->setState(value);
}

void onScoreLeftCommand(HANumeric value, HANumber* sender) {
    scoreboardLeft = (int)value.toFloat();
    sender->setState(value);
}

void onScoreRightCommand(HANumeric value, HANumber* sender) {
    scoreboardRight = (int)value.toFloat();
    sender->setState(value);
}

void onCountdownStart(HAButton* sender) {
    applyCountdownOrStopwatch(pendingCountdownMs, false);
}

void onStopwatchStart(HAButton* sender) {
    applyCountdownOrStopwatch(pendingStopwatchMs, true);
}

void onScoreboardApply(HAButton* sender) {
    applyScoreboard();
}


void setDisplayMode(String mode) {
    if (mode == "clock") {
        applyClockMode(0);
        printLocalTime();
        breakOutSet = 1;
    } else if (mode == "date") {
        applyClockMode(7);
    } else if (mode == "snake") {
        applyClockMode(0);
        printLocalTime();
        breakOutSet = 1;
    } else if (mode == "matrix") {
        clockMode = 5;
        lightshowMode = 2;
        suspendType = 0;
        isAsleep = 0;
        markSettingsChanged();  // Ensure changes are saved
    } else if (mode == "rainbow") {
        currentMode = 1;
        markSettingsChanged();
    } else if (mode == "twinkles") {
        currentMode = 2;
        markSettingsChanged();
    } else if (mode == "fire") {
        currentMode = 4;
        markSettingsChanged();
    } else if (mode == "greenmatrix") {
        currentMode = 5;
        markSettingsChanged();
    } else if (mode == "cylon") {
        currentMode = 0;
        markSettingsChanged();
    }
}

void setupHA() {
    device.setName("ShelfClock");
    device.setManufacturer("Mastershort");
    device.setModel("ESP32 LED Clock");
    device.setSoftwareVersion("0.98");

    haMode.setOptions("Clock;Countdown;Scoreboard;Stopwatch;Lightshow;Rainbows;Date;Display Off;Scroll");
    haMode.setName("01 Mode");
    haMode.onCommand(onModeCommand);
    haMqtt.addDeviceType(&haMode);

    haLightShowMode.setOptions("Chase;Twinkles;Rainbow;Matrix;Rain;Fire;Snake;Cylon");
    haLightShowMode.setName("02 Lightshow Mode");
    haLightShowMode.onCommand(onLightShowCommand);
    haMqtt.addDeviceType(&haLightShowMode);

    haPalette.setOptions("Major;Pastel");
    haPalette.setName("10 Color Palette");
    haPalette.onCommand(onPaletteCommand);
    haMqtt.addDeviceType(&haPalette);

    haColorChangeFreq.setOptions("Every Second;Every Minute;Every Hour;Every Day;Every Week;Every Month");
    haColorChangeFreq.setName("10 Color Change Freq");
    haColorChangeFreq.onCommand(onColorChangeFreqCommand);
    haMqtt.addDeviceType(&haColorChangeFreq);

    haSuspendType.setOptions("Disabled;Digits Only;All Lights");
    haSuspendType.setName("09 Suspend Type");
    haSuspendType.onCommand(onSuspendTypeCommand);
    haMqtt.addDeviceType(&haSuspendType);

    haSuspendFreq.setOptions("1;5;10;15;30;45;60");
    haSuspendFreq.setName("09 Suspend Minutes");
    haSuspendFreq.onCommand(onSuspendFreqCommand);
    haMqtt.addDeviceType(&haSuspendFreq);

    haClockDisplay.setOptions("Center 3-Digit;24h Military;12h Space-Padded;Blinking Center;New Years");
    haClockDisplay.setName("11 Clock Display");
    haClockDisplay.onCommand(onClockDisplayCommand);
    haMqtt.addDeviceType(&haClockDisplay);

    haColonType.setOptions("Colon;Bar;Period");
    haColonType.setName("11 Colon Type");
    haColonType.onCommand(onColonTypeCommand);
    haMqtt.addDeviceType(&haColonType);

    haGmtOffset.setName("11 UTC Offset");
    haGmtOffset.setMin(-43200);
    haGmtOffset.setMax(50400);
    haGmtOffset.setStep(900);
    haGmtOffset.onCommand(onGmtOffsetCommand);
    haMqtt.addDeviceType(&haGmtOffset);

    haDST.setName("11 Daylight Savings");
    haDST.onCommand(onDSTCommand);
    haMqtt.addDeviceType(&haDST);

    haClockColorSettings.setOptions("Separate Colors;Hour = Minutes;Random Each;Same Random;Random Every Second");
    haClockColorSettings.setName("11 Clock Color Mode");
    haClockColorSettings.onCommand(onClockColorSettingsCommand);
    haMqtt.addDeviceType(&haClockColorSettings);

    haDateDisplay.setOptions("Zero-Padded;Space-Padded;Center 3-Digit;Day of Week;Numeric Day;Separator;Year");
    haDateDisplay.setName("12 Date Display");
    haDateDisplay.onCommand(onDateDisplayCommand);
    haMqtt.addDeviceType(&haDateDisplay);

    haDateColorSettings.setOptions("Separate Colors;Month = Day;Random Each;Same Random;Random Every Second");
    haDateColorSettings.setName("12 Date Color Mode");
    haDateColorSettings.onCommand(onDateColorSettingsCommand);
    haMqtt.addDeviceType(&haDateColorSettings);

    haScrollFrequency.setOptions("Every Minute;Every 5 Minutes;Every 10 Minutes;Every 15 Minutes;Every 30 Minutes;Every Hour");
    haScrollFrequency.setName("16 Scroll Frequency");
    haScrollFrequency.onCommand(onScrollFrequencyCommand);
    haMqtt.addDeviceType(&haScrollFrequency);

    haScrollOpt1.setName("16 Scroll: Military Time");
    haScrollOpt1.onCommand(onScrollOptCommand);
    haMqtt.addDeviceType(&haScrollOpt1);

    haScrollOpt2.setName("16 Scroll: Day of Week");
    haScrollOpt2.onCommand(onScrollOptCommand);
    haMqtt.addDeviceType(&haScrollOpt2);

    haScrollOpt3.setName("16 Scroll: Date");
    haScrollOpt3.onCommand(onScrollOptCommand);
    haMqtt.addDeviceType(&haScrollOpt3);

    haScrollOpt4.setName("16 Scroll: Year");
    haScrollOpt4.onCommand(onScrollOptCommand);
    haMqtt.addDeviceType(&haScrollOpt4);

    haScrollOpt7.setName("16 Scroll: Text");
    haScrollOpt7.onCommand(onScrollOptCommand);
    haMqtt.addDeviceType(&haScrollOpt7);

    haScrollOpt8.setName("16 Scroll: IP Address");
    haScrollOpt8.onCommand(onScrollOptCommand);
    haMqtt.addDeviceType(&haScrollOpt8);

    haScrollColorSettings.setOptions("Selected Color;Same Random");
    haScrollColorSettings.setName("16 Scroll Color Mode");
    haScrollColorSettings.onCommand(onScrollColorSettingsCommand);
    haMqtt.addDeviceType(&haScrollColorSettings);

    haScrollOverride.setName("16 Scroll Override");
    haScrollOverride.onCommand(onScrollOverrideCommand);
    haMqtt.addDeviceType(&haScrollOverride);

    haUseSpotlights.setName("08 Spotlights Enable");
    haUseSpotlights.onCommand(onUseSpotlightsCommand);
    haMqtt.addDeviceType(&haUseSpotlights);

    haSpotlightsColorSettings.setOptions("Chosen Color;Random Color;Cycle Wheel;Cycle Slow;Twinkle Fade");
    haSpotlightsColorSettings.setName("08 Spotlights Color Mode");
    haSpotlightsColorSettings.onCommand(onSpotlightsColorSettingsCommand);
    haMqtt.addDeviceType(&haSpotlightsColorSettings);

    haColorChangeCD.setName("07 Color Change Countdown");
    haColorChangeCD.onCommand(onColorChangeCDCommand);
    haMqtt.addDeviceType(&haColorChangeCD);

    haBrightness.setName("06 Brightness");
    haBrightness.setMin(10);
    haBrightness.setMax(255);
    haBrightness.setStep(1);
    haBrightness.onCommand(onBrightnessCommand);
    haMqtt.addDeviceType(&haBrightness);

    // Color lights (RGB)
    HALight* colorLights[] = {
        &haHourColor, &haMinColor, &haColonColor, &haDayColor, &haMonthColor,
        &haSeparatorColor, &haCountdownColor, &haScoreLeftColor,
        &haScoreRightColor, &haScrollColor, &haSpotlightsColor
    };
    const char* colorNames[] = {
        "20 Color Hour", "20 Color Minute", "20 Color Colon", "20 Color Day", "20 Color Month",
        "20 Color Separator", "20 Color Countdown", "20 Color Score Left",
        "20 Color Score Right", "20 Color Scroll", "20 Color Spotlights"
    };
    for (size_t i = 0; i < sizeof(colorLights) / sizeof(colorLights[0]); i++) {
        colorLights[i]->setName(colorNames[i]);
        colorLights[i]->onRGBColorCommand(onLightRGBCommand);
        colorLights[i]->onStateCommand(onLightStateCommand);
        colorLights[i]->setCurrentState(true);
        haMqtt.addDeviceType(colorLights[i]);
    }

    haCountdownSeconds.setName("04 Countdown Seconds");
    haCountdownSeconds.setMin(1);
    haCountdownSeconds.setMax(86400);
    haCountdownSeconds.setStep(1);
    haCountdownSeconds.onCommand(onCountdownSecondsCommand);
    haMqtt.addDeviceType(&haCountdownSeconds);

    haStopwatchSeconds.setName("05 Stopwatch Seconds");
    haStopwatchSeconds.setMin(1);
    haStopwatchSeconds.setMax(86400);
    haStopwatchSeconds.setStep(1);
    haStopwatchSeconds.onCommand(onStopwatchSecondsCommand);
    haMqtt.addDeviceType(&haStopwatchSeconds);

    haScoreboardLeft.setName("04 Scoreboard Left");
    haScoreboardLeft.setMin(0);
    haScoreboardLeft.setMax(99);
    haScoreboardLeft.setStep(1);
    haScoreboardLeft.onCommand(onScoreLeftCommand);
    haMqtt.addDeviceType(&haScoreboardLeft);

    haScoreboardRight.setName("04 Scoreboard Right");
    haScoreboardRight.setMin(0);
    haScoreboardRight.setMax(99);
    haScoreboardRight.setStep(1);
    haScoreboardRight.onCommand(onScoreRightCommand);
    haMqtt.addDeviceType(&haScoreboardRight);

    haCountdownStart.setName("04 Start Countdown");
    haCountdownStart.onCommand(onCountdownStart);
    haMqtt.addDeviceType(&haCountdownStart);

    haStopwatchStart.setName("05 Start Stopwatch");
    haStopwatchStart.onCommand(onStopwatchStart);
    haMqtt.addDeviceType(&haStopwatchStart);

    haScoreboardApply.setName("04 Apply Scoreboard");
    haScoreboardApply.onCommand(onScoreboardApply);
    haMqtt.addDeviceType(&haScoreboardApply);

}

void haSyncState() {
    unsigned long now = millis();
    if (!haDirty && (now - lastHaSyncMs < 1000)) return;
    lastHaSyncMs = now;
    haDirty = false;

    haMode.setState(modeToIndex(clockMode));
    haLightShowMode.setState(lightshowMode);
    haPalette.setState(pastelColors ? 1 : 0);
    haColorChangeFreq.setState(ColorChangeFrequency);
    haSuspendType.setState(suspendType);

    static const int suspendValues[] = {1, 5, 10, 15, 30, 45, 60};
    haSuspendFreq.setState(indexOfValue(suspendValues, 7, suspendFrequency));

    haClockDisplay.setState(clockDisplayType);
    haColonType.setState(colonType);
    haGmtOffset.setState((float)gmtOffset_sec);
    haDST.setState(DSTime);
    haClockColorSettings.setState(ClockColorSettings);
    haDateDisplay.setState(dateDisplayType);
    haDateColorSettings.setState(DateColorSettings);

    static const int scrollFreqValues[] = {1, 2, 3, 4, 5, 6};
    haScrollFrequency.setState(indexOfValue(scrollFreqValues, 6, scrollFrequency));

    haScrollOpt1.setState(scrollOptions1);
    haScrollOpt2.setState(scrollOptions2);
    haScrollOpt3.setState(scrollOptions3);
    haScrollOpt4.setState(scrollOptions4);
    haScrollOpt7.setState(scrollOptions7);
    haScrollOpt8.setState(scrollOptions8);
    haScrollColorSettings.setState(scrollColorSettings);
    haScrollOverride.setState(scrollOverride);
    haUseSpotlights.setState(useSpotlights);
    haSpotlightsColorSettings.setState(spotlightsColorSettings);
    haColorChangeCD.setState(colorchangeCD);
    haBrightness.setState((float)brightness);

    haHourColor.setRGBColor(rgbFromValue(hourColorValue));
    haMinColor.setRGBColor(rgbFromValue(minColorValue));
    haColonColor.setRGBColor(rgbFromValue(colonColorValue));
    haDayColor.setRGBColor(rgbFromValue(dayColorValue));
    haMonthColor.setRGBColor(rgbFromValue(monthColorValue));
    haSeparatorColor.setRGBColor(rgbFromValue(separatorColorValue));
    haCountdownColor.setRGBColor(rgbFromValue(countdownColorValue));
    haScoreLeftColor.setRGBColor(rgbFromValue(scoreboardColorLeftValue));
    haScoreRightColor.setRGBColor(rgbFromValue(scoreboardColorRightValue));
    haScrollColor.setRGBColor(rgbFromValue(scrollColorValue));
    haSpotlightsColor.setRGBColor(rgbFromValue(spotlightsColorValue));

    haHourColor.setState(true);
    haMinColor.setState(true);
    haColonColor.setState(true);
    haDayColor.setState(true);
    haMonthColor.setState(true);
    haSeparatorColor.setState(true);
    haCountdownColor.setState(true);
    haScoreLeftColor.setState(true);
    haScoreRightColor.setState(true);
    haScrollColor.setState(true);
    haSpotlightsColor.setState(true);

    haCountdownSeconds.setState((float)(pendingCountdownMs / 1000UL));
    haStopwatchSeconds.setState((float)(pendingStopwatchMs / 1000UL));
    haScoreboardLeft.setState((float)scoreboardLeft);
    haScoreboardRight.setState((float)scoreboardRight);
}
