void modeHandling() {  // runs when it's time to do the next step!



  static byte oldMode;
  if (mode == 0) {                // okay here's linear mode!!!!
    if (metaMode < 8) {           // 7 and below. 7 should just reset to 0 every time. 0 should go 7 steps backwards
      static byte countDown = 8;  // declare countDown, the variable that will be decremented once per step
      if (mode != oldMode) {      // check to see if the mode has changed, let's start from a blank page
        countDown = 8;            // and countdown becomes 8.
      }                           // so the pattern should be 0, 7, 6 repeating. metaMode stays 6 for this exercise
      if (countDown == 8) {
        currentStep = 0;                         // top of pattern
        loopStart = true;                        // trigger the loopStart
      } else {                                   // not top anymore, going DOWN
        currentStep = countDown;                 // first one is pot 7, then 6, then 5, going down until it wraps
      }                                          // okay I hope this works it's been so difficult
      countDown--;                               // decrement countDown by one
      if (countDown <= metaMode) countDown = 8;  // wrap it around GGOODOOODDD LLOOOORRDDDTTTTTT I finally got it working ugrhghhraeljfadjfewoahui
    } else {                                     // POT TURNED UP
      currentStep++;                             // it's going up
      if (currentStep > (metaMode - 8)) {        // do restart stuff
        currentStep = 0;                         // and loops when it's time
        loopStart = true;                        // trigger the loopStart
      }
    }
  } else if (mode == 1) {  // CYLON MODE!!! Up and then down
    // if (mode != oldMode) {            // check to see if the mode has changed, let's start from a blank page
    //   currentStep = currentStep % 8;  // and countdown becomes 8.
    // }
    static bool UP;                          // this was so easy compared to the going down thing
    if (UP) {                                // my poor brain was just stuck. Blocked.
      currentStep++;                         // counting up
      if (currentStep > metaMode) UP = !UP;  // or switching the UP variable
    } else {                                 // ... so simple
      currentStep--;                         // counting down
      if (currentStep <= 0) UP = !UP;        // and switching the UP variable back
      loopStart = true;                      // trigger the loopStart
    }
  } else if (mode == 2) {                           // patterns PATTERNS ***PPPPPAATTEERRNNNNSNSSS**** OMGGGEEEE
    static byte patternCount;                       // variable for counting pattern
    static byte oldMetaMode;                        // keeps track of old meta mode
    if (metaMode != oldMetaMode) patternCount = 0;  // blank slate, clears pattern count in case the previous one and this one are'nt compatible
    oldMetaMode = metaMode;                         // store that variable
    switch (metaMode) {                             // of course, this is pattern bank A, there's three total banks, with 5 patterns each

      case 0:                     // bank A, pattern 1, Stayin' Alive
        if (patternCount == 0) {  // THE TUNE STARTS
          loopStart = true;       // trigger the loopStart
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 1) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 2) currentStep = 1;
        else if (patternCount == 3) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 4) currentStep = 2;
        else if (patternCount == 5) currentStep = 2;
        else if (patternCount == 6) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 7) currentStep = 0;
        else if (patternCount == 8) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 9) currentStep = 1;
        else if (patternCount == 10) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 11) currentStep = 3;
        else if (patternCount == 12) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 14) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 15) currentStep = 0;
        else if (patternCount == 16) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 18) currentStep = 3;
        else if (patternCount == 19) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 20) currentStep = 0;
        else if (patternCount == 21) currentStep = 0;
        else if (patternCount == 22) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 23) currentStep = 4;
        else if (patternCount == 24) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 25) currentStep = 3;
        else if (patternCount == 26) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 27) currentStep = 0;
        else if (patternCount == 28) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 29) currentStep = 1;
        else if (patternCount == 30) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 31) currentStep = 3;
        patternCount++;
        if (patternCount > 31) patternCount = 0;
        break;

      case 1:  // groove is in the heart!!!!!
        if (patternCount == 0) {
          currentStep = 7;   // pattern starts
          loopStart = true;  // trigger the loopStart
        } else if (patternCount == 1) {
          currentStep = 7;
          playEnvTracker = 1;
        } else if (patternCount == 2) {
          currentStep = 7;
        } else if (patternCount == 3) {
          currentStep = 7;
        } else if (patternCount == 4) {
          currentStep = 7;
        } else if (patternCount == 5) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 6) {
          currentStep = 2;
        } else if (patternCount == 7) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 8) {
          currentStep = 1;
        } else if (patternCount == 9) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 10) {
          currentStep = 3;
        } else if (patternCount == 11) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 12) {
          currentStep = 2;
        } else if (patternCount == 13) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 14) {
          currentStep = 1;
        } else if (patternCount == 15) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 16) {
          currentStep = 4;
        } else if (patternCount == 17) {
          currentStep = 4;
        } else if (patternCount == 18) {
          currentStep = 4;
        } else if (patternCount == 19) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 20) {
          currentStep = 5;
        } else if (patternCount == 21) {
          currentStep = 7;
          playEnvTracker = 1;
        } else if (patternCount == 22) {
          currentStep = 7;
        } else if (patternCount == 23) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 24) {
          currentStep = 4;
        } else if (patternCount == 25) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 26) {
          currentStep = 5;
        } else if (patternCount == 27) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 28) {
          currentStep = 6;
        } else if (patternCount == 29) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 30) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 31) {
          currentStep = 7;
          playEnvTracker = 1;
        }
        patternCount++;
        if (patternCount > 31) patternCount = 0;
        break;
      case 2:  // riders on the storm!!!!!!!
        if (patternCount == 0) {
          currentStep = 0;
          playEnvTracker = 1;
          loopStart = true;  // trigger the loopStart
        }

        else if (patternCount == 1) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 2) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 3) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 4) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 5) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 6) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 7) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 8) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 9) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 10) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 11) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 12) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 14) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 15) {
          currentStep = 3;
          playEnvTracker = 1;
        }
        patternCount++;
        if (patternCount > 15) patternCount = 0;
        break;
      case 3:  // Billie Jean

        if (patternCount == 0) {
          currentStep = 3;
          playEnvTracker = 1;
          loopStart = true;  // trigger the loopStart
        } else if (patternCount == 1) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 2) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 3) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 4) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 5) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 6) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 7) {
          currentStep = 1;
          playEnvTracker = 1;
        }
        patternCount++;
        if (patternCount > 7) patternCount = 0;
        break;

      case 4:  // Iron Man

        if (patternCount == 0) {
          currentStep = 0;
          playEnvTracker = 1;
          loopStart = true;  // trigger the loopStart
        } else if (patternCount == 1) {
          currentStep = 0;
        } else if (patternCount == 2) {
          currentStep = 0;
        } else if (patternCount == 3) {
          currentStep = 0;
        } else if (patternCount == 4) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 5) {
          currentStep = 1;
        } else if (patternCount == 6) {
          currentStep = 1;
        } else if (patternCount == 7) {
          currentStep = 1;
        } else if (patternCount == 8) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 9) {
          currentStep = 1;
        } else if (patternCount == 10) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 11) {
          currentStep = 2;
        } else if (patternCount == 12) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 2;
        } else if (patternCount == 14) {
          currentStep = 2;
        } else if (patternCount == 15) {
          currentStep = 2;
        } else if (patternCount == 16) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 18) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 19) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 20) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 21) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 22) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 23) {
          currentStep = 1;
        } else if (patternCount == 24) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 25) {
          currentStep = 1;
        } else if (patternCount == 26) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 27) {
          currentStep = 2;
        } else if (patternCount == 28) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 29) {
          currentStep = 2;
        } else if (patternCount == 30) {
          currentStep = 2;
        } else if (patternCount == 31) {
          currentStep = 2;
        }
        patternCount++;
        if (patternCount > 31) patternCount = 0;
        break;

      case 5:  // something I made up? Rhythm inspired by Part Time Lover (etc)
        if (patternCount == 0) {
          currentStep = 1;
          playEnvTracker = 1;
          loopStart = true;  // trigger the loopStart
        } else if (patternCount == 1) {
          currentStep = 1;
        } else if (patternCount == 2) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 3) {
          currentStep = 1;
        } else if (patternCount == 4) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 5) {
          currentStep = 1;
        } else if (patternCount == 6) {
          currentStep = 1;
        } else if (patternCount == 7) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 8) {
          currentStep = 1;
        } else if (patternCount == 9) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 10) {
          currentStep = 1;
        } else if (patternCount == 11) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 12) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 1;
        } else if (patternCount == 14) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 15) {
          currentStep = 2;
        } else if (patternCount == 16) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 3;
        } else if (patternCount == 18) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 19) {
          currentStep = 3;
        } else if (patternCount == 20) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 21) {
          currentStep = 3;
        } else if (patternCount == 22) {
          currentStep = 3;
        } else if (patternCount == 23) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 24) {
          currentStep = 3;
        } else if (patternCount == 25) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 26) {
          currentStep = 3;
        } else if (patternCount == 27) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 28) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 29) {
          currentStep = 3;
        } else if (patternCount == 30) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 31) {
          currentStep = 5;
        } else if (patternCount == 32) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 33) {
          currentStep = 4;
        } else if (patternCount == 34) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 35) {
          currentStep = 4;
        } else if (patternCount == 36) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 37) {
          currentStep = 4;
        } else if (patternCount == 38) {
          currentStep = 4;
        } else if (patternCount == 39) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 40) {
          currentStep = 4;
        } else if (patternCount == 41) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 42) {
          currentStep = 4;
        } else if (patternCount == 43) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 44) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 45) {
          currentStep = 4;
        } else if (patternCount == 46) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 47) {
          currentStep = 5;
        } else if (patternCount == 48) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 49) {
          currentStep = 6;
        } else if (patternCount == 50) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 51) {
          currentStep = 6;
        } else if (patternCount == 52) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 53) {
          currentStep = 6;
        } else if (patternCount == 54) {
          currentStep = 6;
        } else if (patternCount == 55) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 56) {
          currentStep = 5;
        } else if (patternCount == 57) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 58) {
          currentStep = 5;
        } else if (patternCount == 59) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 60) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 61) {
          currentStep = 5;
        } else if (patternCount == 62) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 63) {
          currentStep = 0;
        }

        patternCount++;
        if (patternCount > 63) patternCount = 0;
        break;
    }
  } else if (mode == 3) {
    static byte patternCount;                       // variable for counting pattern
    static byte oldMetaMode;                        // keeps track of old meta mode
    if (metaMode != oldMetaMode) patternCount = 0;  // blank slate, clears pattern count in case the previous one and this one are'nt compatible
    oldMetaMode = metaMode;                         // store that variable
    switch (metaMode) {
      case 0:  // Lowrider
        if (patternCount == 0) {
          currentStep = 0;
          playEnvTracker = 1;
          loopStart = true;  // trigger the loopStart
        } else if (patternCount == 1) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 2) {
          currentStep = 1;
        } else if (patternCount == 3) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 4) {
          currentStep = 2;
        } else if (patternCount == 5) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 6) {
          currentStep = 3;
        } else if (patternCount == 7) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 8) {
          currentStep = 4;
        } else if (patternCount == 9) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 10) {
          currentStep = 5;
        } else if (patternCount == 11) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 12) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 14) {
          currentStep = 6;
          playEnvTracker = 1;
        } else if (patternCount == 15) {
          currentStep = 7;
          playEnvTracker = 1;
        } else if (patternCount == 16) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 18) {
          currentStep = 1;
        } else if (patternCount == 19) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 20) {
          currentStep = 2;
        } else if (patternCount == 21) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 22) {
          currentStep = 3;
        } else if (patternCount == 23) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 24) {
          currentStep = 4;
        } else if (patternCount == 25) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 26) {
          currentStep = 5;
        } else if (patternCount == 27) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 28) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 29) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 30) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 31) {
          currentStep = 2;
          playEnvTracker = 1;
        }
        patternCount++;
        if (patternCount > 31) patternCount = 0;
        break;
      case 1:  // The Chain by Fleetwood Mac
        if (patternCount == 0) {
          currentStep = 2;
          playEnvTracker = 1;
          loopStart = true;  // flash loop trigger
        } else if (patternCount == 1) {
          currentStep = 2;
        } else if (patternCount == 2) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 3) {
          currentStep = 2;
        } else if (patternCount == 4) {
          currentStep = 2;
        } else if (patternCount == 5) {
          currentStep = 2;
        } else if (patternCount == 6) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 7) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 8) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 9) {
          currentStep = 4;
        } else if (patternCount == 10) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 11) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 12) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 14) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 15) {
          currentStep = 3;
        } else if (patternCount == 16) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 0;
        } else if (patternCount == 18) {
          currentStep = 0;
        } else if (patternCount == 19) {
          currentStep = 0;
        } else if (patternCount == 20) {
          currentStep = 0;
        } else if (patternCount == 21) {
          currentStep = 0;
        } else if (patternCount == 22) {
          currentStep = 0;
        } else if (patternCount == 23) {
          currentStep = 0;
        } else if (patternCount == 24) {
          currentStep = 0;
        } else if (patternCount == 25) {
          currentStep = 0;
        } else if (patternCount == 26) {
          currentStep = 0;
        } else if (patternCount == 27) {
          currentStep = 0;
        } else if (patternCount == 28) {
          currentStep = 0;
        } else if (patternCount == 29) {
          currentStep = 0;
        } else if (patternCount == 30) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 31) {
          currentStep = 1;
        } else if (patternCount == 32) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 33) {
          currentStep = 2;
        } else if (patternCount == 34) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 35) {
          currentStep = 2;
        } else if (patternCount == 36) {
          currentStep = 2;
        } else if (patternCount == 37) {
          currentStep = 2;
        } else if (patternCount == 38) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 39) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 40) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 41) {
          currentStep = 4;
        } else if (patternCount == 42) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 43) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 44) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 45) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 46) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 47) {
          currentStep = 3;
        } else if (patternCount == 48) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 49) {
          currentStep = 0;
        } else if (patternCount == 50) {
          currentStep = 0;
        } else if (patternCount == 51) {
          currentStep = 0;
        } else if (patternCount == 52) {
          currentStep = 0;
        } else if (patternCount == 53) {
          currentStep = 0;
        } else if (patternCount == 54) {
          currentStep = 0;
        } else if (patternCount == 55) {
          currentStep = 0;
        } else if (patternCount == 56) {
          currentStep = 0;
        } else if (patternCount == 57) {
          currentStep = 0;
        } else if (patternCount == 58) {
          currentStep = 0;
        } else if (patternCount == 59) {
          currentStep = 0;
        } else if (patternCount == 60) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 61) {
          currentStep = 0;
        } else if (patternCount == 62) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 63) {
          currentStep = 1;
        }
        patternCount++;
        if (patternCount > 63) patternCount = 0;
        break;
      case 2:  // Feel Good

        if (patternCount == 0) {
          currentStep = 0;
          playEnvTracker = 1;
          loopStart = true;  // flash loop trigger
        } else if (patternCount == 1) {
          currentStep = 0;
        } else if (patternCount == 2) {
          currentStep = 0;
        } else if (patternCount == 3) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 4) {
          currentStep = 1;
          playEnvTracker = 1;
        } else if (patternCount == 5) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 6) {
          currentStep = 2;
        } else if (patternCount == 7) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 8) {
          currentStep = 5;
        } else if (patternCount == 9) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 10) {
          currentStep = 4;
        } else if (patternCount == 11) {
          currentStep = 4;
        } else if (patternCount == 12) {
          currentStep = 4;
        } else if (patternCount == 13) {
          currentStep = 4;
        } else if (patternCount == 14) {
          currentStep = 4;
        } else if (patternCount == 15) {
          currentStep = 4;
        } else if (patternCount == 16) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 3;
        } else if (patternCount == 18) {
          currentStep = 3;
        } else if (patternCount == 19) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 20) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 21) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 22) {
          currentStep = 4;
        } else if (patternCount == 23) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 24) {
          currentStep = 2;
        } else if (patternCount == 25) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 26) {
          currentStep = 0;
        } else if (patternCount == 27) {
          currentStep = 0;
        } else if (patternCount == 28) {
          currentStep = 0;
        } else if (patternCount == 29) {
          currentStep = 0;
        } else if (patternCount == 30) {
          currentStep = 0;
        } else if (patternCount == 31) {
          currentStep = 0;
        }
        patternCount++;
        if (patternCount > 31) patternCount = 0;
        break;
      case 3:  // Juanito's Spooky Alarm Song
        if (patternCount == 0) {
          currentStep = 5;
          playEnvTracker = 1;
          loopStart = true;  // flash loop trigger
        } else if (patternCount == 1) {
          currentStep = 1;
        } else if (patternCount == 2) {
          currentStep = 0;
        } else if (patternCount == 3) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 4) {
          currentStep = 1;
        } else if (patternCount == 5) {
          currentStep = 0;
        } else if (patternCount == 6) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 7) {
          currentStep = 1;
        } else if (patternCount == 8) {
          currentStep = 0;
        } else if (patternCount == 9) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 10) {
          currentStep = 1;
        } else if (patternCount == 11) {
          currentStep = 0;
        } else if (patternCount == 12) {
          currentStep = 5;
          playEnvTracker = 1;
        } else if (patternCount == 13) {
          currentStep = 1;
        } else if (patternCount == 14) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 15) {
          currentStep = 1;
        } else if (patternCount == 16) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 17) {
          currentStep = 1;
        } else if (patternCount == 18) {
          currentStep = 0;
        } else if (patternCount == 19) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 20) {
          currentStep = 1;
        } else if (patternCount == 21) {
          currentStep = 0;
        } else if (patternCount == 22) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 23) {
          currentStep = 1;
        } else if (patternCount == 24) {
          currentStep = 0;
        } else if (patternCount == 25) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 26) {
          currentStep = 1;
        } else if (patternCount == 27) {
          currentStep = 0;
        } else if (patternCount == 28) {
          currentStep = 4;
          playEnvTracker = 1;
        } else if (patternCount == 29) {
          currentStep = 1;
        } else if (patternCount == 30) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 31) {
          currentStep = 1;
        } else if (patternCount == 32) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 33) {
          currentStep = 1;
        } else if (patternCount == 34) {
          currentStep = 0;
        } else if (patternCount == 35) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 36) {
          currentStep = 1;
        } else if (patternCount == 37) {
          currentStep = 0;
        } else if (patternCount == 38) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 39) {
          currentStep = 1;
        } else if (patternCount == 40) {
          currentStep = 0;
        } else if (patternCount == 41) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 42) {
          currentStep = 1;
        } else if (patternCount == 43) {
          currentStep = 0;
        } else if (patternCount == 44) {
          currentStep = 3;
          playEnvTracker = 1;
        } else if (patternCount == 45) {
          currentStep = 1;
        } else if (patternCount == 46) {
          currentStep = 0;
          playEnvTracker = 1;
        } else if (patternCount == 47) {
          currentStep = 1;
        } else if (patternCount == 48) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 49) {
          currentStep = 4;
        } else if (patternCount == 50) {
          currentStep = 7;
        } else if (patternCount == 51) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 52) {
          currentStep = 4;
        } else if (patternCount == 53) {
          currentStep = 7;
        } else if (patternCount == 54) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 55) {
          currentStep = 4;
        } else if (patternCount == 56) {
          currentStep = 7;
        } else if (patternCount == 57) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 58) {
          currentStep = 4;
        } else if (patternCount == 59) {
          currentStep = 7;
        } else if (patternCount == 60) {
          currentStep = 2;
          playEnvTracker = 1;
        } else if (patternCount == 61) {
          currentStep = 4;
        } else if (patternCount == 62) {
          currentStep = 7;
          playEnvTracker = 1;
        } else if (patternCount == 63) {
          currentStep = 4;
        }
        patternCount++;
        if (patternCount > 63) patternCount = 0;
        break;
      case 4:


        patternCount++;
        if (patternCount > 31) patternCount = 0;
        break;
    }
  }






  // this line VVV sets the targetCV for glide, just in case
  targetCV = circlePots[currentStep & 0x07] >> 2;  // divide the circlepot value by 4, from 0-4095 to 0-1023
  glideTimer = millis();

  if (slewValue[currentStep & 0x07] == 0) { // checks to see if we need to glide?
    writeDAC(circlePots[currentStep & 0x07]);  // why the hex code? I can't remember... pretty sure that's bitmasking all but the lowest 4 bits out
  }

  if (circlePots[currentStep] > 0) PORTF.OUTSET = (1 << 4);
  else PORTF.OUTCLR = (1 << 4);

  PORTF.OUTSET = (1 << 2);                 // turns the clock output signal HIGH
  if (loopStart) PORTF.OUTSET = (1 << 3);  // turns the END/START OF LOOP signal HIGH
  stepFlash = millis();                    // Time flashes and dimming of the LEDs in the circle
}