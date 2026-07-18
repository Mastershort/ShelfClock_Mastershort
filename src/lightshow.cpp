#include "../include/globals.h"
#include "../include/lightshow.h"
#include "../include/display_modes.h"

// ============================================================
// Lightshow Effects – extracted from ShelfClock.cpp
// ============================================================

void Chase() {   //lightshow chase mode
  int chaseMode = random(0, 7);
  if (pastelColors == 0){ lightchaseColorOne = CHSV(random(0, 255), 255, 255); }
  if (pastelColors == 0){ lightchaseColorTwo = CHSV(random(0, 255), 255, 255); }
  if (pastelColors == 1){ lightchaseColorOne = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
  if (pastelColors == 1){ lightchaseColorTwo = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
  for (int i=0; i<SPECTRUM_PIXELS; i++) {  //draw forward
    for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
   int fake =  i * LEDS_PER_SEGMENT;
      if (chaseMode == 0) {LEDs[FAKE_LEDs_C_BLTR[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 1) {LEDs[FAKE_LEDs_C_BRTL[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 2) {LEDs[FAKE_LEDs_C_BMUP[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 3) {LEDs[FAKE_LEDs_C_TMDN[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 4) {LEDs[FAKE_LEDs_C_CMOT[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 5) {LEDs[FAKE_LEDs_C_TLBR[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 6) {LEDs[FAKE_LEDs_C_CSIN[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 7) {LEDs[FAKE_LEDs_C_TRBL[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 8) {LEDs[FAKE_LEDs_C_OUTS[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 9) {LEDs[FAKE_LEDs_C_VERT[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 10) {LEDs[FAKE_LEDs_C_OUTS2[s+((fake))]] = lightchaseColorOne;}
      if (chaseMode == 11) {LEDs[FAKE_LEDs_C_VERT2[s+((fake))]] = lightchaseColorOne;}
    }
    FastLED.show();
  for( int d = 0; d < 40; d++) {server.handleClient(); }  //delay to speed, but so the web buttons still work
  // delay(1);
  for( int d = 0; d < 40; d++) {
      server.handleClient();
      haMqtt.loop();      // MQTT am Leben erhalten
      mqttClient.loop();  // MQTT am Leben erhalten
      delay(1);
  }  //delay to speed, but so the web buttons still work
  }
  for (int i = SPECTRUM_PIXELS-1; i > -1; --i) {   //draw backwards
    for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
   int fake =  i * LEDS_PER_SEGMENT;
      if (chaseMode == 0) {LEDs[FAKE_LEDs_C_BLTR[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 1) {LEDs[FAKE_LEDs_C_BRTL[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 2) {LEDs[FAKE_LEDs_C_BMUP[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 3) {LEDs[FAKE_LEDs_C_TMDN[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 4) {LEDs[FAKE_LEDs_C_CMOT[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 5) {LEDs[FAKE_LEDs_C_TLBR[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 6) {LEDs[FAKE_LEDs_C_CSIN[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 7) {LEDs[FAKE_LEDs_C_TRBL[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 8) {LEDs[FAKE_LEDs_C_OUTS[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 9) {LEDs[FAKE_LEDs_C_VERT[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 10) {LEDs[FAKE_LEDs_C_OUTS2[s+((fake))]] = lightchaseColorTwo;}
      if (chaseMode == 11) {LEDs[FAKE_LEDs_C_VERT2[s+((fake))]] = lightchaseColorTwo;}
    }
    FastLED.show();
  for( int d = 0; d < 40; d++) {server.handleClient(); }  //delay to speed, but so the web buttons still work
  //  delay(1);
  for( int d = 0; d < 40; d++) {
      server.handleClient();
      haMqtt.loop();      // MQTT am Leben erhalten
      mqttClient.loop();  // MQTT am Leben erhalten
      delay(1);
  }  //delay to speed, but so the web buttons still work
  }
  if (clockMode != 5) { allBlank(); }
} //end of chase



void Twinkles() {
  int seed = random(Level);         // A random number. Higher number => fewer twinkles. Use random16() for values >255.
  int i = random(SPECTRUM_PIXELS);         // A random number. Higher number => fewer twinkles. Use random16() for values >255.
  if (seed > 46) {
    CRGB color = CRGB::Black;
    if (pastelColors == 0){ color = CHSV(random(0, 255), 255, 255); }
    if (pastelColors == 1){ color = CRGB(random(0, 255), random(0, 255), random(0, 255)); }
          int fake =  i * LEDS_PER_SEGMENT;
          for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
            LEDs[FAKE_LEDs_C_BLTR[s+((fake))]] = color;
          }
  }

  for (int j = 0; j < SPECTRUM_PIXELS; j++) {
          int fake =  j * LEDS_PER_SEGMENT;
          for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
            LEDs[FAKE_LEDs_C_BLTR[s+((fake))]].fadeToBlackBy(20);
          }
  }
  if (clockMode != 5) { allBlank(); }
} // twinkles



void Rainbow() {
  //fill_gradient_RGB(LEDs, NUM_LEDS, CRGB::Red, CRGB::Yellow, CRGB::Green, CRGB::Blue);
    for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
      int fake =  j * LEDS_PER_SEGMENT;
      for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
        LEDs[FAKE_LEDs_C_BMUP[s+((fake))]] = colorWheel(((255) + colorWheelPosition) % 256);
      }
    }
  EVERY_N_MILLISECONDS(150) {
    colorWheelPosition = colorWheelPosition - colorWheelSpeed; // SPEED OF COLOR WHEEL
  if (colorWheelPosition < 0) // RESET COLOR WHEEL
    colorWheelPosition = 255;
  }
  if (clockMode != 5) { allBlank(); }
} //end of Rainbow



void GreenMatrix() {
  EVERY_N_MILLISECONDS(65) {
    updateMatrix();
    FastLED.show();
 }
  EVERY_N_MILLISECONDS(30) {
    changeMatrixpattern();
  }
} //loop


void changeMatrixpattern () {
  int rand1 = random16 (SPECTRUM_PIXELS);
  int rand2 = random16 (SPECTRUM_PIXELS);
  if ((greenMatrix[rand1] == 1) && (greenMatrix[rand2] == 0) )   //simple get two random dot 1 and 0 and swap it,
  {
    greenMatrix[rand1] = 0;  //this will not change total number of dots
    greenMatrix[rand2] = 1;
  }
  if (clockMode != 5) { allBlank(); }
} //changeMatrixpattern

void initGreenMatrix() {                               //init array of dots. run once
  for (int i = 0; i < SPECTRUM_PIXELS; i++) {
    if (random8(20) == 0) {
      greenMatrix[i] = 1;  //random8(20) number of dots. decrease for more dots
    }
    else {
      greenMatrix[i] = 0;
    }
  }
} //initGreenMatrix

void updateMatrix() {
    for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
      byte layer = greenMatrix[((j + lightshowSpeed + random8(2) + SPECTRUM_PIXELS) % SPECTRUM_PIXELS)];   //fake scroll based on shift coordinate
      // random8(2) add glitchy look
      if (layer) {
          int fake =  j * LEDS_PER_SEGMENT;
          for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
            //LEDs[FAKE_LEDs_C_RAIN[s+((fake))]] = CHSV(110, 255, 255);
            LEDs[FAKE_LEDs_C_RAIN[s+((fake))]] = CRGB::Green;
          }
      }
    }
  lightshowSpeed ++;
    for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
      int fake =  j * LEDS_PER_SEGMENT;
      for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
        LEDs[FAKE_LEDs_C_RAIN[s+((fake))]].fadeToBlackBy( 60 );
      }
    }
} //updateMatrix



void blueRain() {
  //  Serial.println(Level);
    int delayRain = 400-Level;
  EVERY_N_MILLIS_I( thistimer, 400 ) { // initial period = 100ms
thistimer.setPeriod(delayRain);
    updaterain();
 //   FastLED.show();
  }
  if (clockMode != 5) { allBlank(); }
} //loop

void raininit() {                               //init array of dots. run once
  for (int i = 0; i < SPECTRUM_PIXELS; i++) {
    if (random8(24) == 0) {  //30?
      rain[i] = 1;  //random8(20) number of dots. decrease for more dots
    }
    else {
      rain[i] = 0;
    }
  }
} //raininit

void updaterain() {
    for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
      byte layer = rain[((j + lightshowSpeed + 2 + SPECTRUM_PIXELS) % SPECTRUM_PIXELS)];   //fake scroll based on shift coordinate
      // random8(2) add glitchy look
      if (layer) {
          int fake =  j * LEDS_PER_SEGMENT;
          for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
            //LEDs[FAKE_LEDs_C_RAIN[s+((fake))]] = CHSV(110, 255, 255);
            LEDs[FAKE_LEDs_C_RAIN[s+((fake))]] = CRGB::Blue;
          }
      }
    }
  lightshowSpeed ++;
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
      int fake =  j * LEDS_PER_SEGMENT;
      for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
        LEDs[FAKE_LEDs_C_RAIN[s+((fake))]].fadeToBlackBy( 128 );
      }
  }

}

void Fire2021() {
// Array of temperature readings at each simulation cell
  static byte heat[SPECTRUM_PIXELS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < SPECTRUM_PIXELS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, 30));  //55-cooling
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= SPECTRUM_PIXELS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat to the bottom center 3
    if( random8() < 128 ) {  //128-sparkling
      int y = random8(0,2);
      heat[y] = qadd8( heat[y], random8(155,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < SPECTRUM_PIXELS; j++) {
      CRGB color = HeatColor( heat[j]);
      int fake =  j * LEDS_PER_SEGMENT;
      for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
        LEDs[FAKE_LEDs_C_FIRE[s+((fake))]] = color;
      }

    }
  if (clockMode != 5) { allBlank(); }
}



void Snake() {  //real random snake mode with random food changing its color
  int move = 0;
  int fadeby = 130;
  int pickOne = random(4);
  //just write out every possible move, that's all, oh and make it random
  if (snakePosition == 0 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 4; move = 1;} else {snakePosition = 5; move = 1;}}
  if (snakePosition == 0 && snakeLastDirection == 2 && move == 0)  {snakeLastDirection = 3; snakePosition = 1; move = 1;}
  if (snakePosition == 1 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 3; move = 1;} else {snakePosition = 2; move = 1;}}
  if (snakePosition == 1 && snakeLastDirection == 1 && move == 0)  {snakeLastDirection = 0; snakePosition = 0; move = 1;}
  if (snakePosition == 2 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 9; move = 1;} else {snakePosition = 10; move = 1;}}
  if (snakePosition == 2 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 3; move = 1;} else {snakePosition = 1; move = 1;}}
  if (snakePosition == 3 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 4; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 8; move = 1;} else {snakePosition = 7; move = 1;}}
  if (snakePosition == 3 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 1; move = 1;} else {snakeLastDirection = 3; snakePosition = 2; move = 1;}}
  if (snakePosition == 4 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 0; move = 1;} else {snakeLastDirection = 0; snakePosition = 5; move = 1;}}
  if (snakePosition == 4 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 7; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 3; move = 1;} else {snakePosition = 8; move = 1;}}
  if (snakePosition == 5 && snakeLastDirection == 0 && move == 0)  {snakeLastDirection = 3; snakePosition = 6; move = 1;}
  if (snakePosition == 5 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 3; snakePosition = 4; move = 1;} else {snakeLastDirection = 2; snakePosition = 0; move = 1;}}
  if (snakePosition == 6 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 7; move = 1;} else {snakePosition = 15; move = 1;}}
  if (snakePosition == 6 && snakeLastDirection == 1 && move == 0)  {snakeLastDirection = 2; snakePosition = 5; move = 1;}
  if (snakePosition == 7 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 6; move = 1;} else {snakeLastDirection = 3; snakePosition = 15; move = 1;}}
  if (snakePosition == 7 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 4; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 8; move = 1;} else {snakePosition = 3; move = 1;}}
  if (snakePosition == 8 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 3; move = 1;} else if (pickOne == 3) {snakeLastDirection = 0; snakePosition = 7; move = 1;} else {snakePosition = 4; move = 1;}}
  if (snakePosition == 8 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 14; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 9; move = 1;} else {snakePosition = 13; move = 1;}}
  if (snakePosition == 9 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 8; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 13; move = 1;} else {snakePosition = 14; move = 1;}}
  if (snakePosition == 9 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 2; move = 1;} else {snakeLastDirection = 3; snakePosition = 10; move = 1;}}
  if (snakePosition == 10 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 12; move = 1;} else {snakePosition = 11; move = 1;}}
  if (snakePosition == 10 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 9; move = 1;} else {snakePosition = 2; move = 1;}}
  if (snakePosition == 11 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 19; move = 1;} else {snakePosition = 20; move = 1;}}
  if (snakePosition == 11 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 12; move = 1;} else {snakePosition = 10; move = 1;}}
  if (snakePosition == 12 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 13; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 18; move = 1;} else {snakePosition = 17; move = 1;}}
  if (snakePosition == 12 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 10; move = 1;} else {snakeLastDirection = 3; snakePosition = 11; move = 1;}}
  if (snakePosition == 13 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 9; move = 1;} else if (pickOne == 3) {snakeLastDirection = 0; snakePosition = 14; move = 1;} else {snakePosition = 8; move = 1;}}
  if (snakePosition == 13 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 17; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 12; move = 1;} else {snakePosition = 18; move = 1;}}
  if (snakePosition == 14 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 15; move = 1;} else {snakeLastDirection = 3; snakePosition = 16; move = 1;}}
  if (snakePosition == 14 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 8; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 13; move = 1;} else {snakePosition = 9; move = 1;}}
  if (snakePosition == 15 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 14; move = 1;} else {snakePosition = 16; move = 1;}}
  if (snakePosition == 15 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 7; move = 1;} else {snakeLastDirection = 1; snakePosition = 6; move = 1;}}
  if (snakePosition == 16 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 17; move = 1;} else {snakePosition = 25; move = 1;}}
  if (snakePosition == 16 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 14; move = 1;} else {snakeLastDirection = 1; snakePosition = 15; move = 1;}}
  if (snakePosition == 17 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 16; move = 1;} else {snakeLastDirection = 3; snakePosition = 25; move = 1;}}
  if (snakePosition == 17 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 13; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 18; move = 1;} else {snakePosition = 12; move = 1;}}
  if (snakePosition == 18 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 12; move = 1;} else if (pickOne == 3) {snakeLastDirection = 0; snakePosition = 17; move = 1;} else {snakePosition = 13; move = 1;}}
  if (snakePosition == 18 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 24; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 19; move = 1;} else {snakePosition = 23; move = 1;}}
  if (snakePosition == 19 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 18; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 23; move = 1;} else {snakePosition = 24; move = 1;}}
  if (snakePosition == 19 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 11; move = 1;} else {snakeLastDirection = 3; snakePosition = 20; move = 1;}}
  if (snakePosition == 20 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 22; move = 1;} else {snakePosition = 21; move = 1;}}
  if (snakePosition == 20 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 19; move = 1;} else {snakePosition = 11; move = 1;}}
  if (snakePosition == 21 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 29; move = 1;} else {snakePosition = 30; move = 1;}}
  if (snakePosition == 21 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 22; move = 1;} else {snakePosition = 20; move = 1;}}
  if (snakePosition == 22 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 23; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 28; move = 1;} else {snakePosition = 27; move = 1;}}
  if (snakePosition == 22 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 20; move = 1;} else {snakeLastDirection = 3; snakePosition = 21; move = 1;}}
  if (snakePosition == 23 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 19; move = 1;} else if (pickOne == 3) {snakeLastDirection = 0; snakePosition = 24; move = 1;} else {snakePosition = 18; move = 1;}}
  if (snakePosition == 23 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 27; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 22; move = 1;} else {snakePosition = 28; move = 1;}}
  if (snakePosition == 24 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 25; move = 1;} else {snakeLastDirection = 3; snakePosition = 26; move = 1;}}
  if (snakePosition == 24 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 18; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 23; move = 1;} else {snakePosition = 19; move = 1;}}
  if (snakePosition == 25 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 24; move = 1;} else {snakePosition = 26; move = 1;}}
  if (snakePosition == 25 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 17; move = 1;} else {snakeLastDirection = 1; snakePosition = 16; move = 1;}}
  if (snakePosition == 26 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 27; move = 1;} else {snakePosition = 34; move = 1;}}
  if (snakePosition == 26 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 24; move = 1;} else {snakeLastDirection = 1; snakePosition = 25; move = 1;}}
  if (snakePosition == 27 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 26; move = 1;} else {snakeLastDirection = 3; snakePosition = 34; move = 1;}}
  if (snakePosition == 27 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 23; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 28; move = 1;} else {snakePosition = 22; move = 1;}}
  if (snakePosition == 28 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 22; move = 1;} else if (pickOne == 3) {snakeLastDirection = 0; snakePosition = 27; move = 1;} else {snakePosition = 23; move = 1;}}
  if (snakePosition == 28 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 33; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 29; move = 1;} else {snakePosition = 32; move = 1;}}
  if (snakePosition == 29 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 28; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 32; move = 1;} else {snakePosition = 33; move = 1;}}
  if (snakePosition == 29 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 21; move = 1;} else {snakeLastDirection = 3; snakePosition = 30; move = 1;}}
  if (snakePosition == 30 && snakeLastDirection == 3 && move == 0)  {snakeLastDirection = 0; snakePosition = 31; move = 1;}
  if (snakePosition == 30 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 0) {snakeLastDirection = 0; snakePosition = 29; move = 1;} else {snakePosition = 21; move = 1;}}
  if (snakePosition == 31 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 32; move = 1;} else {snakePosition = 36; move = 1;}}
  if (snakePosition == 31 && snakeLastDirection == 2 && move == 0)  {snakeLastDirection = 1; snakePosition = 30; move = 1;}
  if (snakePosition == 32 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 29; move = 1;} else if (pickOne == 3) {snakeLastDirection = 0; snakePosition = 33; move = 1;} else {snakePosition = 28; move = 1;}}
  if (snakePosition == 32 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 0; snakePosition = 36; move = 1;} else if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 31; move = 1;}}
  if (snakePosition == 33 && snakeLastDirection == 0 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 34; move = 1;} else {snakeLastDirection = 3; snakePosition = 35; move = 1;}}
  if (snakePosition == 33 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 28; move = 1;} else if (pickOne == 3) {snakeLastDirection = 3; snakePosition = 32; move = 1;} else {snakePosition = 29; move = 1;}}
  if (snakePosition == 34 && snakeLastDirection == 3 && move == 0)  {if (pickOne == 3) {snakeLastDirection = 2; snakePosition = 33; move = 1;} else {snakePosition = 35; move = 1;}}
  if (snakePosition == 34 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 27; move = 1;} else {snakeLastDirection = 1; snakePosition = 26; move = 1;}}
  if (snakePosition == 35 && snakeLastDirection == 3 && move == 0)  {snakeLastDirection = 2; snakePosition = 36; move = 1;}
  if (snakePosition == 35 && snakeLastDirection == 1 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 2; snakePosition = 33; move = 1;} else {snakeLastDirection = 1; snakePosition = 34; move = 1;}}
  if (snakePosition == 36 && snakeLastDirection == 0 && move == 0)  {snakeLastDirection = 1; snakePosition = 35; move = 1;}
  if (snakePosition == 36 && snakeLastDirection == 2 && move == 0)  {if (pickOne == 1) {snakeLastDirection = 1; snakePosition = 32; move = 1;} else {snakePosition = 31; move = 1;}}
  if (snakePosition == foodSpot) { oldsnakecolor = spotcolor;  snakeWaiting = 1; foodSpot = 40;}  //did snake find the food, change snake color
  if (snakeWaiting > 0) {snakeWaiting = snakeWaiting + 1;} //counting while waiting

  if (snakeWaiting > random((30/Level),(600/Level))) {snakeWaiting = 0; foodSpot = random(SPECTRUM_PIXELS-1); spotcolor = CHSV(random(0, 255), 255, 255); if (getSlower > 3){  getSlower =  getSlower / 3;  }}  //waiting time is up, pick new spot, new color, reset speed
  if (getSlower > 1000){  getSlower = 1000;  }
  int fake =  foodSpot * LEDS_PER_SEGMENT;   //draw food
  for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
    if (foodSpot != 40)     {  LEDs[FAKE_LEDs_C_BRTL[s+((fake))]] = spotcolor; }  //draw food, but not while waiting
  }
  int fake2 =  snakePosition * LEDS_PER_SEGMENT;   //draw snake
  for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
    LEDs[FAKE_LEDs_C_BRTL[s+((fake2))]] = oldsnakecolor;
  }
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {  //slowly erase snake
    int fake3 =  j * LEDS_PER_SEGMENT;
    for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
      LEDs[FAKE_LEDs_C_BRTL[s+((fake3))]].fadeToBlackBy( fadeby );
     }
  }
  getSlower = getSlower + 40;
  if (clockMode != 5) { allBlank(); }
} //snake

