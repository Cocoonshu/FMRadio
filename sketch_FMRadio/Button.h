#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class Button {
  public:
    Button(int pin);
    ~Button();

    void setOnClickListener(void (*)(int));
    void handleClick();

  private:
    void onPinInterrupted();

  private:
    void (*Button::mOnClickListener)(int);
    int mPin;
    byte mLastPinValue;
};

#endif
