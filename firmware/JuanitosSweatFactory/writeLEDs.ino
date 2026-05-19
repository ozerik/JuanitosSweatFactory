void writeLEDs() {

  for (byte i = 0; i < ledCount; i++) {
    if (i == 0) setLEDHSV(i, parOneCV, 255, 100);
    else if (i == 3) setLEDHSV(i, parFourCV, 255, 100);
    else if (i < 4) setLEDHSV(i, topLEDs[i], 255, 100);
    else if (currentStep == i - 4) {
      if (millis() - stepFlash < 20) setLEDHSV(i, 255, 255, 255);
      // else setLEDHSV(i, cirLEDs[i - 4], 255, 255 - (map(millis() - stepFlash, 20, 500, 0, 235)));
      else setLEDHSV(i, cirLEDs[i - 4], 255, 150);
    } else setLEDHSV(i, cirLEDs[i - 4], 255, 15);
  }

  ledStrip.write(colors, ledCount, 31);
}