void Cylon() {
  int fake = 0;
  const uint8_t CYLON[12] = {4,8,13,18,23,28,32,28,23,18,13,8};
  if (cylonPosition >=12) { cylonPosition = 0;}
  fake = CYLON[cylonPosition] * LEDS_PER_SEGMENT;
  for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){LEDs[FAKE_LEDs_SNAKE[s+((fake))]] = CRGB::Red;}
  cylonPosition++;
  if (cylonPosition >=12) { cylonPosition = 0;}
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake =  j * LEDS_PER_SEGMENT;
    for (byte s=0; s<LEDS_PER_SEGMENT; s++ ){              // 7 LEDs per segment
      LEDs[FAKE_LEDs_SNAKE[s+((fake))]].fadeToBlackBy( 120 );
    }
  }
  if (clockMode != 5) { allBlank(); }
} //Cylon

void Breathing() {  // Smooth pulse/breathing effect
  static uint8_t breathPhase = 0;
  uint8_t val = sin8(breathPhase);  // smooth sine wave 0-255
  CRGB color;
  if (pastelColors == 0) {
    color = CHSV(colorWheelPosition, 255, val);
  } else {
    color = CRGB(random(0, 255), random(0, 255), random(0, 255));
    color.nscale8(val);
  }
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] = color;
    }
  }
  breathPhase += 2;
  EVERY_N_SECONDS(4) {
    colorWheelPosition += 32;  // shift hue every 4 seconds
  }
  if (clockMode != 5) { allBlank(); }
} //Breathing

