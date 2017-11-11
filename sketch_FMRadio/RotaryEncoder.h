#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "Arduino.h"

class RotaryEncoder {
  public:
    RotaryEncoder(byte pinA, byte pinB);
    ~RotaryEncoder();

    void begin();
    void setOnRotaryEncoderChangedListener(void (*)(int, int));

  private:
    static void onPinInterrupted();

  private:
    volatile int     mLastPulse;
    volatile int     mPulse;
    volatile uint8_t mLastestPinAB; // last last pin value of A and B
    volatile uint8_t mLastPinAB; // last pin value of A and B
    byte             mPinA;
    byte             mPinB;

    void (*RotaryEncoder::mOnChangedListener)(int, int);

  private:
    static RotaryEncoder* sInstance;
};

#endif
