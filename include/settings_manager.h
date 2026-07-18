#pragma once
#include <Arduino.h>
#include <FS.h>

// --- Settings persistence ---
void saveclockSettings(String fileType);
void getclockSettings(String fileType);
void cleanupOldSettings();
void cleanupOldSettings(const String &fileType);

// --- Filesystem helpers ---
void writeFile(fs::FS &fs, const char * path, const char * message);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void deleteFile(fs::FS &fs, const char * path);

// --- Scheduler ---
void processSchedules(bool alarmType);
void createSchedulesArray();
