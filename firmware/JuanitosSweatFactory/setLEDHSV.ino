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