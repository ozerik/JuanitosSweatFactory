void writeLEDs() {                                           // sets all the LED colors
  for (byte i = 0; i < ledCount; i++) {                      // in this for loop
    setLEDHSV(i, targetHue[i], targetSat[i], targetVal[i]);  // plug the values in!
  }                                                          // and thennnnnn
  ledStrip.write(colors, ledCount, 31);                      // actually send the data to all the APA102 LEDs
}