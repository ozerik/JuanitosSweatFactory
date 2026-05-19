void analogReads() {

  circlePots[0] = analogRead(PIN_PD4);
  circlePots[1] = analogRead(PIN_PD5);
  circlePots[2] = analogRead(PIN_PE0);
  circlePots[3] = analogRead(PIN_PE1);
  circlePots[4] = analogRead(PIN_PE4);
  circlePots[5] = analogRead(PIN_PE3);
  circlePots[6] = analogRead(PIN_PE2);
  circlePots[7] = analogRead(PIN_PE5);

  topRowPots[0] = analogRead(PIN_PD0);  // this is the CV jack in the top left. It's a clock input mostly.
  topRowPots[1] = analogRead(PIN_PD1);
  topRowPots[2] = analogRead(PIN_PD2);
  topRowPots[3] = analogRead(PIN_PD3);  // this is the CV jack in the top right. IDK what it's gonna be used for
  
  int arPE7 = (analogRead(PIN_PE7));
  unsigned int newCCMP = map(arPE7, 0, 4095, 2464, 243);
  TCB0.CCMP = newCCMP;
  if (TCB0.CNT > newCCMP) TCB0.CNT = 0;
  TCB0.CCMP = map(arPE7, 0, 4095, 2000, 300); // update tempo! Remember CCMP means compare/capture match point
  parOneCV = map((arPE7 >> 4), 0, 255, 0, 185);   // the pot voltages
  parFourCV = map((analogRead(PIN_PD7) >> 4), 0, 255, 0, 185);  // distinct from CV voltages

  for (byte i = 0; i < 8; i++) {
    cirLEDs[i] = map(circlePots[i] >> 4, 0, 255, 0, 185);
  }

  for (byte i = 0; i < 4; i++) {
    topLEDs[i] = map(topRowPots[i] >> 4, 0, 255, 0, 185);
    if (i == 1 || i == 2) topLEDs[i] = topLEDs[i] << 1;
  }
}