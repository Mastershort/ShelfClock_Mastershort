#include "../include/party_games.h"

// ============================================================
// Party Games – clockMode 2
// Non-blocking state machines using millis() timing
// ============================================================

int partyGameType = 0; // 0=Dice, 1=Roulette, 2=ShotTimer, 3=HigherLower

// --- Dice Game ---
// Rolls a random number 1-6, shows spinning animation then result
static enum { DICE_IDLE, DICE_ROLLING, DICE_SHOW } diceState = DICE_IDLE;
static unsigned long diceTimer = 0;
static int diceRollCount = 0;
static int diceResult = 1;

static void partyDice() {
  unsigned long now = millis();
  switch (diceState) {
    case DICE_IDLE:
      // Waiting for start - show last result or blank
      break;
    case DICE_ROLLING: {
      // Show rapidly changing numbers for animation
      if (now - diceTimer > 80) {
        diceTimer = now;
        int showNum = random(1, 7);
        allBlank();
        CRGB rollColor = CHSV(random8(), 255, 255);
        displayNumber(showNum, 3, rollColor);
        FastLED.show();
        diceRollCount++;
        if (diceRollCount > 20) {
          // Done rolling, show final result
          diceResult = random(1, 7);
          diceState = DICE_SHOW;
          diceTimer = now;
        }
      }
      break;
    }
    case DICE_SHOW: {
      // Flash the result a few times then hold
      unsigned long elapsed = now - diceTimer;
      if (elapsed < 3000) {
        allBlank();
        // Blink effect: show for 300ms, hide for 200ms
        if ((elapsed / 250) % 2 == 0) {
          displayNumber(diceResult, 3, CRGB::Green);
        }
        FastLED.show();
      } else {
        // Hold the result steady
        allBlank();
        displayNumber(diceResult, 3, CRGB::Green);
        FastLED.show();
      }
      break;
    }
  }
}

// --- Roulette Game ---
// Light chases around segments, decelerating to land on a random position
static enum { ROUL_IDLE, ROUL_SPINNING, ROUL_LANDED } roulState = ROUL_IDLE;
static unsigned long roulTimer = 0;
static int roulPos = 0;
static int roulDelay = 30;      // starting speed (ms per step)
static int roulTargetDelay = 0; // when delay reaches this, stop
static int roulFinalPos = 0;

static void partyRoulette() {
  unsigned long now = millis();
  switch (roulState) {
    case ROUL_IDLE:
      break;
    case ROUL_SPINNING: {
      if (now - roulTimer > (unsigned long)roulDelay) {
        roulTimer = now;
        // Fade previous
        for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
          int fake = j * LEDS_PER_SEGMENT;
          for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
            LEDs[FAKE_LEDs_C_BRTL[s + fake]].fadeToBlackBy(80);
          }
        }
        // Light current position
        int fake = roulPos * LEDS_PER_SEGMENT;
        CRGB color = CHSV(roulPos * 7, 255, 255);
        for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
          LEDs[FAKE_LEDs_C_BRTL[s + fake]] = color;
        }
        FastLED.show();
        roulPos = (roulPos + 1) % SPECTRUM_PIXELS;
        // Decelerate
        roulDelay += 2;
        if (roulDelay >= roulTargetDelay) {
          roulFinalPos = roulPos;
          roulState = ROUL_LANDED;
          roulTimer = now;
        }
      }
      break;
    }
    case ROUL_LANDED: {
      // Flash the winning position
      unsigned long elapsed = now - roulTimer;
      allBlank();
      if ((elapsed / 300) % 2 == 0) {
        int fake = roulFinalPos * LEDS_PER_SEGMENT;
        for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
          LEDs[FAKE_LEDs_C_BRTL[s + fake]] = CRGB::Red;
        }
      }
      // Show the position number on digits
      int displayVal = roulFinalPos + 1; // 1-37
      displayNumber((displayVal / 10) % 10, 4, CRGB::White);
      displayNumber(displayVal % 10, 2, CRGB::White);
      FastLED.show();
      break;
    }
  }
}

// --- Shot Timer Game ---
// Random countdown, when it hits 0 the display flashes
static enum { SHOT_IDLE, SHOT_COUNTING, SHOT_BOOM } shotState = SHOT_IDLE;
static unsigned long shotTimer = 0;
static unsigned long shotEndTime = 0;

