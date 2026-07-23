#include "../include/update_check.h"
#include "../include/globals.h"
#include "../include/display_modes.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <WiFi.h>

// GitHub repo this build checks for releases. Publish a new release with
// firmware.bin and/or littlefs.bin attached as assets, tagged "vX.Y.Z", to
// make it show up here.
static const char* UPDATE_REPO = "Mastershort/ShelfClock_Mastershort";

bool updateCheckInProgress = false;
bool updateAvailable = false;
String currentVersionStr = "";
String latestVersionStr = "";
String latestFirmwareUrl = "";
String latestFilesystemUrl = "";
String updateCheckError = "";
unsigned long lastUpdateCheckMs = 0;

// Strips a leading "version-"/"v"/"V" and any trailing non-version suffix
// (e.g. "-alpha") isn't removed - it just naturally sorts before a release
// without a suffix at the same numeric version, which is the desired
// behavior (a tagged release beats a "-alpha" dev build of the same number).
static String normalizeVersion(String v) {
  v.trim();
  if (v.startsWith("version-")) { v = v.substring(8); }
  if (v.startsWith("v") || v.startsWith("V")) { v = v.substring(1); }
  return v;
}

// Compares two "X.Y.Z[-suffix]" strings numerically part by part.
// Returns >0 if a>b, <0 if a<b, 0 if equal. A missing/non-numeric part
// counts as 0; a version WITH a "-suffix" is considered slightly older
// than the same numeric version WITHOUT one.
static int compareVersions(const String& a, const String& b) {
  int ai = 0, bi = 0;
  while (ai < (int)a.length() || bi < (int)b.length()) {
    int aPart = 0, bPart = 0;
    bool aHasSuffix = false, bHasSuffix = false;
    while (ai < (int)a.length() && isDigit(a[ai])) { aPart = aPart * 10 + (a[ai] - '0'); ai++; }
    while (bi < (int)b.length() && isDigit(b[bi])) { bPart = bPart * 10 + (b[bi] - '0'); bi++; }
    if (aPart != bPart) { return aPart - bPart; }
    if (ai < (int)a.length() && a[ai] == '.') { ai++; } else if (ai < (int)a.length()) { aHasSuffix = true; }
    if (bi < (int)b.length() && b[bi] == '.') { bi++; } else if (bi < (int)b.length()) { bHasSuffix = true; }
    if (aHasSuffix || bHasSuffix) {
      if (aHasSuffix && !bHasSuffix) { return -1; }
      if (!aHasSuffix && bHasSuffix) { return 1; }
      break;  // both have a suffix at this point - treat as equal
    }
  }
  return 0;
}

void checkForUpdate() {
  updateCheckInProgress = true;
  updateCheckError = "";
  latestFirmwareUrl = "";
  latestFilesystemUrl = "";
  currentVersionStr = normalizeVersion(softwareVersion);

  WiFiClientSecure client;
  client.setInsecure();  // no root CA pinned - see docs/ota-updates.md for the tradeoff
  HTTPClient http;
  http.setUserAgent("ShelfClock-ESP32");
  String apiUrl = String("https://api.github.com/repos/") + UPDATE_REPO + "/releases/latest";
  if (!http.begin(client, apiUrl)) {
    updateCheckError = "Verbindung zu GitHub fehlgeschlagen";
    updateCheckInProgress = false;
    lastUpdateCheckMs = millis();
    return;
  }
  http.addHeader("Accept", "application/vnd.github+json");

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    updateCheckError = (httpCode > 0) ? ("HTTP " + String(httpCode)) : http.errorToString(httpCode);
    http.end();
    updateCheckInProgress = false;
    lastUpdateCheckMs = millis();
    return;
  }

  // Only keep the fields we need - the full response (release notes etc.)
  // can be many KB and isn't worth holding in RAM
  DynamicJsonDocument filter(256);
  filter["tag_name"] = true;
  filter["assets"][0]["name"] = true;
  filter["assets"][0]["browser_download_url"] = true;

  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
  http.end();

  if (err) {
    updateCheckError = "JSON-Fehler: " + String(err.c_str());
    updateCheckInProgress = false;
    lastUpdateCheckMs = millis();
    return;
  }

  latestVersionStr = normalizeVersion(doc["tag_name"].as<String>());
  for (JsonObject asset : doc["assets"].as<JsonArray>()) {
    String name = asset["name"].as<String>();
    if (name == "firmware.bin") { latestFirmwareUrl = asset["browser_download_url"].as<String>(); }
    if (name == "littlefs.bin") { latestFilesystemUrl = asset["browser_download_url"].as<String>(); }
  }

  updateAvailable = latestVersionStr.length() > 0 && compareVersions(latestVersionStr, currentVersionStr) > 0;
  updateCheckInProgress = false;
  lastUpdateCheckMs = millis();
  webLog("Update check: current=" + currentVersionStr + " latest=" + latestVersionStr + " available=" + String(updateAvailable));
}

static void updateCheckTask(void* param) {
  checkForUpdate();
  vTaskDelete(NULL);
}

void checkForUpdateAsync() {
  if (updateCheckInProgress || WiFi.status() != WL_CONNECTED) { return; }
  updateCheckInProgress = true;  // set here already so a second call can't race a task into existing
  xTaskCreate(updateCheckTask, "updateCheck", 8192, NULL, 1, NULL);
}

bool installUpdateFromUrl(const String& url, int updateType) {
  if (url.length() == 0) { return false; }
  otaInProgress = true;  // stop loop() from driving LEDs/filesystem during the flash write
  allBlank();

  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.setUserAgent("ShelfClock-ESP32");
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);  // GitHub asset URLs redirect to objects.githubusercontent.com
  if (!http.begin(client, url)) {
    webLog("Update install: connection failed");
    otaInProgress = false;
    return false;
  }

  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    webLog("Update install: HTTP " + String(httpCode));
    http.end();
    otaInProgress = false;
    return false;
  }

  int len = http.getSize();
  if (updateType == U_SPIFFS) { FileFS.end(); }  // unmount before the partition gets overwritten

  bool ok = false;
  if (Update.begin(len > 0 ? len : UPDATE_SIZE_UNKNOWN, updateType)) {
    size_t written = Update.writeStream(*http.getStreamPtr());
    if ((len <= 0 || written == (size_t)len) && Update.end(true)) {
      ok = true;
    } else {
      Update.printError(Serial);
    }
  } else {
    Update.printError(Serial);
  }
  http.end();

  if (ok) {
    webLog("Update installed successfully, restarting...");
    delay(500);
    ESP.restart();
  } else {
    webLog("Update install failed");
    if (updateType == U_SPIFFS) { FileFS.begin(); }  // remount the (unchanged) filesystem
    otaInProgress = false;
  }
  return ok;
}
