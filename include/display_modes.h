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

// --- Scrolling (non-blocking state machine) ---
void scroll(String text);          // blocking wrapper, only for setup()
void startScroll(String text);     // queue text, rendered via scrollTick()
void startScrollColored(String text, CRGB color, int repeat);
void scrollTick();                 // call every loop() pass
void stopScroll();
extern bool scrollActive;          // true while a scroll is running
CRGB colorWheel(int pos);
CRGB colorWheel2(int pos);
