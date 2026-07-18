#include "../include/globals.h"
#include "../include/settings_manager.h"
#include "../include/display_modes.h"
#include "../include/prefs.h"

void writeFile(fs::FS &fs, const char * path, const char * message){
   Serial.printf("Writing file: %s\r\n", path);
   File file = fs.open(path, FILE_WRITE);
   if(!file){
      Serial.println("− failed to open file for writing");
      return;
   }
   if(file.print(message)){
      Serial.println("− file written");
   }else {
      Serial.println("− frite failed");
   }
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
   Serial.printf("Listing directory: %s\r\n", dirname);
   File root = fs.open(dirname);
   if(!root){
      Serial.println("− failed to open directory");
      return;
   }
   if(!root.isDirectory()){
      Serial.println(" − not a directory");
      return;
   }
   File file = root.openNextFile();
   while(file){
      if(file.isDirectory()){
         Serial.print("  DIR : ");
         Serial.println(file.name());
         if(levels){
            listDir(fs, file.name(), levels -1);
         }
      } else {
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("\tSIZE: ");
         Serial.println(file.size());
      }
      file = root.openNextFile();
   }
}

void deleteFile(fs::FS &fs, const char * path){
   Serial.printf("Deleting file: %s\r\n", path);
   if(fs.remove(path)){
      Serial.println("− file deleted");
   } else {
      Serial.println("− delete failed");
   }
}

void processSchedules(bool alarmType) {
    int fileChanged = 0;
    int hour = timeinfo.tm_hour;
    int mins = timeinfo.tm_min;
    int minsSixty = 101;
    if (mins == 0) {minsSixty = 60;}
    else if (mins == 45) {minsSixty = 45;}
    else if (mins == 30) {minsSixty = 30;}
    else if (mins == 15) {minsSixty = 15;}
    int dayOfweek = timeinfo.tm_wday;
    int mday = timeinfo.tm_mday;
    int mont = timeinfo.tm_mon + 1;
    int years = (timeinfo.tm_year +1900);
    int currentEOM = 0;
    int object_count = jsonScheduleData.size();
    for (int i = 0; i < (object_count); i++) {
      JsonObject jsonObj = jsonScheduleData[i];
      int scheduleType = (int)jsonObj["scheduleType"];
      int mon = (int)jsonObj["mon"];
      int tue = (int)jsonObj["tue"];
      int wed = (int)jsonObj["wed"];
      int thu = (int)jsonObj["thu"];
      int fri = (int)jsonObj["fri"];
      int sat = (int)jsonObj["sat"];
      int sun = (int)jsonObj["sun"];
      int dailyType = (int)jsonObj["dailyType"];
      int dom = (int)jsonObj["dom"];
      int hours = (int)jsonObj["hours"];
      int minutes = (int)jsonObj["minutes"];
      String song = jsonObj["song"].as<String>();
      String title = jsonObj["title"].as<String>();
      int hourlyType = (int)jsonObj["hourlyType"];
      int gongmode = (int)jsonObj["gongmode"];
      int daymode = (int)jsonObj["daymode"];
      int year = (int)jsonObj["year"];
      int month = (int)jsonObj["month"];
      int day = (int)jsonObj["day"];
      int recurring = (int)jsonObj["recurring"];
      String uid = jsonObj["uid"].as<String>();

   if (scheduleType == 0)  {
      if ((dayOfweek == 0 && sun == 1) || (dayOfweek == 1 && mon == 1) || (dayOfweek == 2 && tue == 1) || (dayOfweek == 3 && wed == 1) || (dayOfweek == 4 && thu == 1) || (dayOfweek == 5 && fri == 1) || (dayOfweek == 6 && sat == 1)) {
        if (((daymode == 1) && ((hour >= 8) && (hour <=19 ))) || (daymode == 0)) {
          if (((dailyType == 1) && ((hours == hour && minutes == mins) || (hours == 99 && minutes == mins) || (hours == hour && mins == 99 && minutes == 0))) || (((hourlyType == 15) && (minsSixty % 15 == 0)) || ((hourlyType == 30) && (minsSixty % 30 == 0)) || ((hourlyType == 60) && (minsSixty % 60 == 0))) || ((dailyType == 0) && ((hourlyType == 99) && (minsSixty % 60 == 0)))) {
          }
        }
      }
   }

   if (scheduleType == 1)  {
    if (mont == 2) {
      if (years % 4 == 0) {
        if (years % 100 == 0) {
          if (years % 400 == 0) { currentEOM = 29; } else { currentEOM = 28; }
        } else { currentEOM = 29; }
      } else { currentEOM = 28; }
    } else if (mont == 4 || mont == 6 || mont == 9 || mont == 11) { currentEOM = 30; }
    else { currentEOM = 31; }
    if ((dom == mday) || (dom == 34 && (mday == currentEOM)) || (dom == 33 && (mday == (currentEOM-1))) || (dom == 32 && (mday == (currentEOM-2))))  {
      if (((hours == hour && minutes == mins) || (hours == 99 && minutes == mins) || (hours == hour && mins == 99 && minutes == 0))) {
              if (!breakOutSet) {scroll(title);}
              allBlank();
      }
    }
   }

   if (scheduleType == 2)  {
        if (mont == month && mday == day && hours == hour && minutes == mins) {
          if (!recurring && year == years) {
            char processedName[40];
            char processedText[64];
            strcpy(processedText, "/scheduler/");
            strcpy(processedName, jsonObj["uid"]);
            strcat(processedText, processedName);
            strcat(processedText, ".json");
            Serial.println(uid);
            Serial.println(processedText);
            deleteFile(FileFS, processedText);
            fileChanged = 1;
          }
          if (!breakOutSet) {scroll(title);}
          allBlank();
        }
   }

   if (scheduleType == 3)  {
     if (mont == month && mday == day) {
          Serial.println("special alarm");
        if (hour == 12 && mins == 0) {
          if (!breakOutSet) {scroll(title);}
          allBlank();
        }
     }
   }

   if (scheduleType == 4)  {
    if (mont == 01 && mday == 01 && hour == 0 && mins == 0) {
          if (!breakOutSet) {scroll(title);}
          allBlank();
    }
   }

}
  if (fileChanged) { createSchedulesArray(); }
}

