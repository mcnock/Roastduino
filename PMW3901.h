
#ifndef __PMW3901_H__
#define __PMW3901_H__

#include "Arduino.h"

#include <stdint.h>

class PMW3901 {
public:
  PMW3901(uint8_t cspin);
  boolean Initialize(void);
  bool Initialized;
  void readMotionCount(int16_t *deltaX, int16_t *deltaY);
  void readMotionCountY(int16_t *deltaY);
  void setLed(bool ledOn);
private:
  uint8_t _cs;
  void registerWrite(uint8_t reg, uint8_t value);
  uint8_t registerRead(uint8_t reg);
  void initRegisters(void);
};

#endif //__PMW3901_H__
