#include "Button.h"

Button::Button(int pin) : mPin(pin) {
  pinMode(mPin, INPUT_PULLUP);
  mLastPinValue = HIGH;
}

Button::~Button() {
  mOnClickListener = NULL;
}

void Button::setOnClickListener(void (*listener)(int pin)) {
  mOnClickListener = listener;
}

void Button::handleClick() {
  if (mOnClickListener != NULL) {
    byte value = digitalRead(mPin);
    if (value == mLastPinValue) {
      return;
    } else {
      mLastPinValue = value;
    }

    if (value == HIGH) {
      mOnClickListener(mPin);
    }
  }
}