void createSchedulesArray() {
  JsonArray dataArray = jsonScheduleData.to<JsonArray>(); // create a JSON array to store the data
  File dir = FileFS.open("/scheduler"); // open the directory containing the text files
  if (!dir) {
    Serial.println("Failed to open /scheduler directory");
    return;
  }
  while (true) { // loop through each file in the directory
    File file = dir.openNextFile();
    if (!file) break; // end the loop when there are no more files
    String fileName = file.name();
    if (fileName.endsWith(".json")) { // only read files that have a .txt extension
      String fileContent = file.readString(); // read the contents of the file
      JsonObject fileData = dataArray.createNestedObject(); // create a JSON object to store the file data
      DynamicJsonDocument jsonScheduleDataTemp(1024);
      DeserializationError error = deserializeJson(jsonScheduleDataTemp, fileContent);
      if (error) {
        Serial.println("Failed to parse JSON file");
        return;
      }
      // Get the values from the JSON object
      JsonObject jsonObj = jsonScheduleDataTemp.as<JsonObject>();
      int scheduleType = jsonObj["scheduleType"];
      int mon = jsonObj["mon"];
      int tue = jsonObj["tue"];
      int wed = jsonObj["wed"];
      int thu = jsonObj["thu"];
      int fri = jsonObj["fri"];
      int sat = jsonObj["sat"];
      int sun = jsonObj["sun"];
      int dailyType = jsonObj["dailyType"];
      int dom = jsonObj["dom"];
      int hours = jsonObj["hours"];
      int minutes = jsonObj["minutes"];
      String song = jsonObj["song"];
      String title = jsonObj["title"];
      int hourlyType = jsonObj["hourlyType"];
      int gongmode = jsonObj["gongmode"];
      int daymode = jsonObj["daymode"];
      int year = jsonObj["year"];
      int month = jsonObj["month"];
      int day = jsonObj["day"];
      int recurring = jsonObj["recurring"];
      String uid = jsonObj["uid"];
      /* Display the values for testing
      Serial.print("scheduleType = ");
      Serial.println(scheduleType);
      Serial.print("mon = ");
      Serial.println(mon);
      Serial.print("tue = ");
      Serial.println(tue);
      Serial.print("wed = ");
      Serial.println(wed);
      Serial.print("thu = ");
      Serial.println(thu);
      Serial.print("fri = ");
      Serial.println(fri);
      Serial.print("sat = ");
      Serial.println(sat);
      Serial.print("sun = ");
      Serial.println(sun);
      Serial.print("dailyType = ");
      Serial.println(dailyType);
      Serial.print("dom = ");
      Serial.println(dom);
      Serial.print("hours = ");
      Serial.println(hours);
      Serial.print("minutes = ");
      Serial.println(minutes);
      Serial.print("song = ");
      Serial.println(song);
      Serial.print("title = ");
      Serial.println(title);
      Serial.print("hourlyType = ");
      Serial.println(hourlyType);
      Serial.print("gongmode = ");
      Serial.println(gongmode);
      Serial.print("daymode = ");
      Serial.println(daymode);
      Serial.print("year = ");
      Serial.println(year);
      Serial.print("month = ");
      Serial.println(month);
      Serial.print("day = ");
      Serial.println(day);
      Serial.print("recurring = ");
      Serial.println(recurring);
      Serial.print("uid = ");
      Serial.println(uid);
      */
      fileData["scheduleType"] = scheduleType;
      fileData["mon"] = mon;
      fileData["tue"] = tue;
      fileData["wed"] = wed;
      fileData["thu"] = thu;
      fileData["fri"] = fri;
      fileData["sat"] = sat;
      fileData["sun"] = sun;
      fileData["dailyType"] = dailyType;
      fileData["dom"] = dom;
      fileData["hours"] = hours;
      fileData["minutes"] = minutes;
      fileData["song"] = song;
      fileData["title"] = title;
      fileData["hourlyType"] = hourlyType;
      fileData["gongmode"] = gongmode;
      fileData["daymode"] = daymode;
      fileData["year"] = year;
      fileData["month"] = month;
      fileData["day"] = day;
      fileData["recurring"] = recurring;
      fileData["uid"] = uid;
    }
    file.close();
  }
  serializeJson(jsonScheduleData, Serial); // output the entire JSON array to the console
  Serial.println("");
}