void Sparkle() {  // Random white sparkles over a dim base color
  static CRGB baseColor = CRGB::Blue;
  EVERY_N_SECONDS(5) {
    if (pastelColors == 0) {
      baseColor = CHSV(random(0, 255), 255, 80);
    } else {
      baseColor = CRGB(random(20, 60), random(20, 60), random(20, 60));
    }
  }
  // Set all to dim base
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] = baseColor;
    }
  }
  // Add random sparkles
  for (int i = 0; i < 3; i++) {
    int pos = random(SPECTRUM_PIXELS);
    int fake = pos * LEDS_PER_SEGMENT;
    CRGB sparkColor = CRGB::White;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] = sparkColor;
    }
  }
  if (clockMode != 5) { allBlank(); }
} //Sparkle

void MeteorRain() {  // Falling meteor trails
  static int meteorPos = 0;
  static CRGB meteorColor = CRGB::Cyan;
  // Fade all LEDs
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      if (random8() > 80) {  // random fade for trail effect
        LEDs[FAKE_LEDs_C_RAIN[s + fake]].fadeToBlackBy(100);
      }
    }
  }
  // Draw meteor head (3 segments bright)
  for (int i = 0; i < 3; i++) {
    int pos = meteorPos - i;
    if (pos >= 0 && pos < SPECTRUM_PIXELS) {
      int fake = pos * LEDS_PER_SEGMENT;
      uint8_t bright = 255 - (i * 80);
      CRGB c = meteorColor;
      c.nscale8(bright);
      for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
        LEDs[FAKE_LEDs_C_RAIN[s + fake]] = c;
      }
    }
  }
  meteorPos++;
  if (meteorPos > SPECTRUM_PIXELS + 10) {
    meteorPos = 0;
    if (pastelColors == 0) {
      meteorColor = CHSV(random(0, 255), 255, 255);
    } else {
      meteorColor = CRGB(random(128, 255), random(128, 255), random(128, 255));
    }
  }
  if (clockMode != 5) { allBlank(); }
} //MeteorRain

