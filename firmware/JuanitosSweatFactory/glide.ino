void glide() {
  if (currentCV == targetCV) return;        // no gliding needs doing YES
  unsigned long duration;                   // how long shall we glide?
  switch (slewValue[currentStep & 0x07]) {  // current step (hex means bitmask to throw away higher bits to avoid writing to unknown memory)
    case 0: duration = 0; break;            // none - instant
    case 1: duration = shortGlide; break;   // short
    case 2: duration = longGlide; break;    // long
  }

  if (duration == 0) {                                             // if there's zero time
    currentCV = targetCV;                                          // instant jump, obviously
  } else {                                                         // okay, let's do math for interpolating the CV
    unsigned long elapsed = millis() - glideTimer;                 // create this variable for when we're gonna finish gliding
    if (elapsed >= duration) {                                     // and watch to see if we're finished?
      currentCV = targetCV;                                        // all  finished
    } else {                                                       // except maybe we're not?
      currentCV = map(elapsed, 0, duration, currentCV, targetCV);  // do this math, adding or subtracting to get to the next CV value
    }
  }
}