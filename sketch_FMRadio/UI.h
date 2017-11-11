#ifndef UI_H
#define UI_H

#define LCD_WIDTH  84
#define LCD_HEIGHT 48

#include "Arduino.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define INTERVAL_15FPS 66
#define INTERVAL_30FPS 33
#define INTERVAL_60FPS 16

class UI {
  public:
    UI(int rst, int ce, int dc, int din, int clk, int bl);
    ~UI();

    void begin();
    void showFMPage();
    void turnLightOn();
    void turnLightOff();
    bool isLightOn();
    void triggerLightOn();
    void setBackgroundLightTimeout(unsigned long timeout);
    void turnFrequency(float deltaFrequency);

    void handleUI();
    
  protected:
    void generateSinWave(unsigned long time);
    void ensureFrequencyText(float frequency, char** text);

  private:
    byte mRST, mCE, mDC, mDIN, mCLK, mBL;
    Adafruit_PCD8544* mLcd;
    byte mWaveBuffer[LCD_WIDTH];
    unsigned long mLastRenderTime;
    unsigned long mStartLightOnTime;
    unsigned long mBackgroundLightTimeout;
    float mFrequency;
};

#endif
