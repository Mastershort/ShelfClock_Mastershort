#ifndef PREFS_H
#define PREFS_H

#include <Preferences.h>
#include <Arduino.h>  // Für String, uint32_t etc.

extern Preferences preferences;

void setupPrefs();

// bool
bool prefGetBool(const char* key, bool deflt);
void prefPutBool(const char* key, bool value);

// int32
int32_t prefGetInt(const char* key, int32_t deflt);
void prefPutInt(const char* key, int32_t value);

// Farben als uint32
uint32_t prefGetU32(const char* key, uint32_t deflt);
void prefPutU32(const char* key, uint32_t value);

// Strings
String prefGetStr(const char* key, const String& deflt);
void prefPutStr(const char* key, const String& value);

#endif
