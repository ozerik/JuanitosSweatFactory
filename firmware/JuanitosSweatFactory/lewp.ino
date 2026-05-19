void lewp() {
  // analog read subroutine. Run it 200 times per second or whatever
  if (millis() - aTimer > 5) {
    analogReads();
    writeLEDs();  // runs the subroutine that writes to the LEDs
    aTimer = millis();
  }

  // clockTick handler
  if (clockTicks > 0) {
    cli();
    clockTicks--;
    sei();
    clockTicks = false;                // just like the ISR gets reset, this variable goes back to FALSE
    ppqnCounter++;                     // adds 1 to the value of ppqnCounter
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