void ColorWipe() {  // Color fills segment by segment, then wipes with new color
  static int wipePos = 0;
  static CRGB wipeColor = CRGB::Red;
  static bool wipingForward = true;
  if (wipePos < SPECTRUM_PIXELS) {
    int idx = wipingForward ? wipePos : (SPECTRUM_PIXELS - 1 - wipePos);
    int fake = idx * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] = wipeColor;
    }
    wipePos++;
  } else {
    wipePos = 0;
    wipingForward = !wipingForward;
    if (pastelColors == 0) {
      wipeColor = CHSV(random(0, 255), 255, 255);
    } else {
      wipeColor = CRGB(random(0, 255), random(0, 255), random(0, 255));
    }
  }
  if (clockMode != 5) { allBlank(); }
} //ColorWipe

void Plasma() {  // Organic flowing plasma effect
  static uint16_t plasmaTime = 0;
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    // Create organic color from multiple sine waves
    uint8_t hue = sin8(j * 15 + plasmaTime) / 2 +
                  sin8(j * 7 - plasmaTime / 2) / 2;
    uint8_t val = sin8(j * 10 + plasmaTime / 3);
    val = map(val, 0, 255, 100, 255);  // minimum brightness 100
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] = CHSV(hue, 220, val);
    }
  }
  plasmaTime += 3;
  if (clockMode != 5) { allBlank(); }
} //Plasma


