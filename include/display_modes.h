#pragma once
#include <Arduino.h>
#include <FastLED.h>

// --- Display mode functions ---
void displayTimeMode();
void displayDateMode();
void displayScrollMode();
void displayCountdownMode();
void displayStopwatchMode();
void displayScoreboardMode();
void displayHAValueMode();
void displayLightshowMode();
void displayRealtimeMode();

// --- Core display helpers ---
void displayNumber(uint16_t number, byte segment, CRGB color);
void allBlank();
void allTest();
void fakeClock(int loopy);
void applyBrightness();
void printLocalTime();

// --- Display sub-functions ---
void BlinkDots();
void ShelfDownLights();
void endCountdown();
void checkSleepTimer();
void scroll(String text);
CRGB colorWheel(int pos);
CRGB colorWheel2(int pos);
