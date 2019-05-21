#include "RotaryEncoder.h"
#include "Arduino.h"

RotaryEncoder::RotaryEncoder(int pinA, int pinB) {
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(0,  RotaryEncoder::interruptA, RISING);
  attachInterrupt(1,  RotaryEncoder::interruptB, RISING);
  _pinA = pinA;
  _pinB = pinB;
}

int RotaryEncoder::getChange() {
  int change = encoderPos - oldEncPos;
  if (change != 0) {
    oldEncPos = encoderPos;
  }
  return change;
}

void RotaryEncoder::checkPositionA(uint8_t reading) {
 reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos --; //decrement the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
}

void RotaryEncoder::checkPositionB(uint8_t reading) {
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    encoderPos ++; //increment the encoder's position count
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
}

void RotaryEncoder::interruptA() {
  cli();
  RotaryEncoder::instance->checkPositionA(PIND & 0xC);
  sei();
}

void RotaryEncoder::interruptB() {
  cli();
  RotaryEncoder::instance->checkPositionB(PIND & 0xC);
  sei();
}
