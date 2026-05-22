void analogReads() {
  circlePots[0] = (analogRead(PIN_PD4) >> 2);
  circlePots[1] = (analogRead(PIN_PD5) >> 2);
  circlePots[2] = (analogRead(PIN_PE0) >> 2);
  circlePots[3] = (analogRead(PIN_PE1) >> 2);
  circlePots[4] = (analogRead(PIN_PE4) >> 2);
  circlePots[5] = (analogRead(PIN_PE3) >> 2);
  circlePots[6] = (analogRead(PIN_PE2) >> 2);
  circlePots[7] = (analogRead(PIN_PE5) >> 2);
  topRowPots[0] = analogRead(PIN_PD0);  // CV jack top left, clock input mostly
  topRowPots[1] = analogRead(PIN_PD1);
  topRowPots[2] = analogRead(PIN_PD2);
  // topRowPots[3] = analogRead(PIN_PD3);  // CV jack top right REMOVING THIS FROM ANALOG READS because right now it's a gate input
  arPE7 = analogRead(PIN_PE7);  // clock pot, high res
  arPD7 = analogRead(PIN_PD7);  // envelope pot, high res

  parOneCV = map((arPE7 >> 4), 0, 255, 0, 185);
  parFourCV = map((arPD7 >> 4), 0, 255, 0, 185);

  for (byte i = 0; i < 8; i++) { // this loop gets the circlePot LEDs ready
    cirLEDs[i] = map(circlePots[i] >> 2, 0, 255, 0, 185);
  }
  for (byte i = 0; i < 4; i++) {
    topLEDs[i] = map(topRowPots[i] >> 4, 0, 255, 0, 185);
    if (i == 1 || i == 2) topLEDs[i] = topLEDs[i] << 1;
  }

  static bool firstRun = true;                          // all the envelope parameters should be what the pot
  if (firstRun) {                                       // is set to.
    attack = decay = sustain = release = (arPD7 >> 4);  // this line sets the envelope parameters to whatever the envelope pot is at startup
    firstRun = false;                                   // but just do it once, not every time through
  }
}