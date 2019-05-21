#include <stdint.h>

#ifndef RotaryEncoder_h
#define RotaryEncoder_h
class RotaryEncoder {
  public:
    RotaryEncoder(int pinA, int pinB);
    static RotaryEncoder* instance;
    void checkPositionA(uint8_t reading);
    void checkPositionB(uint8_t reading);
    int getChange();
    static void interruptA();
    static void interruptB();
    int encoderPos = 0;
  private:
    int _pinA;
    int _pinB;
    uint8_t aFlag = 0;
    uint8_t bFlag = 0;
    int oldEncPos = 0;
};
#endif
