#pragma once
#include <Arduino.h>

// --- GitHub-based OTA update check ---
// Checks https://api.github.com/repos/<repo>/releases/latest, compares the
// tag against the compiled-in softwareVersion, and if a firmware.bin /
// littlefs.bin asset is attached, offers a one-click install sourced
// directly from that release (no manual file upload needed).

void checkForUpdate();                          // blocking HTTPS call (~1-3s) - fine for a button click
void checkForUpdateAsync();                     // runs checkForUpdate() on its own task - never blocks loop()
bool installUpdateFromUrl(const String& url, int updateType);  // U_FLASH or U_SPIFFS

// --- State (read on the settings page / HA sensor) ---
extern bool updateCheckInProgress;
extern bool updateAvailable;
extern String currentVersionStr;    // softwareVersion, normalized (leading "version-"/"v" stripped)
extern String latestVersionStr;
extern String latestFirmwareUrl;    // empty if the release has no firmware.bin asset
extern String latestFilesystemUrl;  // empty if the release has no littlefs.bin asset
extern String updateCheckError;     // empty if the last check succeeded
extern unsigned long lastUpdateCheckMs;
