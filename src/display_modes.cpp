#include "../include/globals.h"
#include "../include/display_modes.h"
#include "../include/lightshow.h"

void displayTimeMode() {  //main clock function
  currentMode = 0;
	if(!getLocalTime(&timeinfo)){
	  Serial.println("Failed to obtain time");
	}
	int hour = timeinfo.tm_hour;
	int mins = timeinfo.tm_min;
	int secs = timeinfo.tm_sec;

  if (clockDisplayType != 1 && hour > 12){hour = hour - 12; }
  if (clockDisplayType != 1 && hour < 1){hour = hour + 12; }
	byte h1 = hour / 10;
	byte h2 = hour % 10;
	byte m1 = mins / 10;
	byte m2 = mins % 10;
	byte s1 = secs / 10;
	byte s2 = secs % 10;

  if (ClockColorSettings == 0) { hourColor = CRGB(hourColorValue);  minColor = CRGB(minColorValue); }
  if (ClockColorSettings == 1) { hourColor = CRGB(hourColorValue);  minColor = hourColor; }
  if ((ClockColorSettings == 2 && pastelColors == 0)  && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { hourColor = CHSV(random(0, 255), 255, 255);  minColor = CHSV(random(0, 255), 255, 255);}
  if ((ClockColorSettings == 2 && pastelColors == 1)  && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { hourColor = CRGB(random(0, 255), random(0, 255), random(0, 255));  minColor = CRGB(random(0, 255), random(0, 255), random(0, 255));}
  if ((ClockColorSettings == 3 && pastelColors == 0) && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { hourColor = CHSV(random(0, 255), 255, 255);  minColor = hourColor;}
  if ((ClockColorSettings == 3 && pastelColors == 1) && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { hourColor = CRGB(random(0, 255), random(0, 255), random(0, 255));  minColor = hourColor;}
  if ((clockDisplayType == 3)) {          //Clock for Blinking Center Light
  	if (h1 > 0) {
      tinyhourColor = hourColor;
      if (ClockColorSettings == 4 && pastelColors == 0){ tinyhourColor = CHSV(random(0, 255), 255, 255); }
      if (ClockColorSettings == 4 && pastelColors == 1){ tinyhourColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=(32*LEDS_PER_SEGMENT); i<(33*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinyhourColor;}
      if (ClockColorSettings == 4 && pastelColors == 0){ tinyhourColor = CHSV(random(0, 255), 255, 255); }
      if (ClockColorSettings == 4 && pastelColors == 1){ tinyhourColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=(33*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinyhourColor;}
  	} else {
  	    for (int i=(32*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black;}
  	  }
      displayNumber(h2,5,hourColor);
      displayNumber(m1,2,minColor);
      displayNumber(m2,0,minColor);
      BlinkDots();
  }

  if (clockDisplayType == 2) {  //12-hour Space-Padded
    if (h1 < 1) { displayNumber(10,6,hourColor); }
    else { displayNumber(h1,6,hourColor); }
      displayNumber(h2,4,hourColor);
      displayNumber(m1,2,minColor);
      displayNumber(m2,0,minColor);
	  BlinkDots();
  }
  if (clockDisplayType == 0) {  //center set and hour is less than 1 and no 0 is set, default
  if (h1 > 0) {
    tinyhourColor = hourColor;
    if (ClockColorSettings == 4 && pastelColors == 0){ tinyhourColor = CHSV(random(0, 255), 255, 255); }
    if (ClockColorSettings == 4 && pastelColors == 1){ tinyhourColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(32*LEDS_PER_SEGMENT); i<(33*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinyhourColor;}
    if (ClockColorSettings == 4 && pastelColors == 0){ tinyhourColor = CHSV(random(0, 255), 255, 255); }
    if (ClockColorSettings == 4 && pastelColors == 1){ tinyhourColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(33*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinyhourColor;}
  } else {for (int i=(32*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black;}
   }
    	displayNumber(h2,5,hourColor);
    	displayNumber(m1,3,minColor);
    	displayNumber(m2,1,minColor);
  }
  if (clockDisplayType == 1) {     //24-hour Military Time
    if (h1 < 1) { displayNumber(0,6,hourColor);}
    else  { displayNumber(h1,6,hourColor);}
    displayNumber(h2,4,hourColor);
    displayNumber(m1,2,minColor);
    displayNumber(m2,0,minColor);
	  BlinkDots();
  }

  if (clockDisplayType == 4) {     //New Year Countdown
    int hour = timeinfo.tm_hour;
    int mins = timeinfo.tm_min;
    int secs = timeinfo.tm_sec;
    int mday = timeinfo.tm_mday;
    int mont = timeinfo.tm_mon + 1;
    int year = (timeinfo.tm_year +1900);
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int daysLeft = 0;
    if (((year % 4 == 0) && (year % 100 != 0)) || ((year % 100 != 0) && (year % 400 == 0))){daysLeft = daysLeft + 1;}
    if (mont !=12) {for (int i=(mont+1); i<13; i++) { daysLeft = daysLeft + daysInMonth[i-1];}}
    daysLeft = daysLeft + (daysInMonth[mont-1]-mday);
    int hoursLeft = (daysLeft * 24) + (23 - hour);
    bool DST;
    int y = year-2000;
    int x = (y + y/4 + 2) % 7;
    if(mont == 3 && mday == (14 - x) && hour >= 2){DST = 1;}
    if((mont == 3 && mday > (14 - x)) || mont > 3){DST = 1;}
    if(mont == 11 && mday == (7 - x) && hour >= 2){DST = 0; }
    if((mont == 11 && mday > (7 - x)) || mont > 11 || mont < 3){DST = 0;}
    if(DST == 1) {hoursLeft = hoursLeft + 1; }
    int minutesLeft = (hoursLeft * 60) + (59 - mins);
    int secondsLeft = (minutesLeft * 60) + (60 - secs);
    int inputNums = hoursLeft;
    if ((minutesLeft <= 9999) && (secondsLeft > 9999)) {inputNums = minutesLeft; hourColor = minColor;}
    if ((secondsLeft <= 9999) && (secondsLeft > 10)) {inputNums = secondsLeft; hourColor = colonColor;}
    if (secondsLeft <= 10) {inputNums = secondsLeft;FastLED.setBrightness(255);hourColor = CRGB::Red;}
    byte ledNum1 = inputNums / 1000;
    byte ledNum2 = (inputNums - (ledNum1 * 1000)) / 100;
    byte ledNum3 = ((inputNums - (ledNum1 * 1000)) - (ledNum2 * 100)) / 10;
    byte ledNum4 = inputNums % 10;

    if (inputNums >= 1000) {
      if (clearOldLeds != 1000){clearOldLeds = 1000; allBlank();}
      displayNumber(ledNum1,6,hourColor);
      displayNumber(ledNum2,4,hourColor);
      displayNumber(ledNum3,2,hourColor);
      displayNumber(ledNum4,0,hourColor);
    }
    if ((inputNums >= 100) && (inputNums < 1000)) {
      if (clearOldLeds != 100){clearOldLeds = 100; allBlank();}
      displayNumber(ledNum2,5,hourColor);
      displayNumber(ledNum3,3,hourColor);
      displayNumber(ledNum4,1,hourColor);
    }
    if ((inputNums > 0) && (inputNums < 100)) {
      if (clearOldLeds != 10){clearOldLeds = 10; allBlank();}
      displayNumber(ledNum3,4,hourColor);
      displayNumber(ledNum4,2,hourColor);
    }
    }
} // end of update clock


void displayDateMode() {  //main date function
  currentMode = 0;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
  }
  int mday = timeinfo.tm_mday;
  int mont = timeinfo.tm_mon + 1;
  int year = (timeinfo.tm_year +1900)-2000;
  byte d1 = mday / 10;
  byte d2 = mday % 10;
  byte m1 = mont / 10;
  byte m2 = mont % 10;
  byte y1 = year / 10;
  byte y2 = year % 10;
  if (DateColorSettings == 0) { monthColor = CRGB(monthColorValue);  dayColor = CRGB(dayColorValue); separatorColor = CRGB(separatorColorValue);}
  if (DateColorSettings == 1) { monthColor = CRGB(monthColorValue);  dayColor = monthColor; separatorColor = CRGB(separatorColorValue);}
  if ((DateColorSettings == 2 && pastelColors == 0)  && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { monthColor = CHSV(random(0, 255), 255, 255);  dayColor = CHSV(random(0, 255), 255, 255);separatorColor = CHSV(random(0, 255), 255, 255);}
  if ((DateColorSettings == 2 && pastelColors == 1)  && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { monthColor = CRGB(random(0, 255), random(0, 255), random(0, 255));  dayColor = CRGB(random(0, 255), random(0, 255), random(0, 255));separatorColor = CRGB(random(0, 255), random(0, 255), random(0, 255));}
  if ((DateColorSettings == 3 && pastelColors == 0) && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { monthColor = CHSV(random(0, 255), 255, 255);  dayColor = monthColor; separatorColor = monthColor;}
  if ((DateColorSettings == 3 && pastelColors == 1) && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { monthColor = CRGB(random(0, 255), random(0, 255), random(0, 255));  dayColor = monthColor; separatorColor = monthColor;}

 if ((dateDisplayType == 5)) {    //With "." Separator (MM.DD)
  if (m1 > 0) {
    tinymonthColor = monthColor;
    if (DateColorSettings == 4 && pastelColors == 0){ tinymonthColor = CHSV(random(0, 255), 255, 255); }
    if (DateColorSettings == 4 && pastelColors == 1){ tinymonthColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(32*LEDS_PER_SEGMENT); i<(33*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinymonthColor;}
    if (DateColorSettings == 4 && pastelColors == 0){ tinymonthColor = CHSV(random(0, 255), 255, 255); }
    if (DateColorSettings == 4 && pastelColors == 1){ tinymonthColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(33*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinymonthColor;}
  } else {for (int i=(32*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black;}
   }
    displayNumber(m2,5,monthColor);
    displayNumber(d1,2,dayColor);
    displayNumber(d2,0,dayColor);
    if ((DateColorSettings == 4 ) && pastelColors == 0){ separatorColor = CHSV(random(0, 255), 255, 255); }
    if ((DateColorSettings == 4 ) && pastelColors == 1){ separatorColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(20*LEDS_PER_SEGMENT); i<(21*LEDS_PER_SEGMENT); i++) { LEDs[i] = separatorColor;}
  }

  if (dateDisplayType == 1) {    //Space-Padded (MMDD)
    if (m1 < 1) { displayNumber(10,6,monthColor); }
    else { displayNumber(m1,6,monthColor); }
      displayNumber(m2,4,monthColor);
      displayNumber(d1,2,dayColor);
      displayNumber(d2,0,dayColor);
  }

  if (dateDisplayType == 2) {    //Center Dates (1MDD)
  if (m1 > 0) {
    tinymonthColor = monthColor;
    if (DateColorSettings == 4 && pastelColors == 0){ tinymonthColor = CHSV(random(0, 255), 255, 255); }
    if (DateColorSettings == 4 && pastelColors == 1){ tinymonthColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(32*LEDS_PER_SEGMENT); i<(33*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinymonthColor;}
    if (DateColorSettings == 4 && pastelColors == 0){ tinymonthColor = CHSV(random(0, 255), 255, 255); }
    if (DateColorSettings == 4 && pastelColors == 1){ tinymonthColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      for (int i=(33*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = tinymonthColor;}
  } else {for (int i=(32*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black;}
   }
      displayNumber(m2,5,monthColor);
      displayNumber(d1,3,dayColor);
      displayNumber(d2,1,dayColor);
  }

  if (dateDisplayType == 0) {    //Zero-Padded (MMDD)
    if (m1 < 1) { displayNumber(0,6,monthColor);}
    else  { displayNumber(m1,6,monthColor);}
      displayNumber(m2,4,monthColor);
      displayNumber(d1,2,dayColor);
      displayNumber(d2,0,dayColor);
  }

  if (dateDisplayType == 4) {    //Just Numeric Day (DD)
  if (d1 < 1) {
      displayNumber(d2,3,dayColor);
  } else {
      displayNumber(d1,4,dayColor);
      displayNumber(d2,2,dayColor);
  }
  }

  if (dateDisplayType == 3) {    //Just Day of Week (Sun)
  if (timeinfo.tm_wday == 1)    {displayNumber(78,5,dayColor);displayNumber(80,3,dayColor); displayNumber(79,1,dayColor);}
  if (timeinfo.tm_wday == 2)  {displayNumber(85,6,dayColor);displayNumber(54,4,dayColor); displayNumber(38,2,dayColor); displayNumber(52,0,dayColor);}
  if (timeinfo.tm_wday == 3) {displayNumber(88,5,dayColor);displayNumber(38,3,dayColor); displayNumber(69,1,dayColor);}
  if (timeinfo.tm_wday == 4)  {displayNumber(85,6,dayColor);displayNumber(73,4,dayColor); displayNumber(86,2,dayColor); displayNumber(83,0,dayColor);}
  if (timeinfo.tm_wday == 5)    {displayNumber(39,5,dayColor);displayNumber(83,3,dayColor); displayNumber(42,1,dayColor);}
  if (timeinfo.tm_wday == 6)  {displayNumber(52,5,dayColor);displayNumber(34,3,dayColor); displayNumber(85,1,dayColor);}
  if (timeinfo.tm_wday == 0)    {displayNumber(52,5,dayColor);displayNumber(86,3,dayColor); displayNumber(79,1,dayColor);}
  }

  if (dateDisplayType == 6) {    //Just Year (YYYY)
  displayNumber(2,6,monthColor);
  displayNumber(0,4,monthColor);
  displayNumber(y1,2,dayColor);
  displayNumber(y2,0,dayColor);
  }
} // end of update date

void displayScrollMode(){   //scrollmode for displaying clock things not just text
  currentMode = 0;
  if (scrollActive) { return; }   //already scrolling - keep going
  if (realtimeMode == 0) {
    if (!getLocalTime(&timeinfo)){ Serial.println("Failed to obtain time");  }
    char strTime[10];
    char strDate[10];
    char strYear[10];
    char strIPaddy[20];
    char DOW[10];
    int hour = timeinfo.tm_hour;
    int mins = timeinfo.tm_min;
    int mday = timeinfo.tm_mday;
    int mont = timeinfo.tm_mon + 1;
    int year = timeinfo.tm_year +1900;

    if (timeinfo.tm_wday == 1)    {sprintf(DOW,"%s","Mon    ");}
    if (timeinfo.tm_wday == 2)    {sprintf(DOW,"%s","tUES    ");}
    if (timeinfo.tm_wday == 3)    {sprintf(DOW,"%s","WEd    ");}
    if (timeinfo.tm_wday == 4)    {sprintf(DOW,"%s","thur    ");}
    if (timeinfo.tm_wday == 5)    {sprintf(DOW,"%s","FrI    ");}
    if (timeinfo.tm_wday == 6)    {sprintf(DOW,"%s","SAt    ");}
    if (timeinfo.tm_wday == 0)    {sprintf(DOW,"%s","Sun    ");}
    sprintf(strTime, "%.2d%.2d    ", hour, mins);  //1111
    sprintf(strDate, "%.2d-%.2d    ", mont, mday);  //10-22
    sprintf(strYear, "%d    ", year);  //2021
    sprintf(strIPaddy, "%s", WiFi.localIP().toString().c_str());  //192_168_0_10
    String processedText;
    processedText.reserve(256);      // pre-allocates to avoid reallocs

    if (scrollOptions1) processedText += String(strTime)        + "  ";
    if (scrollOptions2) processedText += String(DOW)            + "  ";
    if (scrollOptions3) processedText += String(strDate)        + "  ";
    if (scrollOptions4) processedText += String(strYear)        + "  ";
    if (scrollOptions7) processedText += scrollText             + "    ";
    if (scrollOptions8) processedText += String(strIPaddy)      + "  ";
    startScroll(processedText);
  }
}

void displayCountdownMode() {     //main countdown function
  if (countdownMilliSeconds == 0 && endCountDownMillis == 0)
    return;
  unsigned long restMillis = endCountDownMillis - millis();
  unsigned long hours   = ((restMillis / 1000) / 60) / 60;
  unsigned long minutes = (restMillis / 1000) / 60;
  unsigned long seconds = restMillis / 1000;
  int remSeconds = seconds - (minutes * 60);
  int remMinutes = minutes - (hours * 60);
  byte h1 = hours / 10;
  byte h2 = hours % 10;
  byte m1 = remMinutes / 10;
  byte m2 = remMinutes % 10;
  byte s1 = remSeconds / 10;
  byte s2 = remSeconds % 10;
  CRGB countdownColor = CRGB(countdownColorValue);
  if (restMillis <= 10000 && colorchangeCD == 1) {  //red mode last 10 seconds
    countdownColor = CRGB::Red;
  }
  if (hours > 0) {   // hh:mm
    displayNumber(h1,6,countdownColor);
    displayNumber(h2,4,countdownColor);
    displayNumber(m1,2,countdownColor);
    displayNumber(m2,0,countdownColor);
  } else {   // mm:ss
    displayNumber(m1,6,countdownColor);
    displayNumber(m2,4,countdownColor);
    displayNumber(s1,2,countdownColor);
    displayNumber(s2,0,countdownColor);
  }
  if (hours <= 0 && remMinutes <= 0 && remSeconds <= 0) { //timer ended
    endCountdown();
    countdownMilliSeconds = 0;
    endCountDownMillis = 0;
    return;
  }
}

void displayStopwatchMode() {     //main stopwatch timer function
  if (millis() >= endCountDownMillis) {  //timer ended
    CountUpMillis = 0;
    endCountDownMillis = 0;
    endCountdown();
    return;
  }
  unsigned long restMillis = millis() - CountUpMillis;
  unsigned long hours   = ((restMillis / 1000) / 60) / 60;
  unsigned long minutes = (restMillis / 1000) / 60;
  unsigned long seconds = restMillis / 1000;
  int remSeconds = seconds - (minutes * 60);
  int remMinutes = minutes - (hours * 60);
  byte h1 = hours / 10;
  byte h2 = hours % 10;
  byte m1 = remMinutes / 10;
  byte m2 = remMinutes % 10;
  byte s1 = remSeconds / 10;
  byte s2 = remSeconds % 10;
  CRGB countdownColor = CRGB(countdownColorValue);
  if (millis() >= (endCountDownMillis - 10000) && colorchangeCD == 1) {  //red mode at last 10 seconds
    countdownColor = CRGB::Red;
  }
  if (hours > 0) { //show hours and minutes
    displayNumber(h1,6,countdownColor);
    displayNumber(h2,4,countdownColor);
    displayNumber(m1,2,countdownColor);
    displayNumber(m2,0,countdownColor);
  } else { //or show minutes and seconds
    displayNumber(m1,6,countdownColor);
    displayNumber(m2,4,countdownColor);
    displayNumber(s1,2,countdownColor);
    displayNumber(s2,0,countdownColor);
  }
}

void displayScoreboardMode() {  //main scoreboard function
  currentMode = 0;
  byte sl1 = scoreboardLeft / 10;
  byte sl2 = scoreboardLeft % 10;
  byte sr1 = scoreboardRight / 10;
  byte sr2 = scoreboardRight % 10;
  displayNumber(sl1,6,scoreboardColorLeft);
  displayNumber(sl2,4,scoreboardColorLeft);
  displayNumber(sr1,2,scoreboardColorRight);
  displayNumber(sr2,0,scoreboardColorRight);
}//end of update scoreboard

void displayHAValueMode() {
  currentMode = 0;
  int intVal = (int)haDisplayValue;
  bool hasDec = (haDisplayValue != (float)intVal);

  if (haDisplayDegree && hasDec && haDisplayValue < 100.0f) {
    // XX.X° format (e.g., 19.4°)
    int val = (int)(haDisplayValue * 10.0f + 0.5f); // one decimal
    if (val > 999) val = 999;
    displayNumber((val / 100) % 10, 6, hourColor);
    displayNumber((val / 10) % 10, 4, hourColor);
    displayNumber(val % 10, 2, hourColor);
    displayNumber(26, 0, hourColor); // degree symbol
    // Show decimal dot (bottom colon position)
    LEDs[19 * LEDS_PER_SEGMENT + LEDS_PER_SEGMENT / 2] = hourColor;
  } else if (haDisplayDegree && !hasDec && haDisplayValue < 1000.0f) {
    // XXX° format (e.g., 42°)
    if (intVal > 999) intVal = 999;
    if (intVal < 0) intVal = 0;
    displayNumber((intVal / 100) % 10, 6, hourColor);
    displayNumber((intVal / 10) % 10, 4, hourColor);
    displayNumber(intVal % 10, 2, hourColor);
    displayNumber(26, 0, hourColor); // degree symbol
  } else if (hasDec && haDisplayValue < 100.0f) {
    // XX.XX format without degree (e.g., 19.40)
    int val = (int)(haDisplayValue * 100.0f + 0.5f);
    if (val > 9999) val = 9999;
    displayNumber((val / 1000) % 10, 6, hourColor);
    displayNumber((val / 100) % 10, 4, hourColor);
    displayNumber((val / 10) % 10, 2, hourColor);
    displayNumber(val % 10, 0, hourColor);
    LEDs[19 * LEDS_PER_SEGMENT + LEDS_PER_SEGMENT / 2] = hourColor;
  } else {
    // XXXX integer format (e.g., 1234)
    if (intVal > 9999) intVal = 9999;
    if (intVal < 0) intVal = 0;
    displayNumber((intVal / 1000) % 10, 6, hourColor);
    displayNumber((intVal / 100) % 10, 4, hourColor);
    displayNumber((intVal / 10) % 10, 2, hourColor);
    displayNumber(intVal % 10, 0, hourColor);
  }
}

void displayLightshowMode() {
  currentMode = 0;
  // all lightshow effects (incl. Chase) run non-blocking from displayRealtimeMode()
}

void displayRealtimeMode(){   //main RealtimeModes function, always is running
  if (scrollActive) { return; }   //don't fight with a running scroll over the LEDs
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 0) {EVERY_N_MILLISECONDS(40) {Chase(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 1) {EVERY_N_MILLISECONDS(30) {Twinkles();FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 2) {Rainbow();FastLED.show();}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 3) {EVERY_N_MILLISECONDS(100) {GreenMatrix();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 4) {blueRain();FastLED.show();}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 5) {EVERY_N_MILLISECONDS(60) {Fire2021();FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 6) {EVERY_N_MILLISECONDS(getSlower) {Snake();FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 7) {EVERY_N_MILLISECONDS(150) {Cylon(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 8) {EVERY_N_MILLISECONDS(20) {Breathing(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 9) {EVERY_N_MILLISECONDS(50) {Sparkle(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 10) {EVERY_N_MILLISECONDS(40) {MeteorRain(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 11) {EVERY_N_MILLISECONDS(60) {ColorWipe(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 12) {EVERY_N_MILLISECONDS(30) {Plasma(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 14) {EVERY_N_MILLISECONDS(30) {Confetti(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 15) {EVERY_N_MILLISECONDS(30) {Juggle(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 16) {EVERY_N_MILLISECONDS(30) {Heartbeat(); FastLED.show();}}
  // Pixel-level effects
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 17) {EVERY_N_MILLISECONDS(20) {PixelRainbow(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 18) {EVERY_N_MILLISECONDS(20) {PixelWave(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 19) {EVERY_N_MILLISECONDS(30) {PixelFireworks(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 20) {EVERY_N_MILLISECONDS(20) {PixelRunningLights(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 21) {EVERY_N_MILLISECONDS(60) {PixelTheaterChase(); FastLED.show();}}
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 22) {EVERY_N_MILLISECONDS(30) {StarrySky(); FastLED.show();}}

  // Random Lightshow – cycle through all effects
  if ( (suspendType == 0 || isAsleep == 0) && clockMode == 5 && lightshowMode == 13) {
    static int randomCurrentEffect = 0;
    static unsigned long randomLastSwitch = 0;
    if (millis() - randomLastSwitch > 15000) { // switch every 15 seconds
      randomLastSwitch = millis();
      do { randomCurrentEffect = random(0, 23); } while (randomCurrentEffect == 13); // any effect except "Random" itself
      allBlank();
      oldsnakecolor = CRGB::Green;
      getSlower = 180;
    }
    switch (randomCurrentEffect) {
      case 0:  EVERY_N_MILLISECONDS(100) { Chase(); FastLED.show(); } break;
      case 1:  EVERY_N_MILLISECONDS(100) { Twinkles(); FastLED.show(); } break;
      case 2:  EVERY_N_MILLISECONDS(20)  { Rainbow(); FastLED.show(); } break;
      case 3:  EVERY_N_MILLISECONDS(75)  { GreenMatrix(); FastLED.show(); } break;
      case 4:  EVERY_N_MILLISECONDS(75)  { blueRain(); FastLED.show(); } break;
      case 5:  EVERY_N_MILLISECONDS(100) { Fire2021(); FastLED.show(); } break;
      case 6:  EVERY_N_MILLISECONDS(50)  { Snake(); FastLED.show(); } break;
      case 7:  EVERY_N_MILLISECONDS(50)  { Cylon(); FastLED.show(); } break;
      case 8:  EVERY_N_MILLISECONDS(20)  { Breathing(); FastLED.show(); } break;
      case 9:  EVERY_N_MILLISECONDS(50)  { Sparkle(); FastLED.show(); } break;
      case 10: EVERY_N_MILLISECONDS(40)  { MeteorRain(); FastLED.show(); } break;
      case 11: EVERY_N_MILLISECONDS(60)  { ColorWipe(); FastLED.show(); } break;
      case 12: EVERY_N_MILLISECONDS(30)  { Plasma(); FastLED.show(); } break;
      case 14: EVERY_N_MILLISECONDS(30)  { Confetti(); FastLED.show(); } break;
      case 15: EVERY_N_MILLISECONDS(30)  { Juggle(); FastLED.show(); } break;
      case 16: EVERY_N_MILLISECONDS(30)  { Heartbeat(); FastLED.show(); } break;
      case 17: EVERY_N_MILLISECONDS(20)  { PixelRainbow(); FastLED.show(); } break;
      case 18: EVERY_N_MILLISECONDS(20)  { PixelWave(); FastLED.show(); } break;
      case 19: EVERY_N_MILLISECONDS(30)  { PixelFireworks(); FastLED.show(); } break;
      case 20: EVERY_N_MILLISECONDS(20)  { PixelRunningLights(); FastLED.show(); } break;
      case 21: EVERY_N_MILLISECONDS(60)  { PixelTheaterChase(); FastLED.show(); } break;
      case 22: EVERY_N_MILLISECONDS(30)  { StarrySky(); FastLED.show(); } break;
    }
  }
}//end of RealtimeModes

void endCountdown() {  //countdown timer has reached 0, sound alarm and flash End for 30 seconds
  //  Serial.println("endcountdown function");
          Serial.println("timer ends");
  breakOutSet = 0;
  FastLED.setBrightness(255);
  CRGB color = CRGB::Red;
  allBlank();
  clockMode = currentMode;
  realtimeMode = currentReal;
  if (!breakOutSet) {startScroll("tIMEr Ended      tIMEr Ended");}
  allBlank();
}

CRGB colorWheel(int pos) {   //color wheel for mostly spectrum analyzer
  CRGB color (0,0,0);
  if(pos < 85) {
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  } else if(pos < 170) {
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  } else if(pos < 256) {
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }
  return color;
}

CRGB colorWheel2(int pos) {   //color wheel for things not the spectrum analyzer
  CRGB color (0,0,0);
  if(pos < 85) {
    color.g = 0;
    color.r = ((float)pos / 85.0f) * 255.0f;
    color.b = 255 - color.r;
  } else if(pos < 170) {
    color.g = ((float)(pos - 85) / 85.0f) * 255.0f;
    color.r = 255 - color.g;
    color.b = 0;
  } else if(pos < 256) {
    color.b = ((float)(pos - 170) / 85.0f) * 255.0f;
    color.g = 255 - color.b;
    color.r = 1;
  }
  return color;
}

static uint16_t translateChar(char SentenceLetter) {   //maps a character to its numbers[] font index
    uint16_t LetterNumber = 10;  //for all unknown characters
    if( SentenceLetter == '0') { LetterNumber = 0; }
    if( SentenceLetter == '1') { LetterNumber = 1; }
    if( SentenceLetter == '2') { LetterNumber = 2; }
    if( SentenceLetter == '3') { LetterNumber = 3; }
    if( SentenceLetter == '4') { LetterNumber = 4; }
    if( SentenceLetter == '5') { LetterNumber = 5; }
    if( SentenceLetter == '6') { LetterNumber = 6; }
    if( SentenceLetter == '7') { LetterNumber = 7; }
    if( SentenceLetter == '8') { LetterNumber = 8; }
    if( SentenceLetter == '9') { LetterNumber = 9; }
    if( SentenceLetter == ' ') { LetterNumber = 10; }
    if( SentenceLetter == '`') { LetterNumber = 17; }
    if( SentenceLetter == 'A') { LetterNumber = 34; }
    if( SentenceLetter == 'B') { LetterNumber = 35; }
    if( SentenceLetter == 'C') { LetterNumber = 36; }
    if( SentenceLetter == 'D') { LetterNumber = 37; }
    if( SentenceLetter == 'E') { LetterNumber = 38; }
    if( SentenceLetter == 'F') { LetterNumber = 39; }
    if( SentenceLetter == 'G') { LetterNumber = 40; }
    if( SentenceLetter == 'H') { LetterNumber = 41; }
    if( SentenceLetter == 'I') { LetterNumber = 42; }
    if( SentenceLetter == 'J') { LetterNumber = 43; }
    if( SentenceLetter == 'K') { LetterNumber = 44; }
    if( SentenceLetter == 'L') { LetterNumber = 45; }
    if( SentenceLetter == 'M') { LetterNumber = 46; }
    if( SentenceLetter == 'N') { LetterNumber = 47; }
    if( SentenceLetter == 'O') { LetterNumber = 48; }
    if( SentenceLetter == 'P') { LetterNumber = 49; }
    if( SentenceLetter == 'Q') { LetterNumber = 50; }
    if( SentenceLetter == 'R') { LetterNumber = 51; }
    if( SentenceLetter == 'S') { LetterNumber = 52; }
    if( SentenceLetter == 'T') { LetterNumber = 53; }
    if( SentenceLetter == 'U') { LetterNumber = 54; }
    if( SentenceLetter == 'V') { LetterNumber = 55; }
    if( SentenceLetter == 'W') { LetterNumber = 56; }
    if( SentenceLetter == 'X') { LetterNumber = 57; }
    if( SentenceLetter == 'Y') { LetterNumber = 58; }
    if( SentenceLetter == 'Z') { LetterNumber = 59; }
    if( SentenceLetter == 'a') { LetterNumber = 66; }
    if( SentenceLetter == 'b') { LetterNumber = 67; }
    if( SentenceLetter == 'c') { LetterNumber = 68; }
    if( SentenceLetter == 'd') { LetterNumber = 69; }
    if( SentenceLetter == 'e') { LetterNumber = 70; }
    if( SentenceLetter == 'f') { LetterNumber = 71; }
    if( SentenceLetter == 'g') { LetterNumber = 72; }
    if( SentenceLetter == 'h') { LetterNumber = 73; }
    if( SentenceLetter == 'i') { LetterNumber = 74; }
    if( SentenceLetter == 'j') { LetterNumber = 75; }
    if( SentenceLetter == 'k') { LetterNumber = 76; }
    if( SentenceLetter == 'l') { LetterNumber = 77; }
    if( SentenceLetter == 'm') { LetterNumber = 78; }
    if( SentenceLetter == 'n') { LetterNumber = 79; }
    if( SentenceLetter == 'o') { LetterNumber = 80; }
    if( SentenceLetter == 'p') { LetterNumber = 81; }
    if( SentenceLetter == 'q') { LetterNumber = 82; }
    if( SentenceLetter == 'r') { LetterNumber = 83; }
    if( SentenceLetter == 's') { LetterNumber = 84; }
    if( SentenceLetter == 't') { LetterNumber = 85; }
    if( SentenceLetter == 'u') { LetterNumber = 86; }
    if( SentenceLetter == 'v') { LetterNumber = 87; }
    if( SentenceLetter == 'w') { LetterNumber = 88; }
    if( SentenceLetter == 'x') { LetterNumber = 89; }
    if( SentenceLetter == 'y') { LetterNumber = 90; }
    if( SentenceLetter == 'z') { LetterNumber = 91; }
    if( SentenceLetter == ',') { LetterNumber = 22; }
    if( SentenceLetter == '-') { LetterNumber = 23; }
    if( SentenceLetter == '.') { LetterNumber = 24; }
    if( SentenceLetter == ':') { LetterNumber = 27; }
    if( SentenceLetter == '^') { LetterNumber = 26; }
    if( SentenceLetter == '\'') { LetterNumber = 17; }
    if( SentenceLetter == '%') { LetterNumber = 15; }
    return LetterNumber;
} //end of translateChar

// --- Non-blocking scrolling state machine ---
// startScroll() prepares the buffer, scrollTick() (called every loop() pass)
// advances one column every SCROLL_STEP_MS. While scrollActive is true the
// normal mode rendering in loop() is skipped.
#define SCROLL_MAX_CHARS 400
#define SCROLL_STEP_MS   250
static uint16_t scrollBuf[SCROLL_MAX_CHARS * 2 + 13];  // 6 pads + 2/char + 6 pads
static uint16_t scrollSteps = 0;
static uint16_t scrollStep = 0;
static unsigned long scrollNextMs = 0;
static int scrollRepeatsLeft = 0;
bool scrollActive = false;

void startScroll(String IncomingString) {
  Serial.println(IncomingString);
  if (IncomingString.length() == 0) { return; }
  if (IncomingString.length() > SCROLL_MAX_CHARS) { IncomingString = IncomingString.substring(0, SCROLL_MAX_CHARS); }
  breakOutSet = 0;
  if (scrollColorSettings == 0){ scrollColor = CRGB(scrollColorValue); }
  if (scrollColorSettings == 1 && pastelColors == 0){ scrollColor = CHSV(random(0, 255), 255, 255); }
  if (scrollColorSettings == 1 && pastelColors == 1){ scrollColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }

  uint16_t len = IncomingString.length();
  for (int k = 0; k < 6; k++) { scrollBuf[k] = 96; }              //pad front with markers
  for (uint16_t i = 0; i < len; i++) {
    scrollBuf[i * 2 + 6] = translateChar(IncomingString[i]);
    scrollBuf[i * 2 + 7] = 96;  //padding - fake digit shares a leg with adjacent real ones
  }
  for (int k = 0; k < 6; k++) { scrollBuf[len * 2 + 6 + k] = 96; } //pad back with markers
  scrollSteps = len * 2 + 6;
  scrollStep = 0;
  scrollRepeatsLeft = 0;
  scrollNextMs = millis();
  scrollActive = true;
}

void startScrollColored(String text, CRGB color, int repeat) {  //scroll in a fixed color (notifications)
  startScroll(text);
  if (!scrollActive) { return; }
  scrollColor = color;
  scrollRepeatsLeft = (repeat > 1) ? repeat - 1 : 0;
}

void stopScroll() {
  if (!scrollActive) { return; }
  scrollActive = false;
  allBlank();
}

void scrollTick() {
  if (!scrollActive) { return; }
  if (breakOutSet) { stopScroll(); return; }
  unsigned long now = millis();
  if ((long)(now - scrollNextMs) < 0) { return; }
  scrollNextMs = now + SCROLL_STEP_MS;
  for (int i = 0; i < SEGMENTS_LEDS; i++) { LEDs[i] = CRGB::Black; }    //clear
  for (int d = 0; d < 7; d++) {
    if (scrollBuf[scrollStep + d] != 96) { displayNumber(scrollBuf[scrollStep + d], 6 - d, scrollColor); }
  }
  FastLED.show();
  scrollStep++;
  if (scrollStep >= scrollSteps) {
    if (scrollRepeatsLeft > 0) { scrollRepeatsLeft--; scrollStep = 0; }
    else { stopScroll(); }
  }
}

void scroll(String IncomingString) {  //blocking wrapper - only for setup(), before loop() runs
  startScroll(IncomingString);
  while (scrollActive) {
    scrollTick();
    server.handleClient();
    haMqtt.loop();
    mqttClient.loop();
    delay(1);
  }
} //end of scroll function


void printLocalTime() {  //what could this do
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "Local Time: %A, %B %d %Y %H:%M:%S");
}


void checkSleepTimer(){  //controls suspend mode
}


void applyBrightness() {
  FastLED.setBrightness(brightness);
}

// Partial-segment glyphs: letters like M/W/V/X/K are barely readable with
// whole 7-segment strokes. These glyphs light segments in THIRDS instead -
// per role a 3-bit mask in visual order (bit0 = top/left third, bit1 =
// middle, bit2 = bottom/right third): 0b111 full, 0b101 gap in the middle,
// 0b010 center only. Role order matches numbers[]: 0=lower-right, 1=bottom,
// 2=lower-left, 3=upper-left, 4=top, 5=upper-right, 6=middle.
struct PartialGlyph {
  byte charIndex;   // index into numbers[]
  byte thirds[7];
};
static const PartialGlyph partialGlyphs[] = {
  //     r0     r1     r2     r3     r4     r5     r6
  {46, {0b111, 0b000, 0b111, 0b111, 0b101, 0b111, 0b000}},  // M: towers, notch at top center
  {78, {0b111, 0b000, 0b111, 0b111, 0b101, 0b111, 0b000}},  // m
  {56, {0b111, 0b101, 0b111, 0b111, 0b000, 0b111, 0b000}},  // W: towers, notch at bottom center
  {88, {0b111, 0b101, 0b111, 0b111, 0b000, 0b111, 0b000}},  // w
  {55, {0b111, 0b010, 0b111, 0b111, 0b000, 0b111, 0b000}},  // V: sides + pointed bottom center
  {87, {0b111, 0b010, 0b111, 0b111, 0b000, 0b111, 0b000}},  // v
  {57, {0b011, 0b000, 0b011, 0b110, 0b000, 0b110, 0b010}},  // X: hourglass pinch at the middle
  {89, {0b011, 0b000, 0b011, 0b110, 0b000, 0b110, 0b010}},  // x
  {44, {0b011, 0b000, 0b111, 0b111, 0b000, 0b110, 0b011}},  // K: left bar + angled arms
  {76, {0b011, 0b000, 0b111, 0b111, 0b000, 0b110, 0b011}},  // k
};

// LED direction per segment role (ascending FAKE_LEDs order vs. visual
// top->bottom / left->right). Fake digit columns borrow their verticals from
// the neighbouring real digits, so their directions differ.
static const bool segVisualReversedReal[7] = {false, true, true, true, false, false, true};
static const bool segVisualReversedFake[7] = {true,  true, false, false, false, true, true};

void displayNumber(uint16_t number, byte segment, CRGB color) {   //main digit rendering (except when scrolling)
  // segments from left to right: 6, 5, 4, 3, 2, 1, 0
  uint16_t startindex = 0;
 //   Serial.println(startindex);
  switch (segment) {
    case 0:
      startindex = 0;
      break;
    case 1:
      startindex = (LEDS_PER_DIGIT * 1);
      break;
    case 2:
      startindex = (LEDS_PER_DIGIT * 2);
      break;
    case 3:
      startindex = (LEDS_PER_DIGIT * 3);
      break;
    case 4:
      startindex = (LEDS_PER_DIGIT * 4);
      break;
    case 5:
      startindex = (LEDS_PER_DIGIT * 5);
      break;
    case 6:
      startindex = (LEDS_PER_DIGIT * 6);
      break;
  }

  // does this character use a partial-segment glyph?
  const byte* partial = NULL;
  for (unsigned int p = 0; p < sizeof(partialGlyphs) / sizeof(partialGlyphs[0]); p++) {
    if (partialGlyphs[p].charIndex == number) { partial = partialGlyphs[p].thirds; break; }
  }
  const bool* rev = (segment % 2 == 1) ? segVisualReversedFake : segVisualReversedReal;

  for (byte i=0; i<SEGMENTS_PER_NUMBER; i++){                // 7 segments
    for (byte j=0; j<LEDS_PER_SEGMENT; j++ ){              // LEDs per segment
      yield();
      bool ledOn;
      if (partial) {
        byte visualPos = rev[i] ? (LEDS_PER_SEGMENT - 1 - j) : j;
        byte third = (visualPos * 3) / LEDS_PER_SEGMENT;
        ledOn = partial[i] & (1 << third);
      } else {
        ledOn = (numbers[number] & (1 << i)) != 0;
      }
      LEDs[FAKE_LEDs[i * LEDS_PER_SEGMENT + j + startindex]] = ledOn ? color : alternateColor;
    }
  }
} //end of displayNumber

void allTest() {   //tests all non-shelf LEDs to black
  //  Serial.println("alltest function");
  for (int i=0; i<SEGMENTS_LEDS + 14; i++) {
    LEDs[i] = CRGB::Red;
    FastLED.show();
  }
  delay(1000);

  for (int i=0; i<SEGMENTS_LEDS + 14; i++) {
    LEDs[i] = CRGB::Green;
    FastLED.show();
  }
  delay(1000);

  for (int i=0; i<SEGMENTS_LEDS + 14; i++) {
    LEDs[i] = CRGB::Blue;
    FastLED.show();
  }
  delay(1000);

  for (int i=0; i<SEGMENTS_LEDS + 14; i++) {
    LEDs[i] = CRGB::White;
    FastLED.show();
    delay(100);
  }
  delay(1000);

  for (int i=0; i<SEGMENTS_LEDS + 14; i++) {
    LEDs[i] = CRGB::Black;
  }
  FastLED.show();
}

void allBlank() {   //clears all non-shelf LEDs to black
  //  Serial.println("allblank function");
  for (int i=0; i<SEGMENTS_LEDS; i++) {
    LEDs[i] = CRGB::Black;
  }
  FastLED.show();
  applyBrightness();
}  // end of all-blank


void fakeClock(int loopy) {  //flashes 12:00 like all old clocks did
  for (int i=0; i<loopy; i++) {
      for (int i=(32*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Red;}  // 1x:xx
      displayNumber(2,5,CRGB::Red);  // x2:xx
      for (int i=(25*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2-1); i<(25*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1); i++) { LEDs[i] = CRGB::Black; }  // xx:xx
      for (int i=(20*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2-1); i<(20*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1); i++) { LEDs[i] = CRGB::Black; } // xx:xx
      displayNumber(0,2,CRGB::Red); // xx:0x
      displayNumber(0,0,CRGB::Red);  // xx:x0
      FastLED.show();
      delay(500);
      for (int i=(32*LEDS_PER_SEGMENT); i<(34*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black;}
      displayNumber(2,5,CRGB::Black);
      for (int i=(25*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2-1); i<(25*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1); i++) { LEDs[i] = CRGB::Red; }
      for (int i=(20*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2-1); i<(20*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1); i++) { LEDs[i] = CRGB::Red; }
      displayNumber(0,2,CRGB::Black);
      displayNumber(0,0,CRGB::Black);
      FastLED.show();
      delay(500);
   }
  for (int i=(25*LEDS_PER_SEGMENT); i<(26*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black; }
  for (int i=(20*LEDS_PER_SEGMENT); i<(21*LEDS_PER_SEGMENT); i++) { LEDs[i] = CRGB::Black; }
}  //end of fakeClock


void ShelfDownLights() {  //turns on the drop lights on the underside of each shelf
int currentHour  = timeinfo.tm_hour;
 //   Serial.println("ShelfDownLights function");
 if ((suspendType != 2 || isAsleep == 0) && useSpotlights == 1) {  //not sleeping? suposed to be running?
  unsigned long currentMillis = millis();
  if (currentMillis - prevTime2 >= 250) {  //run everything inside here every quarter second
    for (int i=SEGMENTS_LEDS; i<NUM_LEDS; i++) {
        if (spotlightsColorSettings == 0){ spotlightsColor = CRGB(spotlightsColorValue);  LEDs[i] = spotlightsColor;}
        if ((spotlightsColorSettings == 1 && pastelColors == 0)  && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { spotlightsColor = CHSV(random(0, 255), 255, 255);  LEDs[i] = spotlightsColor;}
        if ((spotlightsColorSettings == 1 && pastelColors == 1)  && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { spotlightsColor = CRGB(random(0, 255), random(0, 255), random(0, 255));  LEDs[i] = spotlightsColor;}
        if (spotlightsColorSettings == 2 ){ LEDs[i] = colorWheel2(((i-SEGMENTS_LEDS)  * 18 + colorWheelPositionTwo) % 256); }
        if (spotlightsColorSettings == 3 ){ LEDs[i] = colorWheel2(((255) + colorWheelPositionTwo) % 256); }
        if (spotlightsColorSettings == 4 ){
          int seed = random(2500);         // A random number. Higher number => fewer twinkles. Use random16() for values >255.
          if (seed < 30) {
            CRGB color = CRGB::Black;
            if (pastelColors == 0){ color = CHSV(random(0, 255), 255, 255); }
            if (pastelColors == 1){ color = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
                  {
                    LEDs[SEGMENTS_LEDS+random((NUM_LEDS-SEGMENTS_LEDS))] = color;
                  }
            }
            for (int j=SEGMENTS_LEDS; j<NUM_LEDS; j++) {
              LEDs[j].fadeToBlackBy(1);
              }
         }
    }
    colorWheelPositionTwo = colorWheelPositionTwo - 1; // SPEED OF 2nd COLOR WHEEL
    if (colorWheelPositionTwo < 0) {colorWheelPositionTwo = 255;} // RESET 2nd COLOR WHEEL
    if ((spotlightsColorSettings == 2 || spotlightsColorSettings == 3) && clockMode != 11){FastLED.show();}
    prevTime2 = currentMillis;
  FastLED.show();
  }
 } else if (useSpotlights == 0) {  //or turn them all off
  for (int i=SEGMENTS_LEDS; i<NUM_LEDS; i++) {
    LEDs[i] = CRGB::Black;
  }
  FastLED.show();
 }

}//end of ShelfDownLights


void BlinkDots() {  //displays the dots in the middle of the time (colon, period, bar)
  if (ClockColorSettings == 0 || ClockColorSettings == 1) {colonColor = CRGB(colonColorValue);}
    if (((ClockColorSettings == 2) || (ClockColorSettings == 4)) && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255));}
    if (ClockColorSettings == 3 && ( (ColorChangeFrequency == 0 ) || (ColorChangeFrequency == 1 && randomMinPassed == 1) || (ColorChangeFrequency == 2 && randomHourPassed == 1) || (ColorChangeFrequency == 3 && randomDayPassed == 1) || (ColorChangeFrequency == 4 && randomWeekPassed == 1) || (ColorChangeFrequency == 5 && randomMonthPassed == 1) )) { colonColor = hourColor;}
    if (dotsOn) {

    if (colonType == 0) {   //small colon
      if (clockDisplayType == 1 || clockDisplayType == 2) {     //for 12 or 24-hour Military Time
        if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
        if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=17*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2; i<17*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1; i++) { LEDs[i] = colonColor;}
        if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
        if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=19*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2; i<19*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1; i++) { LEDs[i] = colonColor;}
      }
      else {       //for non 12 or 24-hour Military Time
        if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
        if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=25*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2-1; i<25*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1; i++) { LEDs[i] = colonColor;}
        if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
        if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=20*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2-1; i<20*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1; i++) { LEDs[i] = colonColor;}
      }
    }
    if (colonType == 1) {  //big bar
      if (clockDisplayType == 3) {     //anything but 12 or 24-hour Military Time
        if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
        if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=25*LEDS_PER_SEGMENT; i<26*LEDS_PER_SEGMENT; i++) { LEDs[i] = colonColor;}
        if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
        if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
        for (int i=20*LEDS_PER_SEGMENT; i<21*LEDS_PER_SEGMENT; i++) { LEDs[i] = colonColor;}
      }
    }
    if (colonType == 2) {  //small period
      if (ClockColorSettings == 4 && pastelColors == 0){ colonColor = CHSV(random(0, 255), 255, 255); }
      if (ClockColorSettings == 4 && pastelColors == 1){ colonColor = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
      if (clockDisplayType == 1 || clockDisplayType == 2) {     //for 12 or 24-hour Military Time
        for (int i=17*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2; i<17*LEDS_PER_SEGMENT+LEDS_PER_SEGMENT/2+1; i++) { LEDs[i] = colonColor;}
      }
      else {   //big period for non 12 or 24-hour Military Time
	      for (int i=20*LEDS_PER_SEGMENT; i<21*LEDS_PER_SEGMENT; i++) { LEDs[i] = colonColor;}
      }
    }


  } else {  //hide all dots, blinking
    if (clockDisplayType == 1 || clockDisplayType == 2) {     //12 or 24-hour Military Time
      for (int i=17*LEDS_PER_SEGMENT; i<18*LEDS_PER_SEGMENT; i++) { LEDs[i] = CRGB::Black;}
      for (int i=19*LEDS_PER_SEGMENT; i<20*LEDS_PER_SEGMENT; i++) { LEDs[i] = CRGB::Black;}
    }
    else {
	    for (int i=25*LEDS_PER_SEGMENT; i<26*LEDS_PER_SEGMENT; i++) { LEDs[i] = CRGB::Black;}
	    for (int i=20*LEDS_PER_SEGMENT; i<21*LEDS_PER_SEGMENT; i++) { LEDs[i] = CRGB::Black;}
    }



  }
  dotsOn = !dotsOn;
}//end of shelf and gaps