// Parses a clock settings JSON string and applies it to the globals.
// Returns false if the JSON cannot be parsed.
static bool applyClockSettingsJson(const String& fileContent) {
  DynamicJsonDocument jsonDocu(8192);
  DeserializationError error = deserializeJson(jsonDocu, fileContent);
  if (error) {
    Serial.println("Failed to parse settings JSON");
    return false;
  }
  JsonObject jsonObj = jsonDocu.as<JsonObject>();
  ClockColorSettings = jsonObj["ClockColorSettings"].as<byte>();
  ColorChangeFrequency = jsonObj["ColorChangeFrequency"].as<byte>();
  DSTime = jsonObj["DSTime"].as<bool>();
  DateColorSettings = jsonObj["DateColorSettings"].as<byte>();
  brightness = jsonObj["brightness"].as<byte>();
  countdownColorValue = jsonObj["countdownColor"].as<uint32_t>();
  countdownColor      = CRGB(countdownColorValue);
  clockDisplayType = jsonObj["clockDisplayType"].as<byte>();
  clockMode = jsonObj["clockMode"].as<byte>();
  colonType = jsonObj["colonType"].as<int>();
  colorchangeCD = jsonObj["colorchangeCD"].as<bool>();
  dateDisplayType = jsonObj["dateDisplayType"].as<byte>();
  gmtOffset_sec = jsonObj["gmtOffset_sec"].as<long>();
  if (!jsonObj["timeZone"].isNull()) { timeZone = jsonObj["timeZone"].as<String>(); }  // keep default when loading pre-timezone settings
  lightshowMode = jsonObj["lightshowMode"].as<int>();
  pastelColors = jsonObj["pastelColors"].as<byte>();
  spotlightsColorValue = jsonObj["spotlightsColor"].as<uint32_t>();
  spotlightsColor      = CRGB(spotlightsColorValue);
  scoreboardColorLeftValue = jsonObj["scoreboardColorLeft"].as<uint32_t>();
  scoreboardColorLeft      = CRGB(scoreboardColorLeftValue);
  scoreboardColorRightValue = jsonObj["scoreboardColorRight"].as<uint32_t>();
  scoreboardColorRight      = CRGB(scoreboardColorRightValue);
  scrollColorValue = jsonObj["scrollColor"].as<uint32_t>();
  scrollColor      = CRGB(scrollColorValue);
  hourColorValue = jsonObj["hourColor"].as<uint32_t>();
  hourColor      = CRGB(hourColorValue);
  minColorValue = jsonObj["minColor"].as<uint32_t>();
  minColor      = CRGB(minColorValue);
  colonColorValue = jsonObj["colonColor"].as<uint32_t>();
  colonColor      = CRGB(colonColorValue);
  dayColorValue = jsonObj["dayColor"].as<uint32_t>();
  dayColor      = CRGB(dayColorValue);
  monthColorValue = jsonObj["monthColor"].as<uint32_t>();
  monthColor      = CRGB(monthColorValue);
  separatorColorValue = jsonObj["separatorColor"].as<uint32_t>();
  separatorColor      = CRGB(separatorColorValue);
  realtimeMode = jsonObj["realtimeMode"].as<int>();
  scrollColorSettings = jsonObj["scrollColorSettings"].as<int>();
  scrollFrequency = jsonObj["scrollFrequency"].as<int>();
  scrollOptions1 = jsonObj["scrollOptions1"].as<bool>();
  scrollOptions2 = jsonObj["scrollOptions2"].as<bool>();
  scrollOptions3 = jsonObj["scrollOptions3"].as<bool>();
  scrollOptions4 = jsonObj["scrollOptions4"].as<bool>();
  scrollOptions7 = jsonObj["scrollOptions7"].as<bool>();
  scrollOptions8 = jsonObj["scrollOptions8"].as<bool>();
  scrollOverride = jsonObj["scrollOverride"].as<bool>();
  scrollText = jsonObj["scrollText"].as<String>();
  spotlightsColorSettings = jsonObj["spotlightsColorSettings"].as<int>();
  suspendFrequency = jsonObj["suspendFrequency"].as<int>();
  suspendType = jsonObj["suspendType"].as<byte>();
  useSpotlights = jsonObj["useSpotlights"].as<bool>();
  return true;
}

