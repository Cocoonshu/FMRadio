#include "UI.h"

#define FREQ_TEXT_SIZE      2
#define FREQ_TEXT_X         0
#define FREQ_TEXT_Y         0
#define FREQ_UNIT_TEXT_X    62
#define FREQ_UNIT_TEXT_Y    8
#define FREQ_UNIT_TEXT_SIZE 1

UI::UI(int rst, int ce, int dc, int din, int clk, int bl) {
  mRST = rst;
  mCE  = ce;
  mDC  = dc;
  mDIN = din;
  mCLK = clk;
  mBL  = bl;
  mBackgroundLightTimeout = 0;
  mFrequency = 76.0f;
  mLcd = new Adafruit_PCD8544(mRST, mCE, mDC, mDIN, mCLK);
  memset(mWaveBuffer, 0, LCD_WIDTH);
}

UI::~UI() {
  if (mLcd != NULL) {
    delete mLcd;
    mLcd = NULL;
  }
}

void UI::begin() {
  if (mLcd == NULL) return;

  pinMode(mBL, OUTPUT);
  digitalWrite(mBL, LOW);

  mLcd->begin();
  mLcd->setContrast(40);
  mLcd->display();
  delay(1000);
}

void UI::turnLightOn() {
  digitalWrite(mBL, HIGH);
}

void UI::turnLightOff() {
  digitalWrite(mBL, LOW);
}

bool UI::isLightOn() {
  return digitalRead(mBL) == HIGH;
}

void UI::triggerLightOn() {
  if (mBackgroundLightTimeout == 0) return;
  
  mStartLightOnTime = millis();
  turnLightOn();
}

void UI::setBackgroundLightTimeout(unsigned long timeout) {
  mBackgroundLightTimeout = timeout;
}

void UI::turnFrequency(float deltaFrequency) {
  mFrequency += deltaFrequency;
  mFrequency = mFrequency < 76.0f ? 76.0f : (mFrequency > 108.0f ? 108.0f : mFrequency);
}

void UI::handleUI() {
  if (mLcd == NULL) return;
  unsigned long now = millis();
  if (abs(now - mStartLightOnTime) > mBackgroundLightTimeout) {
    turnLightOff();
  }
}

void UI::showFMPage() {
  if (mLcd == NULL) return;
  unsigned long now = millis();
  if (abs(now - mLastRenderTime) < INTERVAL_15FPS) {
    return;
  } else {
    mLastRenderTime = now;
  }
  mLcd->clearDisplay();

  // Frequency
  char* frequencyText = NULL;
  ensureFrequencyText(mFrequency, &frequencyText);
  mLcd->setCursor(FREQ_TEXT_X, FREQ_TEXT_Y);
  mLcd->setTextSize(FREQ_TEXT_SIZE);
  mLcd->setTextColor(BLACK, WHITE);
  mLcd->print(frequencyText);
  if (frequencyText != NULL) {
    delete [] frequencyText;
    frequencyText = NULL;
  }

  // MHz
  mLcd->setCursor(FREQ_UNIT_TEXT_X, FREQ_UNIT_TEXT_Y);
  mLcd->setTextSize(FREQ_UNIT_TEXT_SIZE);
  mLcd->setTextColor(BLACK, WHITE);
  mLcd->print("MHz");
  mLcd->display();

  // Wave
  byte  axisTop    = 16;
  byte  axisBottom = LCD_HEIGHT;
  float axisRange  = axisBottom - axisTop;
  float axisMiddle = axisTop + (axisBottom - axisTop) / 2;
  float halfRange  = 0;
  generateSinWave(now);
  mLcd->setCursor(0, 0);
  for (int i = 0; i < LCD_WIDTH; i++) {
    halfRange = (mWaveBuffer[i] / 255.0f) * axisRange * 0.5f;
    mLcd->drawLine(
      i, int(axisMiddle + halfRange),
      i, int(axisMiddle - halfRange),
      BLACK);
  }
  mLcd->display();
}

void UI::ensureFrequencyText(float frequency, char** text) {
  byte hunderds = ((int)frequency) / 100;
  byte tens     = ((int)frequency - hunderds * 100) / 10;
  byte units    = (int)frequency % 10;
  byte tenths   = (frequency - (int)frequency) * 10;

  *text = new char[6] {
    hunderds + '0',
    tens + '0',
    units  + '0',
    '.',
    tenths + '0',
    '\0'
  };
}

void UI::generateSinWave(unsigned long time) {
  byte* src  = mWaveBuffer + 1;
  byte* dest = mWaveBuffer;
  memmove(dest, src, LCD_WIDTH - 1);
  mWaveBuffer[LCD_WIDTH - 1] = (sin(time / 10) * cos(time / 100) + 1.0) * 0.5 * 255;
}