// ============================================================
// NEW EFFECTS
// ============================================================

void Confetti() {  // Random colorful pixels popping up and fading like confetti
  // Fade all segments slowly
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]].fadeToBlackBy(15);
    }
  }
  // Light up a few random segments with vivid colors
  for (int i = 0; i < 2; i++) {
    int pos = random(SPECTRUM_PIXELS);
    int fake = pos * LEDS_PER_SEGMENT;
    CRGB color;
    if (pastelColors == 0) {
      color = CHSV(random8(), 200, 255);
    } else {
      color = CRGB(random(100, 255), random(100, 255), random(100, 255));
    }
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] += color;
    }
  }
  if (clockMode != 5) { allBlank(); }
} //Confetti

void Juggle() {  // Multiple colored dots bouncing at different speeds
  // Fade all segments
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]].fadeToBlackBy(25);
    }
  }
  // Draw multiple bouncing dots at different speeds
  for (int i = 0; i < 4; i++) {
    // beatsin16 returns a sine value that oscillates, creating a bounce effect
    int pos = beatsin16(i + 5, 0, SPECTRUM_PIXELS - 1);
    int fake = pos * LEDS_PER_SEGMENT;
    CRGB color = CHSV(i * 64, 200, 255);
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] |= color;
    }
  }
  if (clockMode != 5) { allBlank(); }
} //Juggle