static void partyShotTimer() {
  unsigned long now = millis();
  switch (shotState) {
    case SHOT_IDLE:
      break;
    case SHOT_COUNTING: {
      long remaining = (long)(shotEndTime - now);
      if (remaining <= 0) {
        shotState = SHOT_BOOM;
        shotTimer = now;
        break;
      }
      // Show remaining seconds
      int secs = remaining / 1000;
      allBlank();
      displayNumber((secs / 10) % 10, 4, CRGB::Yellow);
      displayNumber(secs % 10, 2, CRGB::Yellow);
      FastLED.show();
      break;
    }
    case SHOT_BOOM: {
      // Flash "GO" or all red for 5 seconds
      unsigned long elapsed = now - shotTimer;
      if (elapsed < 5000) {
        allBlank();
        if ((elapsed / 200) % 2 == 0) {
          // Flash all segments red
          for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
            int fake = j * LEDS_PER_SEGMENT;
            for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
              LEDs[FAKE_LEDs_C_BLTR[s + fake]] = CRGB::Red;
            }
          }
          displayNumber(40, 4, CRGB::White); // G
          displayNumber(48, 2, CRGB::White); // O (same as 0)
        }
        FastLED.show();
      } else {
        allBlank();
        displayNumber(40, 4, CRGB::Green); // G
        displayNumber(48, 2, CRGB::Green); // O
        FastLED.show();
      }
      break;
    }
  }
}

// --- Higher/Lower Game ---
// Shows a number, waits, then shows a new number and indicates higher/lower
static enum { HL_IDLE, HL_SHOW_FIRST, HL_WAITING, HL_REVEAL } hlState = HL_IDLE;
static unsigned long hlTimer = 0;
static int hlFirstNum = 0;
static int hlSecondNum = 0;

static void partyHigherLower() {
  unsigned long now = millis();
  switch (hlState) {
    case HL_IDLE:
      break;
    case HL_SHOW_FIRST: {
      // Show the first number for 3 seconds
      allBlank();
      displayNumber((hlFirstNum / 10) % 10, 4, CRGB::Cyan);
      displayNumber(hlFirstNum % 10, 2, CRGB::Cyan);
      FastLED.show();
      if (now - hlTimer > 3000) {
        hlState = HL_WAITING;
        hlTimer = now;
        // Show "?" while waiting
        allBlank();
        displayNumber(32, 3, CRGB::Yellow); // ? character
        FastLED.show();
      }
      break;
    }
    case HL_WAITING: {
      // Blink "?" for 3 seconds then auto-reveal
      unsigned long elapsed = now - hlTimer;
      allBlank();
      if ((elapsed / 500) % 2 == 0) {
        displayNumber(32, 3, CRGB::Yellow); // ?
      }
      FastLED.show();
      if (elapsed > 3000) {
        hlSecondNum = random(1, 100);
        hlState = HL_REVEAL;
        hlTimer = now;
      }
      break;
    }
    case HL_REVEAL: {
      // Show both numbers and arrow indicator
      allBlank();
      // First number on left
      displayNumber((hlFirstNum / 10) % 10, 6, CRGB::Cyan);
      displayNumber(hlFirstNum % 10, 4, CRGB::Cyan);
      // Second number on right
      CRGB resultColor = (hlSecondNum > hlFirstNum) ? CRGB::Green : CRGB::Red;
      displayNumber((hlSecondNum / 10) % 10, 2, resultColor);
      displayNumber(hlSecondNum % 10, 0, resultColor);
      FastLED.show();

      // After 5 seconds, start new round automatically
      if (now - hlTimer > 5000) {
        hlFirstNum = hlSecondNum;
        hlState = HL_SHOW_FIRST;
        hlTimer = now;
      }
      break;
    }
  }
}

// --- Main dispatcher ---
void displayPartyMode() {
  switch (partyGameType) {
    case 0: partyDice(); break;
    case 1: partyRoulette(); break;
    case 2: partyShotTimer(); break;
    case 3: partyHigherLower(); break;
  }
}

// --- Start a game (called from web/HA) ---
void startPartyGame() {
  allBlank();
  switch (partyGameType) {
    case 0: // Dice
      diceState = DICE_ROLLING;
      diceTimer = millis();
      diceRollCount = 0;
      break;
    case 1: // Roulette
      roulState = ROUL_SPINNING;
      roulTimer = millis();
      roulPos = 0;
      roulDelay = 30;
      roulTargetDelay = random(250, 500);
      break;
    case 2: // Shot Timer
      shotState = SHOT_COUNTING;
      shotTimer = millis();
      shotEndTime = millis() + (unsigned long)random(5, 31) * 1000UL;
      break;
    case 3: // Higher/Lower
      hlFirstNum = random(1, 100);
      hlState = HL_SHOW_FIRST;
      hlTimer = millis();
      break;
  }
}
