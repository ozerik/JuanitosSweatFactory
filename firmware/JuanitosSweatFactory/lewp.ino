void lewp() {

  static bool oldShift;                                // shift state tracker for debouncing
  shift = !(PORTA.IN & (1 << 7));                      // shift key (button) handler. This reads the position of the key
  if (shift != oldShift) {                             // has the value changed? this happens only once per debounce
    oldEnvPotValue = arPD7;                            // snapshot of arPD7 (which is the envelope pot value)
    if (millis() - shiftPressedDebounce > debounce) {  // debounce timer
      shiftPressedDebounce = millis();                 // when the debounce has elapsed, restart the timer
      oldShift = shift;                                // and say "hey, the key/button is in a constant state"
    }                                                  // debounce done
                                                       // this next part runs just once per shift button press:
    if (shift) {                                       // ONCE PER PRESS!!! is the key pressed?
      if (millis() - doubleClickTimer < 280) {         // looks for a quick double click
        doubleClick = true;                            // okay I guess double-click happened
        doubleClickTimer = 0;                          // probably prevents triple-clicks from turnning double-click mode off
        shiftMode++;                                   // adds 1 to shiftmode. So far, shiftMode is just 0 or 1, I guess it can go farther that's why this isn't a bool
        if (shiftMode > 1) {                           // This means "TIME TO GO BACK TO NORMAL MODE"
          EEPROM.update(RANGE_LSB, CVRange + 0xFF);
          EEPROM.update(RANGE_MSB, (CVRange >> 8) & 0xFF);
          EEPROM.update(QUANTMODE, qMode);
          EEPROM.update(BRIGHTNESS, brightness);
          shiftMode = 0;
        } else {  // this means "GOIN IN TO SHIFTMODE 1" or 2, if there's more modes added later :/
          CVRCheck = circlePots[6];
          QMCheck = circlePots[2];
          BRCheck = circlePots[4];
          CVRAdjust = false;
          QMAdjust = false;
          BRAdjust = false;
        }

        shiftStep = currentStep;           // just for doubleClick mode :D
      }                                    // that's all
      doubleClickTimer = millis();         // timer for is this a doubleclick?
      for (byte i = 0; i < 8; i++) {       // a for loop to....
        potNeedCatch[i] = false;           // hmm, maybe this is the name of the variable I used for this
        oldPotsValue[i] = TcirclePots[i];  // record the current pots into this value
      }                                    // 0 to 1024, by the way
                                           // and t his next part just just once per shift button RELEASED
      if (firstClock == false) {           // and if shift is true???
        extClock = false;                  // says "HEY NO MORE EXTERNAL CLOCKIN PLEASE"
        firstClock = true;                 // and lets the next clock impulse be "first" for calculation purposes
        TCB0.CTRLA |= TCB_ENABLE_bm;       // start timer again for normal internal clock mode
      }
    } else {                // this code below here only happens when the shift key is RELEASED!!!! So clever :rolleyes:
      doubleClick = false;  // no longer holding the shift button
      // if I can think of anything to do when the shift key is released, I should put it here
    }
  }
  if (shift) {                                    // EVERY every lewp()
    if ((oldEnvPotValue >> 7) != (arPD7 >> 7)) {  // envPot changed, 128 values either direction
      if (oldEnvPotValue < arPD7) {               // the pot was turned UP
        record = false;                           // stop recording, just playback
      } else {                                    // okay go back to recording, because the pot was turned down
        record = true;                            // yup, RECORD NOW
      }                                           // I THINK THIS IS WORKING NOW
      envPotPickedUp = false;                     // omg this might be enough to get it back picked up after toggling trackFork
    }
  }

  // I think this works VVVVV this part turns SLEW on per-pot
  // NOPE GOT RID OF DIFFERENT SHIFT MODES shift mode one, adjust slew value between steps
  if (shift == true) {                                          // Every Loop. if shift key is pressed?
    for (byte i = 0; i < 8; i++) {                              // run around the circle of pots WHILE SHIFT is being held
      if (((oldPotsValue[i] >> 6) != (TcirclePots[i] >> 6))) {  // the >> 6 introduces a 64-value buffer zone for jitter
        potNeedCatch[i] = true;                                 // it's true. We're all looking for whoever is responsible for this

        if (TcirclePots[i] > oldPotsValue[i]) {  // this checks if WHOEVER IT WAS turned the knob up?
          slewValue[i] = constrain(((TcirclePots[i] - oldPotsValue[i]) >> 6), 0, 2);
        } else if (slewValue[i] > 0) {  // slew needs to be turned down
          slewValue[i] = 0;             // makes slew just be zero
        }
        targetHue[i + 4] = 210;  // reddish pink?
        targetVal[i + 4] = constrain(slewValue[i] << 3, 0, 255);
        targetSat[i + 4] = 255;
      } else if (potNeedCatch[i]) {
        targetVal[i + 4] = 0;
        slewValue[i] = 0;
      }
    }








    // todo this part is NO GOOD because it interrupts the normal extClock playback, fix it somehow later
    // if (micros() - extNowReal > 0xFFFFF) {
    // if (firstClock == false) {  // and if shift is true???
    //   extClock = false;             // says "HEY NO MORE EXTERNAL CLOCKIN PLEASE"
    //   firstClock = true;            // and lets the next clock impulse be "first" for calculation purposes
    //   TCB0.CTRLA |= TCB_ENABLE_bm;  // start timer again for normal internal clock mode
    // }
    // }
  } else {
    for (byte i = 0; i < 8; i++) {
      if (potNeedCatch[i]) {                           // this part is to re-catch the circle pots to take effect again
        int cDiff = oldPotsValue[i] - TcirclePots[i];  // this part
        if (cDiff > 10) {
          targetHue[i + 4] = 160;
          targetSat[i + 4] = 255;
          targetVal[i + 4] = map(cDiff, 0, 1024, 5, 255);
        } else if (cDiff < -10) {
          targetHue[i + 4] = 0;    // red
          targetSat[i + 4] = 255;  // saturated as heck
          targetVal[i + 4] = abs(map(cDiff, 0, 1024, 5, 255));
        } else {                   // okay, this is the first blink of the flashieflashy knob-picked-up part
          targetSat[i + 4] = 0;    // white
          targetVal[i + 4] = 255;  // white
          potNeedCatch[i] = false;
          CPotTimer[i] = millis();
        }
      }
    }
  }

  // I think this part ^^^^ works now?

  for (byte i = 0; i < 8; i++) {          // here's the part where the circlePots get to flash. These numbers are different -- the total flash time
    if (millis() - CPotTimer[i] < 100) {  // is 1/10 second, not a quarter second
      targetVal[i + 4] = 255;             // bright
      targetSat[i + 4] = 0;               // white
      if (millis() - CPotTimer[i] > 25) {
        targetVal[i + 4] = 0;
        targetSat[i + 4] = 0;
      }
      if (millis() - CPotTimer[i] > 50) {
        targetVal[i + 4] = 255;
        targetSat[i + 4] = 0;
      }
      if (millis() - CPotTimer[i] > 75) {
        targetVal[i + 4] = 0;
        targetSat[i + 4] = 0;
      }
    }
  }


  // analog read, write LEDs, and glide subroutines. Run 330 times per second or whatever
  if (clockPotPickedUp == false || envPotPickedUp == false) analogReads();
  if (millis() - aTimer > 3) {  // run it 330 times per second




    analogReads();                              // analog reads all 12 analog pin inputs
    aTimer = millis();                          // reset the timer
    targetCV = circlePots[currentStep & 0x07];  //Nope, do NOT divide by 4, already done in analogReads() DUH
    glide();                                    // run the glide part
    // if (doubleClick == true) {                  // watch for doubleclick
    //   // currentCV = circlePots[shiftStep];        // keep value on the step it's on!
    //   for (byte i = 0; i < 8; i++) {
    //   }
    // }















    writeLEDs();                                     // runs the "write LEDs" subroutine
    rangedCV = map(currentCV, 0, 1023, 0, CVRange);  // rangify that currentCV
    // okay does quantize go here?????? Let's try it
    rangedCV = quantize(rangedCV);
    writeDAC(rangedCV);  // maps currentCV to range, and then writes it to the DAC value
  }                      // this code works great!



  envPressed = !(PORTB.IN & (1 << 2));  // is true while button pressed
  if (envPressed != oldEnvPressed) {    // here's how we choose the parameter of the ADSR

    if (millis() - envPressedDebounce > debounce) {          // by pressing the button under the envelope pot
      envPressedDebounce = millis();                         // debounce variable wheee
      oldEnvPressed = envPressed;                            // checking to be sure we're not just saying "pressed NOT PRESSED pressed" etc super fast
      if (envPressed && envPotPickedUp && record == true) {  // what to do! can only do this when envPot is being effective
        envelopeMode++;                                      // which is to increment this variable. 0 = attack, 1 = decay, 2 = sustain, 3 = release
        if (envelopeMode > 3) envelopeMode = 0;              // rolls back to zero
        envPotPickedUp = false;                              // assuming the values changed
      }                                                      // this code is working great!
    }                                                        // MAYBEEEE -- the gesture record part may interfere?
  }

  /* envelope pot picker-upper, catcher, complicated?*/
  if ((envPotPickedUp == false) && (record == true)) {    // envelope pot in wrong position
    byte tempEnvPot = map(arPD7, 0, 4095, 0, 255);        // work with low-res LED values
    if (envelopeMode == 0) Ediff = attack - tempEnvPot;   // it's adjusting ATTACK
    if (envelopeMode == 1) Ediff = decay - tempEnvPot;    // adjusting decay
    if (envelopeMode == 2) Ediff = sustain - tempEnvPot;  // sustain
    if (envelopeMode == 3) Ediff = release - tempEnvPot;  // release
    if (Ediff > 4) {
      targetHue[3] = 160;                           // blue or bluish
      targetSat[3] = 255;                           // saturated as heck
      int tDiff = map(Ediff, 4, 255, 0, 250);       // hope this makes smooth color fading until POOF it's right
      targetVal[3] = tDiff;                         // dimmer as pickup point gets closer
    } else if (Ediff < -4) {                        // the difference might be positive
      targetHue[3] = 0;                             // red
      targetSat[3] = 255;                           // saturated as heck
      int tDiff = map(abs(Ediff), 4, 255, 0, 250);  // heck yes, finally nalied this part of the math
      targetVal[3] = tDiff;                         // dimmer as pickup point gets closer
    } else {                                        // so when you nail the pot position, it flashes white like this:
      targetHue[3] = 255;                           //it's... WHH
      targetSat[3] = 0;                             //           I
      targetVal[3] = 255;                           //            TE (waggles eyebrows like The Colonel)
      pot3Flash = millis();                         // tryinna get the pot to flash when it's picked up
      envPotPickedUp = true;                        // YESSSS, we did it.
    }
  } else if (record == true && shift == false) {           // used to be } else if (record == true) { but maybe removed envelope playback while record == false
    byte envValue = arPD7 >> 4;                            // gets pot readings into ADSR values. >> means "move binary over 4 places" which divides 4095 by sixteen
    switch (envelopeMode) {                                // handles putting variables in to the ADSR envelope
      case 0:                                              // attack, potentiometer shades of white
        targetHue[3] = 80;                                 // green means go
        targetSat[3] = 255;                                // saturated
        targetVal[3] = constrain((envValue + 6), 0, 255);  // brighness of LED, plus a tiny bit so even if it's zero, we know what parameter it's at
        attack = envValue;                                 // but now the value is entered
        break;
      case 1:                                              // decay, pot is shades of pinkish
        targetHue[3] = 200;                                // okay for real pinkish this time
        targetSat[3] = 255;                                // fully saturated
        targetVal[3] = constrain((envValue + 6), 0, 255);  // brighness of LED
        decay = envValue;                                  // decay parameter
        break;
      case 2:                                              // SUSTAIN, pot is shades of yellow
        targetHue[3] = 24;                                 // yellow
        targetSat[3] = 255;                                // fully saturated
        targetVal[3] = constrain((envValue + 6), 0, 255);  // setting the LED
        sustain = envValue;                                // and parameter
        break;
      case 3:                                              // RELEASE, pot is shades of yellow?
        targetHue[3] = 122;                                // pink
        targetSat[3] = 200;                                // fully saturated
        targetVal[3] = constrain((envValue + 6), 0, 255);  // LED
        release = envValue;                                // release parameter
        break;
      case 4:
        // LED player section
        break;
    }
  }

  if (millis() - pot3Flash < 200) {  // these lines make pot3 (envelope pot) flash when picked up
    targetVal[3] = 255;
    targetSat[3] = 0;
    if (millis() - pot3Flash > 50) {
      targetVal[3] = 0;
      // targetSat[3] = 0;
    }
    if (millis() - pot3Flash > 100) {
      targetVal[3] = 255;
      // targetSat[3] = 0;
    }
    if (millis() - pot3Flash > 150) {
      targetVal[3] = 0;
      // targetSat[3] = 0;
    }
  }

  // here's the envelope monitor. The ONLY way to get here is by holding the envelope button
  if (envPressed && record == true) {  // envelope monitor, flashes the pot LED white to show what the envelope will do
    targetHue[3] = 44;
    targetSat[3] = 200;
    targetVal[3] = 0;
    if (LEDEnvTracker == 0) {
      LEDEnvTracker = 1;
      pot3Flash = millis() + 100000;  // stops the flash from happening
    }
    if (LEDEnvTracker == 1) {                                 // it's time to play the envelope! Value 1 means start now!
      if ((pot3Flash - 99500) <= millis()) {                  // why did I choose this value? half a second after the press, I guess LOL
        LEDEnvelopeTimer = millis();                          // envelopes need timing
        envelopeMode--;                                       // decrements envelopeMode
        if (envelopeMode > 250) envelopeMode = 0;             // ((byte)0 - 1) = 255, so this fixes that haha
        LEDEnvTracker = 2;                                    // move on dot org
      }                                                       // moving on to ATTACK PHASE!
    } else if (LEDEnvTracker == 2) {                          // ATTACK ALL THE THINGS
      unsigned long elapsed = millis() - LEDEnvelopeTimer;    // how long has it been?
      unsigned long duration = map(attack, 0, 255, 1, 1000);  // map "attack" to however long the attack will actually be
      LEDenvelopeValue = map(elapsed, 0, duration, 0, 255);   // map duration to how high
      targetVal[3] = LEDenvelopeValue;                        // write that freaking value!
      if (elapsed > duration) {                               //
        LEDEnvTracker = 3;                                    //
        LEDEnvelopeTimer = millis();                          // aaaaand restart the envelope timer
      }
    } else if (LEDEnvTracker == 3) {                               // time to decay. Decompose. Rot. Bury that envelope under the compost pile
      unsigned long elapsed = millis() - LEDEnvelopeTimer;         // how long?
      unsigned long duration = map(decay, 0, 255, 10, 2000);       // two second decay?
      LEDenvelopeValue = map(elapsed, 0, duration, 255, sustain);  // map to possible voltage outputs
      targetVal[3] = LEDenvelopeValue;                             // and do the writing
      if (elapsed > duration) {
        LEDEnvTracker = 4;  // move on to sustain
        LEDEnvelopeTimer = millis();
      }                                                                // but since this is just LED, not gate-based, let's have a one second note......
    } else if (LEDEnvTracker == 4) {                                   // SUSTAIN! The simplest of all the envelope parameters
      targetVal[3] = sustain;                                          // already 0 to 255, no need to update it
      if (millis() - LEDEnvelopeTimer > 1000) {                        // here we just have "sustain" be a time, vs looking for the gate to drop low
        LEDEnvTracker = 5;                                             // release. Let go. Set them free.
        LEDEnvelopeTimer = millis();                                   // we need the timer again for the release timer
      }                                                                // we did that, now...
    } else if (LEDEnvTracker == 5) {                                   // let's do this, RELEASE
      unsigned long elapsed = millis() - LEDEnvelopeTimer;             // time based envelope parameter
      unsigned long duration = map(release, 0, 255, 10, 5000);         // five second release? might need more?
      byte(LEDenvelopeValue) = map(elapsed, 0, duration, sustain, 0);  // how long for the release? this might be so wrong
      targetVal[3] = LEDenvelopeValue;
      if (elapsed > duration) targetVal[3] = 0;
      // ugh, we shall see
      if (elapsed > duration + 1000) LEDEnvTracker = 0;  // back to "nothing is playing" after one second of delay
    }
  } else LEDEnvTracker = 0;  // resets the LED envelope thing for future envelope shows


  static byte attackStartValue;                  // for when the voltage isn't at zero when the envelope starts
  static byte releaseStartValue;                 // for when the voltage isn't at "sustain" value when release begins
  static bool lastGS = false;                    // stands for last Gate State. It's the variable that tracks what the last run-through value of the gate was
  if (record == true) GS = PORTD.IN & (1 << 3);  // this looks at PIN_PD3 to see if the voltage is HIGH
  static bool lastIGS = false;                   // tracker for internally generated gates
  static bool IGS = false;                       // for internally generated gates


  /* OKAY THIS WILL BE FIXED ONCE I have an array of gate times to plug in to the IGS to tell the "sustain" step when to go to the "release" step*/
  /* this part handles the envelope generator AND the LED envelope generator*/
  /* the first part detects the internal or external gate signal*/
  if ((!lastGS && GS) || (!lastIGS && IGS)) {  // this returns TRUE on the rising edge of the gate, the first time lewp() runs after the gate is HIGH
    if (playEnvTracker == 0) {                 // if starting from no envelope playing, do this
      playEnvTracker = 1;                      // but no matter what, start playing the envelope, but if an envelope is already going, start from whatever value it's currently at
      attackStartValue = 0;                    // and start playing from zero in this case
    } else {                                   // ooh, but what if the envelope is doing release?
      attackStartValue = envelopeValue;        // start the attack slope from whatever value is current
      playEnvTracker = 1;
    }
  }

  if ((lastGS && !GS) || (lastIGS && !IGS)) {  // gate just fell. Triggers once-per-gate-fall, and is literally the ONLY WAY to start release phase of envelope
    if (playEnvTracker != 0) {                 // only go to release envelope phase if the envelope is currently playing
      playEnvTracker = 5;                      // RELEASE phase
      envelopeTimer = millis();                // gotta get this timer started
      releaseStartValue = envelopeValue;       // okay release start value, what voltage is the envelope at?
      envelopeValue = releaseStartValue;       // whyyyyyyyyy just to get rid of a "unused variable" compiler complaint
    } else playEnvTracker = 0;                 // probably redundant?
  }

  lastGS = GS;    // just recording the gateSTate for next time we run through lewp()
  lastIGS = IGS;  // recording the last internal gate state

  if (playEnvTracker == 1) {                                           // it's time to play the envelope! Value 1 means start now!
    envelopeTimer = millis();                                          // envelopes need timing
    playEnvTracker = 2;                                                // moving on to ATTACK PHASE!
  } else if (playEnvTracker == 2) {                                    // ATTACK ALL THE THINGS
    unsigned long elapsed = millis() - envelopeTimer;                  // how long has it been?
    unsigned long duration = map(attack, 0, 255, 1, 1000);             // map "attack" to however long the attack will actually be
    envelopeValue = map(elapsed, 0, duration, attackStartValue, 255);  // map duration to how high
    if (elapsed > duration) {
      playEnvTracker = 3;
      envelopeTimer = millis();
    }                                                         // aaaaand restart the envelope timer
  } else if (playEnvTracker == 3) {                           // time to decay. Decompose. Rot. Bury that envelope under the compost pile
    unsigned long elapsed = millis() - envelopeTimer;         // how long?
    unsigned long duration = map(decay, 0, 255, 10, 2000);    // two second decay?
    envelopeValue = map(elapsed, 0, duration, 255, sustain);  // map to possible voltage outputs
    if (elapsed > duration) {
      playEnvTracker = 4;  // move on to sustain
      envelopeTimer = millis();
    }
  } else if (playEnvTracker == 4) {                           // SUSTAIN! The simplest of all the envelope parameters
    if (!(PORTD.IN & (1 << 3))) playEnvTracker = 5;           // move on if the pin is LOW
  } else if (playEnvTracker == 5) {                           // let's do this RELEASE nonsense!
    unsigned long elapsed = millis() - envelopeTimer;         // time based envelope parameter
    unsigned long duration = map(release, 0, 255, 10, 5000);  // five second release? might need more?

    envelopeValue = map(elapsed, 0, duration, sustain, 0);  // how long for the release? this might be so wrong

    if (elapsed > duration) playEnvTracker = 0;  // back to "nothing is playing" not super necessary.
  }
  if (playEnvTracker == 4) envelopeValue = sustain;
  if (playEnvTracker == 0) envelopeValue = 0;

  bool clockPressed = !(PORTB.IN & (1 << 1));  // will happen when clock-pot button is pressed.
  if (clockPressed) {                          // whoah, clock pot button is being pressed
    if (clockPotPickedUp == true) {            // runs the first time through!
      lastClockPotValue = arPE7;               // stores old value! This will be important when you let go of the button
      clockPotPickedUp = false;                // don't record the value on bounces
      ignoreClockPot = false;                  // every time clock pot gets pressed, don't ignore it!!! Unless taptempo is run
      tapCount = 0;                            // reset tapCount every time clockPotButton is pressed
    }                                          // the rest of this stuff happens the whole time the clock button is pressed

    // okay HERE IS WHERE TAP TEMPO HAS TO LIVE!!!!!
    if (clockDivider == 4) tapTempo();
    // that's where it lives. ^^^ Up there ^^^

    if (ignoreClockPot == false) {               // well I guess tapTempo() didn't ACTUALLY run, so do the thing?
      newCCMP = map(arPE7, 0, 4096, 2464, 200);  // 2464 = about 40 BPM?
      TCB0.CCMP = newCCMP;                       // set that CCMP what is that, clock/compare match point? Something like that
      parOneCV = map(arPE7, 0, 4096, 0, 185);    // clock-style hue follows pot only when it's a clock control
      targetHue[0] = parOneCV;                   // hue follows pot while button held
      targetSat[0] = 255;                        // saturated
      targetVal[0] = 100;                        // kinda middlebright
    } else targetSat[0] = 0;                     // if we're doing taptempo, make the pot WHITE
    if (TCB0.CNT > newCCMP) TCB0.CNT = 0;
  } else if (clockPotPickedUp == false) {            // only do this if the clockPot isn't PickedUp
    int diff = lastClockPotValue - arPE7;            // what's the difference?
    if (diff > 10) {                                 // might be negative?
      targetHue[0] = 160;                            // blue or bluish
      targetSat[0] = 255;                            // saturated as heck
      int tDiff = map(diff, 10, 4095, 0, 250);       // hope this makes smooth color fading until POOF it's right
      targetVal[0] = tDiff;                          // dimmer as pickup point gets closer
    } else if (diff < -10) {                         // the difference might be positive
      targetHue[0] = 0;                              // red
      targetSat[0] = 255;                            // saturated as heck
      int tDiff = map(abs(diff), 10, 4095, 0, 250);  // heck yes, finally nalied this part of the math
      targetVal[0] = tDiff;                          // dimmer as pickup point gets closer
    } else {                                         // so when you nail the pot position, it flashes white like this:
      targetHue[0] = 255;                            //it's... WHH
      targetSat[0] = 0;                              //           I
      targetVal[0] = 255;                            //            TE (waggles eyebrows like The Colonel)
      pot1Flash = millis();                          // tryinna get the pot to flash when it's picked up
      clockPotPickedUp = true;                       // YESSSS, we did it.
    }
  }

  for (byte i = 0; i < LEDCOUNT; i++) {  // calculate all the LED parameter values
    if (currentStep > 7) cylonFix = currentStep - 8;
    else cylonFix = currentStep;
    if (i == 0) {                                                  // top LEFT LED handler!!! CLOCK LED
      if (clockPotPickedUp == true) {                              // HEY! Is the pot positioned where it needs to be? Yes??? DO NORMAL STUFF
        if (millis() - pot1Flash < 200) {                          // these lines
          if (millis() - pot1Flash > 50) targetVal[0] = 0;         // are what make the pots
          if (millis() - pot1Flash > 100) targetVal[0] = 255;      // flash when they're
          if (millis() - pot1Flash > 150) targetVal[0] = 0;        // in the right spot.
        } else {                                                   // they go WHITE-BLACK-WHITE-BLACK and back to the actual color, in 1/5th of a second
          targetSat[0] = 255;                                      // fully saturated
          targetVal[0] = 150;                                      // medium bright
          if (abs(arPE7 - oldClockPotValue) > hysteresisWindow) {  // let's ONLY change values if we really meant to
            byte newDivider = map(arPE7, 0, 4095, 0, 9);           // new divider, this is the ppqn divider is internal clock mode
            if (newDivider != oldClockDivider) {                   // whoah, we moved the pot far enough
              oldClockDivider = newDivider;                        // to change the divider for real
              clockDivider = newDivider;                           // here's where that's done
              oldClockPotValue = arPE7;                            // and here's where to store the pot value
            }                                                      // definitely gonna do this for the other four top pots
          }






          if (extClock == true) {           // THIS IS NEW -- if the external clock has fired AT ALL (and hasn't been reset) the clock pot behavior changes,adn does this
            if (clockDivider < 3) {         // is clockDivider 0, 1, or 2?
              extClockMode = 0;             // this means "watch for 24 external clocks. increment recordSteps for each one, fire modeHandling() every 24"
              targetHue[0] = 95;            // sorta aqua
            } else if (clockDivider > 5) {  // is clockDivider 6, 7, or 8? (or 9, but that's edge-case)
              extClockMode = 2;             // this means "divide extClock intervals (periods haha) into 24, increment recordSTeps on each extClock, fire modehandling() every 24"
              targetHue[0] = 220;           // pink
            } else {                        // huh, clockDivider must be 3, 4, or 5. Got it.
              extClockMode = 1;             // this is 16ths, "divide extClock into 12, run modeHandling() every 2 extClock, increment recordSTeps every 12th???""
              targetSat[0] = 0;             // white
              targetVal[0] = 255;           // bright
            }










          } else switch (clockDivider) {  // each of the 8 values gets its own special color
              case 0:                     // two whole notes, cool green
                ppqnPerStep = 192;        // slowwww
                targetHue[0] = 95;        // sorta aqua
                break;                    // break! Stop doing this
              case 1:                     // whole notes - pure red
                ppqnPerStep = 96;         //
                targetHue[0] = 0;         // this is red
                break;                    //
              case 2:                     // half notes - light blue
                ppqnPerStep = 48;         // 48, half-notes
                targetHue[0] = 156;       // light blue
                break;                    //
              case 3:                     // triplet quarters - yellow
                ppqnPerStep = 32;         // triplet quarters. Also called dotted quarter notes?
                targetHue[0] = 64;        // yellow
                break;                    // we were on a break, said Ross (worst character ever)
              case 4:                     // quarter notes - white, the "home" division
                ppqnPerStep = 24;         // ONE QUARTER PORTION
                targetHue[0] = 255;       // speaking of David Schwimmer: white
                targetSat[0] = 0;         // white as can be
                targetVal[0] = 255;       // not an ounce of color in that man
                break;                    // I could never understand what Rachel saw in that guy
              case 5:                     // triplet eighths - orange
                ppqnPerStep = 20;         // I mean, at least Ross wasn't orange
                targetHue[0] = 4;         // OMB
                break;                    // break
              case 6:                     // eighth notes - sorta teal
                ppqnPerStep = 9;          // nine
                targetHue[0] = 100;       // teal
                break;                    //
              case 7:                     // triplet sixteenths - yellow
                ppqnPerStep = 8;          // pretty quick
                targetHue[0] = 68;        // pure yellow
                break;                    //
              case 8:                     // sixteenth notes - pink
                ppqnPerStep = 6;          // sixteenth notes!!! Hammer me with some of those 909CHHs 6
                targetHue[0] = 220;       // pink
                break;                    // whew, all done
            }
        }
        if (CFC > 12) targetVal[0] = targetVal[0] >> 2;
      }

    } else if (i == 3) /*envelope pot*/ {  // top RIGHT led, Envelope Mode
                                           // do nothing because it's handled elsewhere
    } else if (i < 4) {                    // the middle two pots on the top row
                                           // value
    } else if (i == 12) /* shift LED */ {  // do nothing because it's handled elsewhereeee???
      if (shiftMode == 0) {
        targetHue[12] = cirLEDs[currentStep];  // whatever color
        targetSat[12] = 255;                   // bright whatever color
        targetVal[12] = 255;

      } else if (shiftMode == 1) {
        targetHue[12] = 160;  // blue
        targetVal[12] = 255;
        // if (currentStep > 7) cylonFix = currentStep - 8;
        targetVal[cylonFix + 4] = 255;
        targetSat[cylonFix + 4] = 0;  // this fails to work in cylon mode todo
      }
    } else if ((currentStep & 0x07) == i - 4) {  // whoah, this is the active step!!!
      if (millis() - stepFlash < 50) {           // does the LED need to flash?
        doFlash = true;
        targetHue[i] = 255;  // hue
        targetSat[i] = 255;  // saturation
        targetVal[i] = 255;  // value
      } else {               // no, the LED doesn't need to flash, it just needs to be bright
        doFlash = false;
        targetHue[i] = cirLEDs[i - 4];  // hue
        targetSat[i] = 255;             // saturation
        targetVal[i] = 150;             // value
      }                                 // end the active step LED
    } else {                            // no, the LED doesn't need to flash, it just needs to be bright
      targetHue[i] = cirLEDs[i - 4];    // hue                                                        // okay these are the non-active step LEDs
      targetSat[i] = 255;               // saturation
      targetVal[i] = 15;                // value so dim!!!
    }
  }

  while (clockTicks > 0) {  // clockTick handler, happens once per Pulse (per quarter note)
    cli();                  // clear interrupts,
    clockTicks--;           // minus one from clockTicks variable, it'll zoom through until it's zero? Why isn't it just = 0? I DON'T REMEMBER
    sei();                  // set interrupts again

    if (extClock == true) {     // mmkayyy gotta subdivide these thingies
      if (extClockMode == 0) {  // just watches for 24 extClock pusles.
        recordSteps++;          // each extClock pusle is a new recordSteps
        tickTrack++;            // this counts up to 24!
        if (tickTrack > 24) {   // did we do 24 of these extClocks?
          tickTrack = 0;        // yes? then let's reset
          modeHandling();       // and do All The Things
        }
      } else if (extClockMode == 1) {         // this is 16th note mode, 12 recordSteps between clock inputs
        if (extClockFirst == true) {          // and one modeHandlign() per 2 clock inputs
          extNow = micros();                  // here's the first extNow value, to be divided by 12
          extClockFirst = false;              // and don't keep doing this? I think?
        } else {                              // okay do THIS instead, so this isn't the first extClock
          extPeriod = micros() - extNow;      // which is, calculate the time since "extNow" before, right?
          extDividedPeriod = extPeriod / 12;  // make a divided period to run in the main lewp()
          extNow = micros();                  // wait, what? This part runs ONCE PER extClock pulse? Is this right???
          static bool doHandling = false;     // tracks if we're gonna do the thing or NOT do the thing
          if (doHandling == true) {           // shall we do the thing?
            modeHandling();                   // um sure, mode handling dot comzky
          }                                   //
          doHandling = !doHandling;           // tells doHandling to flip flop to the other bool state
        }                                     //
      } else {                                // the only alternative left is extClockMode 2, which is, each extClock do modeHandling BUT increment recordSteps 24 times between
        if (extClockFirst == true) {          // first time through this?
          extNow = micros();                  // drop a temporal reference point
          extClockFirst = false;              // and don't do another one until after measuring the interval
          modeHandling();                     // do the thing every time, including this one
        } else {                              //
          extPeriod = micros() - extNow;      // interval calculation
          extDividedPeriod = extPeriod / 24;  // 24ths. This should be the magic opposite of mode 0, where each external clock gets divided into 24
          extNow = micros();                  // good grief I'm so confused i THINK this will work but??????
          modeHandling();                     // okay this kinda works, besides the recordSteps advance. Hmmmmmm
        }
      }

    } else {

      ppqnCounter++;                     // adds 1 to the value of ppqnCounter
      recordSteps++;                     // adds 1 to the value of recordSteps
      if (ppqnCounter >= ppqnPerStep) {  // has the PPQN threshold been reached?
        ppqnCounter = 0;                 // if so, reset to zero!
        /*this part runs once per thing... the circle pots advance every time this runs*/
        modeHandling();
      } else {
        if (ppqnCounter > 3) targetVal[12] = constrain((255 - (ppqnCounter << 3)), 0, 255);  // shift key LED off
      }
    }
    CFC++;                                                             // CFC equals clock flash counter haha -- this is just for the clock LED to go brighter and dimmer
    if (CFC > 23) CFC = 0;                                             // reset CFC
    if (record == true) {                                              // well, record must equal equal true
      writePWM(rangedCV);                                              // locks this value in
      recorded[recordSteps] = (uint16_t)(gateForRecord << 15)          // gate, on or off, also empties all the binary values
                              | (rangedCV & 0x03FF);                   // the CV, 10 bits, 1024 values
      if ((recorded[recordSteps] >> 15) & 1) PORTF.OUTSET = (1 << 5);  // gate outs! This can be from recorded, or from gestureRecorder
      else PORTF.OUTCLR = (1 << 5);                                    // gate out is zero
      recordedB[recordSteps] = envelopeValue;                          // quick records that envelope value into the recorded loop
      TCA0.SPLIT.HCMP1 = recordedB[recordSteps];                       // and right away plays that envelope back from the HCMP1 output
      // if (loopStart == true) {                                         // this part works during internal clocks,
      //   recordBOC = recordSteps;                                       // but gets missed during external clocking????
      //   recordSteps = 0;                                               // maybe something about loopStart isn't running? HHHHMMMMM
      // }
    } else {                        // the tracks have been FORKED!!!!
      targetHue[3] = (arPD7 >> 3);  // tracks with knob position
      targetSat[3] = 255;           // also, gesture record!!!!
      gestureRecord();              // run that gesture record funtion! It puts the fun into nevermind
      if (extClock == true) {
        if (CFC < 12) targetVal[3] = map(CFC, 0, 12, 0, 255);
        else targetVal[3] = map(CFC, 12, 24, 255, 0);
      } else if (ppqnCounter < (ppqnPerStep >> 1)) {                                    // on the way up, brighter <-- this part just makes the envPot "breathe" red
        targetVal[3] = map(ppqnCounter, 0, (ppqnPerStep >> 1), 0, 255);                 // maps to full bright
      } else targetVal[3] = map(ppqnCounter, (ppqnPerStep >> 1), ppqnPerStep, 255, 0);  // full bright to zero
                                                                                        // ^^^ this part runs just fine
      writePWM(recorded[recordSteps] & 0x03FF);                                         // plays back the recorded CV with the writePWM() function
      if ((recorded[recordSteps] >> 15) & 1) PORTF.OUTSET = (1 << 5);
      else PORTF.OUTCLR = (1 << 5);  // gate out is zero
      TCA0.SPLIT.HCMP1 = recordedB[recordSteps];
      if (recordSteps >= recordBOC) recordSteps = 0;
      else if (recordSteps > 1535) recordSteps = 0;
    }
    TCA0.SPLIT.HCMP2 = envelopeValue;
    // but the on/off still hasn't happened?
    if (shiftMode == 1) {
    }
  }
  // writePWM(currentCV);  // this is to the PWM pin HCMP0


  if (extClock == true) {     // recordStep timer advancer. Needs external clock to be true aannddd
    if (extClockMode != 0) {  // needs extClockMode to be 1 or 2 (2 or 3, haha) because mode 0 (1) is just 24ppqn feeding the sequencer

      if (micros() - extNowReal > extDividedPeriod) {  // ooh, maybe this can work for either extClockMode 1 or 2
        extNowReal = micros();                         // because all it does is watch for the time to exceed extDividedPeriod, and doesn't care about other stuff
        recordSteps++;                                 // and increments this variable for recording stuff whenever
      }
    }
  }



  loopStart = false;

  if (millis() - stepFlash > TRIGGER_TIME) {  // time how long the HIGH signals for trigger outs last
    PORTF.OUTCLR = (0b00001100);              // hey, turn off those HIGH signals for the clock out and end/start of loop signals
  }
}