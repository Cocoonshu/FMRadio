#include "RotaryEncoder.h"

RotaryEncoder* RotaryEncoder::sInstance = NULL;

RotaryEncoder::RotaryEncoder(byte pinA, byte pinB) {
  mPinA              = pinA;
  mPinB              = pinB;
  mLastPulse         = 0;
  mPulse             = 0;
  mLastestPinAB      = 0;
  mLastPinAB         = 0;
  mOnChangedListener = NULL;

  RotaryEncoder::sInstance = this;
}

RotaryEncoder::~RotaryEncoder() {
  RotaryEncoder::sInstance = NULL;
}

void RotaryEncoder::begin() {
  pinMode(mPinA, INPUT_PULLUP);
  pinMode(mPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(mPinA), onPinInterrupted, CHANGE);
  attachInterrupt(digitalPinToInterrupt(mPinB), onPinInterrupted, CHANGE);
  mLastestPinAB = mLastPinAB = 1;
}

void RotaryEncoder::setOnRotaryEncoderChangedListener(void (*listener)(int, int)) {
  mOnChangedListener = listener;
}

void RotaryEncoder::onPinInterrupted() {
  if (sInstance == NULL || sInstance->mOnChangedListener == NULL) return;
  
  uint8_t currentPin = digitalRead(sInstance->mPinA) * 10 + digitalRead(sInstance->mPinB);
  if (currentPin == sInstance->mLastPinAB) {
    return;
  } else {
    // (1<=>1) is rotary encoder start position
    // (1<=>0) is start to rotate clockwise, or (0<=>1) is start to rotate re-clockwise
    // (0<=>0) is middle position
    // (0<=>1) is end to rotate clockwise, or (1<=>0) is start to rotate re-clockwise
    // (1<=>1) is rotary encoder end position
    if (sInstance->mLastPinAB == 00) {
      if (sInstance->mLastestPinAB == 10 && currentPin == 01) sInstance->mPulse++;
      else if (sInstance->mLastestPinAB == 01 && currentPin == 10) sInstance->mPulse--;
    }

    sInstance->mLastestPinAB = sInstance->mLastPinAB;
    sInstance->mLastPinAB = currentPin;
  }

  if (sInstance->mLastPulse != sInstance->mPulse) {
    int direction = sInstance->mPulse - sInstance->mLastPulse;
    direction = direction >= 1 ? 1 : (direction <= -1) ? -1 : 0;
    sInstance->mOnChangedListener(sInstance->mPulse, direction);
    sInstance->mLastPulse = sInstance->mPulse;
  }
}
