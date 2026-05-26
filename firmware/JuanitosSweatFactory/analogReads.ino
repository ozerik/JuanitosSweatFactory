void analogReads() {
  RTPots[0] = (analogRead(PIN_PD4) >> 2);  // this >> 2 means we're making these values 1024, the same as the resolution of the DAC
  RTPots[1] = (analogRead(PIN_PD5) >> 2);  // this >> 2 means we're making these values 1024, the same as the resolution of the DAC
  RTPots[2] = (analogRead(PIN_PE0) >> 2);
  RTPots[3] = (analogRead(PIN_PE1) >> 2);
  RTPots[4] = (analogRead(PIN_PE4) >> 2);
  RTPots[5] = (analogRead(PIN_PE3) >> 2);
  RTPots[6] = (analogRead(PIN_PE2) >> 2);
  RTPots[7] = (analogRead(PIN_PE5) >> 2);
  topRowPots[0] = analogRead(PIN_PD0);  // CV jack top left, clock input mostly
  topRowPots[1] = analogRead(PIN_PD1);  // top row pots without switches under them
  topRowPots[2] = analogRead(PIN_PD2);  // top row pots without switches under them
  // topRowPots[3] = analogRead(PIN_PD3);  // CV jack top right REMOVING THIS FROM ANALOG READS because right now it's a gate input
  arPE7 = analogRead(PIN_PE7);  // clock pot, high res
  arPD7 = analogRead(PIN_PD7);  // envelope pot, high res

  // okay here's where we check the pots against if they're picked up, and if they are, record new ACTUAL values
  for (byte i = 0; i < 8; i++) {
    if (!potNeedCatch[i]) circlePots[i] = RTPots[i];
  }








  topRowPots[1] = topRowPots[1] * 3;  // these are temporaryyyyyyyyyyyyyy the 2nd prototype hardware will have the gain fixed
  topRowPots[2] = topRowPots[2] * 2;  // these are temporaryyyyyyyyyyyyyy the 2nd prototype hardware will have the gain fixed

  // the following part handles the reading of the middle two pots, and outputs the proper mode and metaMode variables
  {
    /* variables:
      mode = what number mode using
      oldModePot = old pot value, high res
      newModePot = new pot value, high res
      metaMode = what the metaMode is
      */
    static int oldModePotValue;
    static byte oldMode;
    static byte newMode;
    newMode = constrain(map(topRowPots[1], 0, 4055, 0, 5), 0, 4);  // 0, linear; 1, cylon; 2, pattern bank A; 3, pattern bank B; 4, user-saved patterns????
    if (newMode != oldMode && abs(topRowPots[1] - oldModePotValue) > jitterDefeater) {
      mode = oldMode = newMode;       // get that jitter out of here!
      currentStep = currentStep % 8;  // gets rid of the possibly gigantic number that may have accumulated without changing the actual step that's current
      oldModePotValue = topRowPots[1];
    }
    if (mode == 0) {
      targetHue[1] = 0;
      targetSat[1] = 255;
      targetVal[1] = 155;
    } else if (mode == 1) {
      targetHue[1] = 128;
      targetSat[1] = 255;
      targetVal[1] = 50;
    } else if (mode == 2) {
      targetHue[1] = 77;
      targetSat[1] = 255;
      targetVal[1] = 155;
    } else if (mode == 3) {
      targetHue[1] = 200;
      targetSat[1] = 200;
      targetVal[1] = 180;
    } else if (mode == 4) {
      targetHue[1] = 0;
      targetSat[1] = 255;
      targetVal[1] = 100;
    }
    if (topRowPots[1] > 4090) {  // this part turns the metaMode pot into a step selector, to more easily tune the knobs
      doStepSelection = true;
      targetSat[1] = 0;
      targetVal[1] = 255;
    } else doStepSelection = false;

    static int oldMetaModePotValue;
    static byte oldMetaMode;
    static byte newMetaMode;
    if (mode == 0) {                                                       // messy and disorganized, but let's do LED stuff here too
      newMetaMode = constrain(map(topRowPots[2], 0, 4095, 0, 16), 0, 15);  // here's the anti-jitter strategy for the mode pot. Doing this all over the sketch
      if (newMetaMode == 8) newMetaMode = 7;                               // *SIGH* I am bad at coding. This avoids having two spots in the pot that doesn't do anything
      if (newMetaMode != oldMetaMode && abs(topRowPots[2] - oldMetaModePotValue) > 70) {
        metaMode = oldMetaMode = newMetaMode;
        oldMetaModePotValue = topRowPots[2];
      }
      targetHue[2] = metaMode << 5;               // zero through 255
      targetSat[2] = (metaMode & 1) ? 250 : 255;  // a ternary function, the very first one I ever wrote! Is metaMode value even or odd? saturation numbers
      targetVal[2] = (metaMode & 1) ? 35 : 180;   // another ternary function what the
      if (metaMode == 7) {                        // oops, the pot is where there's no motion
        targetSat[2] = 0;                         // when the thing just sits in one spot, make the LED dim and white
        targetVal[2] = 30;
      }

    } else if (mode == 1) {                                                               // CYLON MODE!!!
      newMetaMode = constrain(map(topRowPots[2], 0, 4095, 0, 12), 0, 11);                 // here's the anti-jitter strategy again
      if (newMetaMode != oldMetaMode && abs(topRowPots[2] - oldMetaModePotValue) > 70) {  //
        metaMode = oldMetaMode = newMetaMode;                                             //
        oldMetaModePotValue = topRowPots[2];                                              //
      }
      targetHue[2] = ~metaMode << 4;              // zero through 255 also inverts the bits to make it different
      targetSat[2] = (metaMode & 1) ? 250 : 255;  // a ternary function, a copy of the very first one I ever wrote! Is metaMode value even or odd? saturation numbers
      targetVal[2] = (metaMode & 1) ? 35 : 180;   // another ternary function what the

    } else {                                                                              // here's PATTERN MODE! Each bank has six patterns, there's um, two banks? Two or three.
      newMetaMode = constrain(map(topRowPots[2], 0, 4065, 0, 6), 0, 5);                   // here's the anti-jitter strategy for the mode pot. Doing this all over the sketch
      if (newMetaMode != oldMetaMode && abs(topRowPots[2] - oldMetaModePotValue) > 70) {  // watches the movement of the analogRead, and if it has moved more than 70...
        metaMode = oldMetaMode = newMetaMode;                                             // well, the new value gets entered!!!
        oldMetaModePotValue = topRowPots[2];                                              // stores the old analogRead value
      }
      targetHue[2] = metaMode << 6;               // sets the color of the pot, red to pink
      targetSat[2] = 252;                         // just a bit less saturated than full
      targetVal[2] = (metaMode & 1) ? 100 : 255;  // another ternary function. Flips from brighter and dimmer
    }
    if (doStepSelection) {                                 // this part lets you choose which step pot is current, and you can tune them this way
      currentStep = map(topRowPots[2], 0, 4095, 5, 12);    // 5 to 12 what the???
      if (currentStep > 7) currentStep = currentStep - 8;  // see, the pointer of the metaMode pot points sorta in the direction of the current pot
      targetHue[2] = targetHue[currentStep + 4];           // yeah, gotta do math because currentPot needs to roll over
      writeDAC(circlePots[currentStep & 0x07]);            // draw the LED color without latency (200Hz I guess)
    }
  }

  for (byte i = 0; i < 8; i++) {                           // this loop gets the circlePot LEDs ready
    cirLEDs[i] = map(circlePots[i] >> 2, 0, 255, 0, 185);  // the ">> 2" means "divide by 4"
  }


  static bool firstRun = true;                          // all the envelope parameters should be what the pot
  if (firstRun) {                                       // is set to.
    attack = decay = sustain = release = (arPD7 >> 4);  // this line sets the envelope parameters to whatever the envelope pot is at startup
    firstRun = false;                                   // but just do it once, not every time through
  }
}