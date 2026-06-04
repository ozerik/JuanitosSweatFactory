void writePWM(uint16_t value10bit) {                                    // crams a 10-bit value into an 8-bit PWM pin
  static byte dither = 0;                                               // dither value
  byte high = value10bit >> 2;                                          // main value to send
  if (high < 255) {                                                     // checks for room? 255 + 1 = zerooooo what in the
    dither += value10bit & 0x03;                                        // grab those two least significant bits
    TCA0.SPLIT.HCMP0 = (dither >= 4) ? (dither -= 4, high + 1) : high;  // Is dither greater than 3? if so, get rid of 4/4ths of a whole value out of "dither" and add that 4/4ths to the value sent to the PWM pin
  } else TCA0.SPLIT.HCMP0 = 255;                                        // full value. Jeez, this is pretty clever
}