void getclockSettings(String fileType) {
  // The NVS backup survives filesystem updates (littlefs.bin overwrites the
  // settings file with defaults) - for the main settings it is the freshest
  // copy, so prefer it over the file
  if (fileType == "generic") {
    String backup = prefGetStr("settingsBak", "");
    if (backup.length() > 0 && applyClockSettingsJson(backup)) {
      Serial.println("Settings restored from NVS backup");
      // re-sync the file only when it differs (e.g. right after a filesystem update)
      String fileContent = "";
      if (FileFS.exists("/settings/clockSettings-generic.json")) {
        File f = FileFS.open("/settings/clockSettings-generic.json");
        if (f) { fileContent = f.readString(); f.close(); }
      }
      if (fileContent != backup) { updateSettingsRequired = 1; }
      return;
    }
  }

  char new_filename[50];
  sprintf(new_filename, "/settings/clockSettings-%s.json", fileType); //build filename and path from argument
  Serial.print("Opening ");
  Serial.println(new_filename);

  // Check if file exists
  if (!FileFS.exists(new_filename)) {
    Serial.println("Settings file not found - using defaults");
    return;
  }

  File file = FileFS.open(new_filename);
  if (!file) {
    Serial.println("Failed to open settings file");
    return;
  }

  String fileContent = file.readString(); // read the contents of the file
  file.close();
  Serial.println(fileContent);
  applyClockSettingsJson(fileContent);
}

void cleanupOldSettings(const String &fileType) {
  const char *folder = "/settings";
  File dir = FileFS.open(folder);
  if (!dir || !dir.isDirectory()) {
    Serial.println("cleanup: cannot open /settings");
    return;
  }

  String prefix = "clockSettings-" + fileType + ".json.";
  std::vector<String> toDelete;

  // 1) gather names
  File entry = dir.openNextFile();
  while (entry) {
    String full = entry.name();
    // strip leading path, if any
    int slash = full.lastIndexOf('/');
    String fname = (slash >= 0) ? full.substring(slash + 1) : full;

    if (fname.startsWith(prefix)) {
      toDelete.push_back(String(folder) + "/" + fname);
    }
    entry.close();                  // close this file descriptor
    entry = dir.openNextFile();
  }
  dir.close();                      // close the directory now

  // 2) remove them once everything's closed
  for (auto &path : toDelete) {
    Serial.println("Removing old backup: " + path);
    if (!FileFS.remove(path)) {
      Serial.println("failed to remove: " + path);
    }
  }
}