void Heartbeat() {  // Double-pulse heartbeat pattern (lub-dub)
  static uint16_t beatPhase = 0;
  static uint8_t hueShift = 0;

  // Heartbeat waveform: two quick bright pulses followed by a longer pause
  // Phase 0-40: first pulse (lub)
  // Phase 41-60: brief dip
  // Phase 61-100: second pulse (dub)
  // Phase 101-255: long pause
  uint8_t val = 0;
  if (beatPhase < 40) {
    val = sin8(map(beatPhase, 0, 40, 0, 255));
  } else if (beatPhase < 60) {
    val = 30;  // brief dim between beats
  } else if (beatPhase < 100) {
    val = sin8(map(beatPhase, 60, 100, 0, 255));
    val = val * 3 / 4;  // second beat slightly softer
  } else {
    val = 0;  // pause
  }

  CRGB color;
  if (pastelColors == 0) {
    color = CHSV(hueShift, 255, val);
  } else {
    color = CRGB(val, val / 4, val / 4);  // reddish heartbeat
  }

  // Light all segments with the heartbeat intensity
  for (byte j = 0; j < SPECTRUM_PIXELS; j++) {
    int fake = j * LEDS_PER_SEGMENT;
    for (byte s = 0; s < LEDS_PER_SEGMENT; s++) {
      LEDs[FAKE_LEDs_C_BLTR[s + fake]] = color;
    }
  }

  beatPhase += 2;
  if (beatPhase > 255) { beatPhase = 0; }

  EVERY_N_SECONDS(8) {
    hueShift += 20;  // slowly shift color over time
  }
  if (clockMode != 5) { allBlank(); }
} //Heartbeat


