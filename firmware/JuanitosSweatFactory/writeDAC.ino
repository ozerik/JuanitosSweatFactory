void writeDAC(unsigned int value) {  // this is so silly -- the data needs to be written twice, one as a byte, and once with just two bits
  DAC0.DATAL = value & 0x03;         // write lower 2 bits
  DAC0.DATAH = (value >> 2) & 0xFF;  // write upper 8 bits and latches the output
}  // don't mess with this function, it'll break EVERYTHING