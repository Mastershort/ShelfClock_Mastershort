#include "../include/prefs.h"

extern Preferences preferences;

void setupPrefs() {
    if (!preferences.begin("ShelfClock", false)) {
        Serial.println("Failed to initialize Preferences!");
    } else {
        Serial.println("Preferences initialized.");
    }
   // hourColorValue = prefGetU32("hourColor", 0xFF0000);  // Default: rot

}

// bool
bool prefGetBool(const char* key, bool deflt) {
    return preferences.getBool(key, deflt);
}
void prefPutBool(const char* key, bool value) {
    preferences.putBool(key, value);
}

// int32 (z. B. brightness, modes)
int32_t prefGetInt(const char* key, int32_t deflt) {
    return preferences.getInt(key, deflt);
}
void prefPutInt(const char* key, int32_t value) {
    preferences.putInt(key, value);
}

// Farben als uint32 (0xRRGGBB)
uint32_t prefGetU32(const char* key, uint32_t deflt) {
    return preferences.getUInt(key, deflt);
}
void prefPutU32(const char* key, uint32_t value) {
    preferences.putUInt(key, value);
}

// Strings (MQTT-Server etc.)
String prefGetStr(const char* key, const String& deflt) {
    return preferences.getString(key, deflt);
}
void prefPutStr(const char* key, const String& value) {
    preferences.putString(key, value);
}