// ============================================================
// PIXEL-LEVEL EFFECTS (operate on 333 individual LEDs)
// ============================================================

void PixelRainbow() {  // Full-spectrum rainbow gradient flowing across all 333 LEDs
  static uint16_t rainbowOffset = 0;
  for (int i = 0; i < SEGMENTS_LEDS; i++) {
    uint8_t hue = (uint8_t)((i * 256L / SEGMENTS_LEDS) + rainbowOffset);
    LEDs[FAKE_LEDs_C_BLTR[i]] = CHSV(hue, 255, 255);
  }
  rainbowOffset += 2;
  if (clockMode != 5) { allBlank(); }
} //PixelRainbow

void PixelWave() {  // Sine-wave color wave flowing across individual LEDs
  static uint16_t wavePhase = 0;
  for (int i = 0; i < SEGMENTS_LEDS; i++) {
    uint8_t brightness = sin8((i * 8) + wavePhase);
    uint8_t hue = (i * 2) + (wavePhase / 3);
    LEDs[FAKE_LEDs_C_BLTR[i]] = CHSV(hue, 255, brightness);
  }
  wavePhase += 3;
  if (clockMode != 5) { allBlank(); }
} //PixelWave

void PixelFireworks() {  // Random pixel bursts with radial fade
  // Fade all pixels
  for (int i = 0; i < SEGMENTS_LEDS; i++) {
    LEDs[FAKE_LEDs_C_BMUP[i]].fadeToBlackBy(30);
  }
  // Randomly launch new bursts
  if (random8() < 40) {
    int center = random(SEGMENTS_LEDS);
    CRGB burstColor;
    if (pastelColors == 0) {
      burstColor = CHSV(random8(), 255, 255);
    } else {
      burstColor = CRGB(random(100, 255), random(100, 255), random(100, 255));
    }
    // Light the center and a few neighbors
    int spread = random(3, 8);
    for (int s = -spread; s <= spread; s++) {
      int idx = center + s;
      if (idx >= 0 && idx < SEGMENTS_LEDS) {
        uint8_t bright = 255 - (abs(s) * (200 / spread));
        CRGB c = burstColor;
        c.nscale8(bright);
        LEDs[FAKE_LEDs_C_BMUP[idx]] += c;
      }
    }
  }
  if (clockMode != 5) { allBlank(); }
} //PixelFireworks

