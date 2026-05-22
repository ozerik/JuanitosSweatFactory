void lewp() {


  // analog read subroutine. Run it 200 times per second or whatever
  if (millis() - aTimer > 5) {  // run it 200 times per second
    analogReads();              // analog reads all 12 analog pin inputs
    writeLEDs();                // runs the "write LEDs" subroutine
    aTimer = millis();          // reset the timer
  }                             // this code works great!




  writeDAC(circlePots[currentStep]);



  envPressed = !(PORTB.IN & (1 << 2));               // is true when button pressed
  if (envPressed != oldEnvPressed) {                 // here's how we choose the parameter of the ADSR
    if (millis() - envPressedDebounce > debounce) {  // by pressing the button under the envelope pot
      envPressedDebounce = millis();                 // debounce variable wheee
      oldEnvPressed = envPressed;                    // checking to be sure we're not just saying "pressed NOT PRESSED pressed" etc super fast
      if (envPressed && envPotPickedUp) {            // what to do! can only do this when envPot is being effective
        envelopeMode++;                              // which is to increment this variable. 0 = attack, 1 = decay, 2 = sustain, 3 = release
        if (envelopeMode > 4) envelopeMode = 0;      // rolls back to zero
        envPotPickedUp = false;                      // assuming the values changed
      }                                              // this code is working great!
    }
  }

  /* envelope pot picker-upper, catcher, complicated?*/

  if (envPotPickedUp == false) {                          // envelope pot in wrong position
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
  } else {


    byte envValue = arPD7 >> 4;            // gets pot readings into ADSR values. >> means "move binary over 4 places" which divides 4095 by sixteen
    switch (envelopeMode) {                // handles putting variables in to the ADSR envelope
      case 0:                              // attack, potentiometer shades of white
        targetHue[3] = 80;                 // green means go
        targetSat[3] = 255;                // saturated
        targetVal[3] = attack = envValue;  // brighness of LED, and attack parameter both equal whatever value the pot is set to
        break;
      case 1:                             // decay, pot is shades of pinkish
        targetHue[3] = 200;               // okay for real pinkish this time
        targetSat[3] = 255;               // fully saturated
        targetVal[3] = decay = envValue;  // brighness of LED, and decay parameter
        break;
      case 2:                               // SUSTAIN, pot is shades of green
        targetHue[3] = 24;                  // green
        targetSat[3] = 255;                 // fully saturated
        targetVal[3] = sustain = envValue;  // setting the LED, and the sustain parameter
        break;
      case 3:                               // RELEASE, pot is shades of yellow?
        targetHue[3] = 244;                 // blue
        targetSat[3] = 255;                 // fully saturated
        targetVal[3] = release = envValue;  // equals equals
        break;
      case 4:
        // LED player section
        break;
    }
  }







  if (millis() - pot3Flash < 200) {  // these lines
    targetVal[3] = 255;
    targetSat[3] = 0;
    if (millis() - pot3Flash > 50) {
      targetVal[3] = 0;
      targetSat[3] = 0;
    }
    if (millis() - pot3Flash > 100) {
      targetVal[3] = 255;
      targetSat[3] = 0;
    }
    if (millis() - pot3Flash > 150) {
      targetVal[3] = 0;
      targetSat[3] = 0;
    }
  }


  if (envelopeMode == 4) {  // this is the envelope monitor
    targetHue[3] = 44;
    targetSat[3] = 200;
    if (LEDEnvTracker == 0) {
      LEDEnvTracker = 1;
      pot3Flash = millis() + 200;
    }
    if (LEDEnvTracker == 1) {                                 // it's time to play the envelope! Value 1 means start now!
      LEDEnvelopeTimer = millis();                            // envelopes need timing
      LEDEnvTracker = 2;                                      // moving on to ATTACK PHASE!
    } else if (LEDEnvTracker == 2) {                          // ATTACK ALL THE THINGS
      unsigned long elapsed = millis() - LEDEnvelopeTimer;    // how long has it been?
      unsigned long duration = map(attack, 0, 255, 1, 2000);  // map "attack" to however long the attack will actually be
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
  }







  static byte attackStartValue;   // for when the voltage isn't at zero when the envelope starts
  static byte releaseStartValue;  // for when the voltage isn't at "sustain" value when release begins
  static bool lastGS = false;     // stands for last Gate State. It's the variable that tracks what the last run-through value of the gate was
  bool GS = PORTD.IN & (1 << 3);  // this looks at PIN_PD3 to see if the voltage is HIGH
  static bool lastIGS = false;    // tracker for internally generated gates
  static bool IGS = false;        // for internally generated gates

  /* this part handles the envelope generator AND the LED envelope generator*/
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
                                               // if (!go) {                              // this is true (meaning go value == false) for one second after the two seconds of go being true
    if (playEnvTracker != 0) {                 // only go to release envelope phase if the envelope is currently playing
      playEnvTracker = 5;                      // RELEASE phase
      envelopeTimer = millis();                // gotta get this timer started
      releaseStartValue = envelopeValue;       // okay release start value, what voltage is the envelope at?
    } else playEnvTracker = 0;                 // probably redundant?
  }

  lastGS = GS;    // just recording the gateSTate for next time we run through lewp()
  lastIGS = IGS;  // recording the last internal gate state

  if (playEnvTracker == 1) {                                           // it's time to play the envelope! Value 1 means start now!
    envelopeTimer = millis();                                          // envelopes need timing
    playEnvTracker = 2;                                                // moving on to ATTACK PHASE!
  } else if (playEnvTracker == 2) {                                    // ATTACK ALL THE THINGS
    unsigned long elapsed = millis() - envelopeTimer;                  // how long has it been?
    unsigned long duration = map(attack, 0, 255, 1, 2000);             // map "attack" to however long the attack will actually be
    envelopeValue = map(elapsed, 0, duration, attackStartValue, 255);  // map duration to how high
    analogWrite(PIN_PA5, envelopeValue);                               // write that freaking value!
    if (elapsed > duration) {
      playEnvTracker = 3;
      envelopeTimer = millis();
    }                                                         // aaaaand restart the envelope timer
  } else if (playEnvTracker == 3) {                           // time to decay. Decompose. Rot. Bury that envelope under the compost pile
    unsigned long elapsed = millis() - envelopeTimer;         // how long?
    unsigned long duration = map(decay, 0, 255, 10, 2000);    // two second decay?
    envelopeValue = map(elapsed, 0, duration, 255, sustain);  // map to possible voltage outputs
    analogWrite(PIN_PA5, envelopeValue);                      // and do the writing
    if (elapsed > duration) playEnvTracker = 4;               // move on to sustain
  } else if (playEnvTracker == 4) {                           // SUSTAIN! The simplest of all the envelope parameters
    analogWrite(PIN_PA5, sustain);                            // already 0 to 255, no need to update it. Just play this sustain value forever
  } else if (playEnvTracker == 5) {                           // let's do this RELEASE nonsense!
    unsigned long elapsed = millis() - envelopeTimer;         // time based envelope parameter
    unsigned long duration = map(release, 0, 255, 10, 5000);  // five second release? might need more?



    envelopeValue = map(elapsed, 0, duration, sustain, 0);  // how long for the release? this might be so wrong





    analogWrite(PIN_PA5, envelopeValue);         // ugh, we shall see
    if (elapsed > duration) playEnvTracker = 0;  // back to "nothing is playing" not super necessary.
  }








  bool clockPressed = !(PORTB.IN & (1 << 1));  // will happen when clock-pot button is pressed.
  if (clockPressed) {                          // whoah, clock pot button is being pressed
    if (clockPotPickedUp) {                    // runs the first time through!
      lastClockPotValue = arPE7;               // stores old value! This will be important when you let go of the button
      clockPotPickedUp = false;                // don't record the value on bounces
    }
    unsigned int newCCMP = map(arPE7, 0, 4095, 2464, 200);  // 2464 = about 40 BPM?
    TCB0.CCMP = newCCMP;
    if (TCB0.CNT > newCCMP) TCB0.CNT = 0;
    parOneCV = map((arPE7 >> 4), 0, 255, 0, 185);  // clock-style hue follows pot only when it's a clock control
    targetHue[0] = parOneCV;                       // hue follows pot while button held
    targetSat[0] = 255;
    targetVal[0] = 100;
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

  for (byte i = 0; i < ledCount; i++) {                            // calculate all the LED parameter values
    if (i == 0) {                                                  // top LEFT LED handler!!! CLOCK LED
      if (clockPotPickedUp) {                                      // HEY! Is the pot positioned where it needs to be? Yes??? DO NORMAL STUFF
        if (millis() - pot1Flash < 200) {                          // these lines
          if (millis() - pot1Flash > 50) targetVal[0] = 0;         // are what make the pots
          if (millis() - pot1Flash > 100) targetVal[0] = 255;      // flash when they're
          if (millis() - pot1Flash > 150) targetVal[0] = 0;        // in the right spot.
        } else {                                                   // they go WHITE-BLACK-WHITE-BLACK and back to the actual color, in 1/5th of a second
          targetSat[0] = 255;                                      // fully saturated
          targetVal[0] = 150;                                      // medium bright
          if (abs(arPE7 - oldClockPotValue) > hysteresisWindow) {  // let's ONLY change values if we really meant to
            byte newDivider = map(arPE7, 0, 4095, 0, 9);           // new divider,
            if (newDivider != oldClockDivider) {                   // whoah, we moved the pot far enough
              oldClockDivider = newDivider;                        // to change the divider for real
              clockDivider = newDivider;                           // here's where that's done
              oldClockPotValue = arPE7;                            // and here's where to store the pot value
            }                                                      // definitely gonna do this for the other four top pots
          }

          switch (clockDivider) {  // each of the 8 values gets its own special color
            case 0:                // two whole notes, cool green
              ppqnPerStep = 192;   // slowwww
              targetHue[0] = 95;   // bright red
              break;               // break! Stop doing this
            case 1:                // whole notes - pure red
              ppqnPerStep = 96;    //
              targetHue[0] = 0;    // this is red
              ;                    // deep red
              break;               //
            case 2:                // half notes - light blue
              ppqnPerStep = 48;    // 48, half-notes
              targetHue[0] = 156;  // light blue
              break;               //
            case 3:                // triplet quarters - yellow
              ppqnPerStep = 32;    // triplet quarters. Also called dotted quarter notes?
              targetHue[0] = 64;   // yellow
              break;               // we were on a break, said Ross (worst character ever)
            case 4:                // quarter notes - white, the "home" division
              ppqnPerStep = 24;    // ONE QUARTER PORTION
              targetHue[0] = 255;  // speaking of David Schwimmer: white
              targetSat[0] = 0;    // white as can be
              targetVal[0] = 255;  // not an ounce of color in that man
              break;               // I could never understand what Rachel saw in that guy
            case 5:                // triplet eighths - orange
              ppqnPerStep = 20;    // I mean, at least Ross wasn't orange
              targetHue[0] = 4;    // OMB
              break;               // break
            case 6:                // eighth notes - sorta teal
              ppqnPerStep = 9;     // nine
              targetHue[0] = 100;  // teal
              break;               //
            case 7:                // triplet sixteenths - yellow
              ppqnPerStep = 8;     // pretty quick
              targetHue[0] = 68;   // pure yellow
              break;               //
            case 8:                // sixteenth notes - pink
              ppqnPerStep = 6;     // sixteenth notes!!! Hammer me with some of those 909CHHs
              targetHue[0] = 220;  // pink
              break;               // whew, all done
          }
        }
        if (CFC > 12) targetVal[0] = targetVal[0] >> 2;
      }
    } else if (i == 3) /*envelope pot*/ {  // top RIGHT led, Envelope Mode
                                           // do nothing because it's handled elsewhere
    } else if (i < 4) {                    // the middle two pots on the top row
      targetHue[i] = topLEDs[i];           // hue
      targetSat[i] = 255;                  // saturation
      targetVal[i] = 100;                  // value
    } else if (currentStep == i - 4) {     // whoah, this is the active step!!!
      if (millis() - stepFlash < 50) {     // does the LED need to flash?
        targetHue[i] = 255;                // hue
        targetSat[i] = 255;                // saturation
        targetVal[i] = 255;                // value
      } else {                             // no, the LED doesn't need to flash, it just needs to be bright
        targetHue[i] = cirLEDs[i - 4];     // hue
        targetSat[i] = 255;                // saturation
        targetVal[i] = 150;                // value
      }                                    // end the active step LED
    } else {                               // okay these are the non-active step LEDs
      targetHue[i] = cirLEDs[i - 4];       // hue
      targetSat[i] = 255;                  // saturation
      targetVal[i] = 15;                   // value so dim!!!
    }
  }

  if (clockTicks > 0) {                // clockTick handler
    cli();                             // clear interrupts,
    clockTicks--;                      // minus one from clockTicks variable
    sei();                             // set interrupts again
    ppqnCounter++;                     // adds 1 to the value of ppqnCounter
    CFC++;                             // CFC equals clock flash counter haha
    if (CFC > 23) CFC = 0;             // reset CFC
    if (ppqnCounter >= ppqnPerStep) {  // has the PPQN threshold been reached?
      ppqnCounter = 0;                 // if so, reset to zero!
      currentStep++;                   // add one to currentStep
      if (currentStep >= numSteps) {   // number of steps?
        currentStep = 0;               // reset them!
      }
      stepFlash = millis();  // Time flashes and dimming of the LEDs
    }
  }
}