int quantize(int rawCounts) {        // this quantizes the value rawCounts to play in whatever scale qMode is at
  if (qMode == 0) return rawCounts;  // Nope. Just play the value
  if (slewValue[currentStep] > 0) return rawCounts;
  int octave = rawCounts / countsPerOctave;                              // how many octaves up are we?
  int remainder = rawCounts - (octave * countsPerOctave);                // what note in that octave are we at?
  return (octave * countsPerOctave) + quantSnapTable[qMode][remainder];  // okay, snap to teh quantized snap table
}


void gestureRecord() {
  // okay.... pressing shift zeroes out gate. Holding shift plus envPotButton sets gate high. Holding
  // envPotButton down *without* holding shift while turning pot sets CV. Pressing envpotbutton records
  // incoming CV is there is any

  // shift is the variable that's always true when shift button is pressed
  // envPressed is the varialbe thats true when envelope pot is pressed
  // arPD7 is the envelope pot variable. Probably wanna read that every pusle not in analogReads()
  // analogRead(PIN_PD3) is the value of what's coming in the jack associated with the env pot


  if (shift == true) {                                                 // shift button is pressed while we're gestureRecording
    if (envPressed == true) recorded[recordSteps] |= (1 << 15);        // set the gate HIGH? right?
    else recorded[recordSteps] &= ~(1 << 15);                          // clear that bit! Sets gate LOW. Pretty sure.
  } else if (envPressed == true) {                                     // shift is NOT pressed and yet, envelope pot button is!
    int tempArPD3 = (analogRead(PIN_PD3) >> 2);                        // reads PD3 (this is the jack  near the envelope pot), turns 12-bit number into 10-bits
    uint16_t newCV;                                                    // let's use t his variable!
    if (tempArPD3 > 24) newCV = tempArPD3 & 0x03FF;                    // take our analog value from the jack, see if there's anything plugged in there
    else newCV = (arPD7 >> 2) & 0x03FF;                                // no? just use the pot position. Oh yeah, make sure we're bitmasking the important pins
    recorded[recordSteps] = (recorded[recordSteps] & 0xFC00) | newCV;  // and write the bitmasked value to recorded[recordSteps]
  }
}

void tapTempo() {

  static unsigned long lastTap = 0;                                             // when did the last tap happen?
  static unsigned long intervals[8];                                            // stores all the times between taps
  static byte tapIndex = 0;                                                     // which value in that array? This one!
  static unsigned long intervalSum = 0;                                         // add up all the times between taps
  static bool lastShiftState = false;                                           // tracker to only do this stuff once per shift key press
  bool shiftState = !(PORTA.IN & (1 << 7));                                     // LOOKS FOR THE SHIFT KEY TO GET PRESSED!!!!!!
  if (shiftState && !lastShiftState) {                                          // ope, it got pressed AND this is the first time
    ignoreClockPot = true;                                                      // tell the rest of the sketch to ignore the clock pot's CCMP value
    ppqnCounter = 0;                                                            // reset PPQN counter! To synchronize stuff
    modeHandling();                                                             // run the main function that ... handles.... the mode?
    unsigned long now = millis();                                               // timer tracker, it's now time right now
    unsigned long interval = now - lastTap;                                     // interval is how long since last tap
    lastTap = now;                                                              // lastTap is what now is now, but it'll be later next time. Hashtag whaaaatttttt
    if (interval < 2000) {                                                      // ignore if you haven't tapped shift since 2 seconds ago
      intervalSum -= intervals[tapIndex];                                       // remove oldest tap value
      intervals[tapIndex] = interval;                                           // store new interval
      intervalSum += interval;                                                  // add new to sum
      tapIndex = (tapIndex + 1) & 0x07;                                         // advance circular buffer (bitmask throws away all but 3 bits for 0 to 7)
      if (tapCount < 8) {                                                       // if shift has been tapped 8 times, we can divide by 8!!! BITSHIFT!!!!
        tapCount++;                                                             // counts taps!!! Up to 8. If this runs, it averages just by the number of taps
        unsigned long avgInterval = intervalSum / tapCount;                     // but oops, nope, we have to do fancy division
        TCB0.CCMP = (uint16_t)(((23437UL * avgInterval) / 6000UL) - 1);         // IDK about this math.... but trial and error shows it works
      } else {                                                                  // else HEY!!! tapCount is 8 so we can >> 3 AKA bitshift three places AKA divide by 8 in one clock tick
        TCB0.CCMP = (uint16_t)(((23437UL * (intervalSum >> 3)) / 6000UL) - 1);  // um? divides by 8 for nearly free
      }
    }
  }
  lastShiftState = shiftState;
}


void writePWM(uint16_t value10bit) {   // crams a 10-bit value into an 8-bit PWM pin
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {  // nope, this just stores the 10-bit value into the volatile variable
    pwmTarget10bit = value10bit;       // that's all just that
  }
  /*here's all the stuff the dithering code USED to do*/
  // static byte dither = 0;                                               // dither value
  // byte high = value10bit >> 2;                                          // main value to send
  // if (high < 255) {                                                     // checks for room? 255 + 1 = zerooooo what in the
  //   dither += value10bit & 0x03;                                        // grab those two least significant bits
  //   TCA0.SPLIT.HCMP0 = (dither >= 4) ? (dither -= 4, high + 1) : high;  // Is dither greater than 3? if so, get rid of 4/4ths of a whole value out of "dither" and add that 4/4ths to the value sent to the PWM pin
  // } else TCA0.SPLIT.HCMP0 = 255;                                        // full value. Jeez, this is pretty clever
}


void writeDAC(unsigned int value) {  // this is so silly -- the data needs to be written twice, one as a byte, and once with just two bits


  DAC0.DATAL = (value & 0x03) << 6;  // write lower 2 bits
  DAC0.DATAH = (value >> 2) & 0xFF;  // write upper 8 bits and latches the output

}  // don't mess with this function, it'll break EVERYTHING




void writeLEDs() {                                           // sets all the LED colors
  for (byte i = 0; i < LEDCOUNT; i++) {                      // in this for loop
    setLEDHSV(i, targetHue[i], targetSat[i], targetVal[i]);  // plug the values in!
  }                                                          // and thennnnnn
  ledStrip.write(colors, LEDCOUNT, brightness);                      // actually send the data to all the APA102 LEDs
}

void setLEDHSV(byte index, byte h, byte s, byte v) {
  if (h >= 128 && h <= 213) v = min((byte)255, (unsigned int)v * 3 / 2);  // boost blue region 50%
  byte r;
  byte g;
  byte b;
  if (s == 0) r = g = b = v;
  else {
    byte region = h / 43;
    byte remainder = (h - (region * 43)) * 6;
    byte p = ((unsigned int)v * (255 - s)) >> 8;
    byte q = ((unsigned int)v * (255 - (((unsigned int)s * remainder) >> 8))) >> 8;
    byte t = ((unsigned int)v * (255 - (((unsigned int)s * (255 - remainder)) >> 8))) >> 8;
    switch (region) {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      default:
        r = v;
        g = p;
        b = q;
        break;
    }
  }
  colors[index].red = r;
  colors[index].green = g;
  colors[index].blue = b;
}