void PixelRunningLights() {  // Classic running-lights at pixel level
  static uint16_t runPhase = 0;
  static uint8_t runHue = 0;
  for (int i = 0; i < SEGMENTS_LEDS; i++) {
    uint8_t wave = sin8(i * 10 + runPhase);
    CRGB color;
    if (pastelColors == 0) {
      color = CHSV(runHue, 255, wave);
    } else {
      color = CRGB(wave, wave / 2, wave / 3);
    }
    LEDs[FAKE_LEDs_C_BLTR[i]] = color;
  }
  runPhase += 4;
  EVERY_N_SECONDS(3) {
    runHue += 25;
  }
  if (clockMode != 5) { allBlank(); }
} //PixelRunningLights

void PixelTheaterChase() {  // Theater-style chase at pixel level
  static uint8_t chaseStep = 0;
  static uint8_t chaseHue = 0;
  for (int i = 0; i < SEGMENTS_LEDS; i++) {
    if ((i + chaseStep) % 3 == 0) {
      LEDs[FAKE_LEDs_C_BLTR[i]] = CHSV(chaseHue + (i * 2), 255, 255);
    } else {
      LEDs[FAKE_LEDs_C_BLTR[i]] = CRGB::Black;
    }
  }
  chaseStep++;
  if (chaseStep >= 3) { chaseStep = 0; }
  chaseHue += 1;
  if (clockMode != 5) { allBlank(); }
} //PixelTheaterChase

void StarrySky() {  // Night sky: single pixels ignite as stars, twinkle and slowly fade out
  for (int i = 0; i < SEGMENTS_LEDS; i++) {
    LEDs[i].nscale8(250);  // gentle exponential fade - a star glows for a few seconds
  }
  if (random8() < 40) {  // ignite a new star now and then
    int led = random16(SEGMENTS_LEDS);
    if (random8() < 60) {
      LEDs[led] = CHSV(32, 120, 150 + random8(105));       // warm yellowish star
    } else {
      LEDs[led] = CHSV(160, random8(60), 120 + random8(135)); // cold blue-white star
    }
  }
  if (random16() < 60) {  // rare bright flash
    LEDs[random16(SEGMENTS_LEDS)] = CRGB::White;
  }
  if (clockMode != 5) { allBlank(); }
} //StarrySky
