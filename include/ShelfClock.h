#pragma once
#include "Arduino.h"
#include "WString.h"
#include <FastLED.h>
#include <FS.h>



void loadWebPageHandlers();
void loadSetupSettings();
void fakeClock(int);
void printLocalTime();
void initGreenMatrix();
void raininit();
void allBlank();
void applyBrightness();
void checkSleepTimer();
void displayTimeMode();
void displayCountdownMode();
void displayScoreboardMode();
void displayStopwatchMode();
void displayLightshowMode(); 
void displayDateMode();
void displayScrollMode();
void ShelfDownLights();
void displayRealtimeMode();
void scroll(String);
void displayNumber(uint16_t number, byte segment, CRGB color);
void BlinkDots();
void happyNewYear();
void endCountdown();
void Chase();
void Twinkles();
void Rainbow();
void GreenMatrix();
void Fire2021();
void blueRain();
void Snake();
void Cylon();
CRGB colorWheel(int);
CRGB colorWheel2(int);
void updateMatrix();
void changeMatrixpattern();
void updaterain();
void writeFile(fs::FS &fs, const char * path, const char * message);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void deleteFile(fs::FS &fs, const char * path);
void processSchedules(bool alarmType);
void createSchedulesArray();
void saveclockSettings(String fileType);
void getclockSettings(String fileType);
void allTest();
void cleanupOldSettings();
void cleanupOldSettings(const String &fileType);             // new behavior