void saveclockSettings(String fileType) {
  // build names
  char newName[64], oldName[64];
  sprintf(newName, "/settings/clockSettings-%s.json.%05d",
          fileType.c_str(), random(0, 65535));
  sprintf(oldName, "/settings/clockSettings-%s.json", fileType.c_str());

  // Ensure settings directory exists (prevents creation errors on fresh FS)
  {
    File dir = FileFS.open("/settings");
    if (!dir || !dir.isDirectory()) {
      FileFS.mkdir("/settings");
    }
    if (dir) dir.close();
  }

  // If an existing preset file is present, back it up first; otherwise skip rename to avoid errors
  if (FileFS.exists(oldName)) {
    Serial.println("Backing up existing to " + String(newName));
    if (!FileFS.rename(oldName, newName)) {
      Serial.println("Rename failed (low space?), cleaning old backups…");
      cleanupOldSettings(fileType);
      // retry once
      FileFS.rename(oldName, newName);
    }
  } else {
    Serial.println("No existing preset, creating new: " + String(oldName));
  }

  // 2) serialize your JSON into a heap doc
  DynamicJsonDocument doc(8192);
  JsonObject clockSettings = doc.to<JsonObject>();
  // Add the values to the JSON object
  clockSettings["gmtOffset_sec"] = gmtOffset_sec;
  clockSettings["DSTime"] = DSTime;
  clockSettings["timeZone"] = timeZone;
  clockSettings["countdownColor"] = countdownColorValue;
  clockSettings["spotlightsColor"] = spotlightsColorValue;
  clockSettings["hourColor"] = hourColorValue;
  clockSettings["minColor"] = minColorValue;
  clockSettings["colonColor"] = colonColorValue;
  clockSettings["dayColor"] = dayColorValue;
  clockSettings["monthColor"] = monthColorValue;
  clockSettings["separatorColor"] = separatorColorValue;
  clockSettings["scoreboardColorLeft"] = scoreboardColorLeftValue;
  clockSettings["scoreboardColorRight"] = scoreboardColorRightValue;
  clockSettings["scrollColor"] = scrollColorValue;
  clockSettings["clockMode"] = clockMode;
  clockSettings["pastelColors"] = pastelColors;
  clockSettings["ClockColorSettings"] = ClockColorSettings;
  clockSettings["DateColorSettings"] = DateColorSettings;
  clockSettings["colonType"] = colonType;
  clockSettings["ColorChangeFrequency"] = ColorChangeFrequency;
  clockSettings["scrollText"] = scrollText;
  clockSettings["clockDisplayType"] = clockDisplayType;
  clockSettings["dateDisplayType"] = dateDisplayType;
  clockSettings["colorchangeCD"] = colorchangeCD;
  clockSettings["realtimeMode"] = realtimeMode;
  clockSettings["spotlightsColorSettings"] = spotlightsColorSettings;
  clockSettings["brightness"] = brightness;
  clockSettings["useSpotlights"] = useSpotlights;
  clockSettings["scrollColorSettings"] = scrollColorSettings;
  clockSettings["scrollFrequency"] = scrollFrequency;
  clockSettings["scrollOverride"] = scrollOverride;
  clockSettings["scrollOptions1"] = scrollOptions1;
  clockSettings["scrollOptions2"] = scrollOptions2;
  clockSettings["scrollOptions3"] = scrollOptions3;
  clockSettings["scrollOptions4"] = scrollOptions4;
  clockSettings["scrollOptions7"] = scrollOptions7;
  clockSettings["scrollOptions8"] = scrollOptions8;
  clockSettings["lightshowMode"] = lightshowMode;
  clockSettings["suspendFrequency"] = suspendFrequency;
  clockSettings["suspendType"] = suspendType;


// 3) write it out
File file = FileFS.open(oldName, FILE_WRITE);
if (!file) {
  Serial.println("Open for write failed, cleaning backups and retrying…");
  cleanupOldSettings(fileType);
  file = FileFS.open(oldName, FILE_WRITE);
  if (!file) {
    Serial.println("Still can't open file. Giving up.");
    return;
  }
}

serializeJson(doc, file);
file.close();

// Keep a copy of the main settings in NVS - survives filesystem updates
if (fileType == "generic") {
  String backup;
  serializeJson(doc, backup);
  prefPutStr("settingsBak", backup);
}
Serial.println("Settings saved successfully. Old backups (if any) removed.");
}
