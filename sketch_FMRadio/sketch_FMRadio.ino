#include "Button.h"
#include "RotaryEncoder.h"
#include "UI.h"

#define BUTTON_PIN       4
#define ROTARY_ENCODER_A 2
#define ROTARY_ENCODER_B 3
#define NOKIA_5110_RST   15
#define NOKIA_5110_CE    10
#define NOKIA_5110_DC    12
#define NOKIA_5110_DIN   11
#define NOKIA_5110_CLK   13
#define NOKIA_5110_BL    16

void onButtonClicked(int pin);
void onEncoderChanged(int pulse, int deltaPulse);

RotaryEncoder mEncoder(ROTARY_ENCODER_A, ROTARY_ENCODER_B);
Button mButton(BUTTON_PIN);
UI mUI(NOKIA_5110_CLK,
       NOKIA_5110_DIN,
       NOKIA_5110_DC,
       NOKIA_5110_CE,
       NOKIA_5110_RST,
       NOKIA_5110_BL);

void setup() {
  Serial.begin(115200);
  Serial.println("FM Radio ready");

  mButton.setOnClickListener(onButtonClicked);
  mEncoder.setOnRotaryEncoderChangedListener(onEncoderChanged);
  mEncoder.begin();

  mUI.begin();
  mUI.setBackgroundLightTimeout(5000);
  mUI.turnLightOff();
}

void loop() {
  mButton.handleClick();
  mUI.handleUI();
  mUI.showFMPage();
}

void onEncoderChanged(int pulse, int deltaPulse) {
  mUI.triggerLightOn();
  mUI.turnFrequency(deltaPulse * 0.1f);
}

void onButtonClicked(int pin) {
  Serial.print("[onButtonClicked] pin = "); Serial.println(pin);
}
