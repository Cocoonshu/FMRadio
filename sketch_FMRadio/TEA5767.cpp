#include "TEA5767.h"
#include <Wire.h>

TEA5767::TEA5767() {
  mWriteBuffer = new WriteBuffer();
  mWriteBuffer->isMute                  = TRUE;
  mWriteBuffer->isSearchMode            = FALSE;
  mWriteBuffer->prePLL                  = 0;
  mWriteBuffer->postPLL                 = 0;
  mWriteBuffer->searchDirection         = FORWARD;
  mWriteBuffer->stopSearchLevel         = AUTOSTOP_SEARCH_LOW_RF_LEVEL;
  mWriteBuffer->sideBand                = HIGH_SIDE_BAND;
  mWriteBuffer->isMono                  = FALSE;
  mWriteBuffer->isMuteLeft              = FALSE;
  mWriteBuffer->isMuteRight             = FALSE;
  mWriteBuffer->softwarePort1Output     = LOW;
  mWriteBuffer->softwarePort2Output     = LOW;
  mWriteBuffer->isStandby               = TRUE;
  mWriteBuffer->bandLimit               = JAPAN_BAND;
  mWriteBuffer->xtal                    = XTAL_32KHz;
  mWriteBuffer->isSoftwareMute          = TRUE;
  mWriteBuffer->isHighCutControl        = FALSE;
  mWriteBuffer->isStereoNoiseCanceling  = TRUE;
  mWriteBuffer->isEnableSearchIndicator = TRUE;
  mWriteBuffer->isEnablePLLReference    = FALSE;
  mWriteBuffer->DTC                     = DTC_COMMON;
}

TEA5767::~TEA5767() {
  Wire.end();
  if (mReadBuffer != NULL) {
    delete mReadBuffer;
    mReadBuffer = NULL;
  }

  if (mWriteBuffer != NULL) {
    delete mWriteBuffer;
    mWriteBuffer = NULL;
  }
}

void TEA5767::setup() {
  Wire.begin();
  Wire.setClock(I2C_CLOCK);
  readData();
}

void TEA5767::update() {
  writeData();
  readData();
}

void TEA5767::setFrequency(unsigned int frequency) {
  unsigned long pll = convertFrequencyToPLL(frequency);
  if (pll != mWriteBuffer->getPLL()) {
    mWriteBuffer->setPLL(pll);
    mIsWriteBufferDirty = true;
  }
}

unsigned int TEA5767::getFrequency() {
  return (mReadBuffer != NULL) ? convertFrequencyToPLL(mReadBuffer->getPLL()) : 0;
}

void TEA5767::setStereo(bool isStereo) {
  if (mWriteBuffer->isMono != !isStereo) {
    mWriteBuffer->isMono = !isStereo;
    mIsWriteBufferDirty = true;
  }
}

bool TEA5767::isStereo() {
  return (mReadBuffer != NULL) ? mReadBuffer->isStereo : !mWriteBuffer->isMono;
}

void TEA5767::setHighCutControl(bool enabled) {
  if (mWriteBuffer->isHighCutControl != enabled) {
    mWriteBuffer->isHighCutControl = enabled;
    mIsWriteBufferDirty = true;
  }
}

void TEA5767::setStereoNoiseControl(bool enabled) {
  if (mWriteBuffer->isStereoNoiseCanceling != enabled) {
    mWriteBuffer->isStereoNoiseCanceling = enabled;
    mIsWriteBufferDirty = true;
  }
}

void TEA5767::setSoftwareMute(bool enabled) {
  if (mWriteBuffer->isSoftwareMute != enabled) {
    mWriteBuffer->isSoftwareMute = enabled;
    mIsWriteBufferDirty = true;
  }
}

void TEA5767::search(int direction, int stopLevel) {
  if (mWriteBuffer->searchDirection != direction || mWriteBuffer->stopSearchLevel != stopLevel) {
    mWriteBuffer->searchDirection = direction;
    mWriteBuffer->stopSearchLevel = stopLevel;
    mIsWriteBufferDirty = true;
  }
}

void TEA5767::setBand(int band) {
  if (mWriteBuffer->bandLimit != band) {
    mWriteBuffer->bandLimit = band;
    mIsWriteBufferDirty = true;
  }
}

void TEA5767::setPowerOn(bool isPowerOn) {
  if (mWriteBuffer->isStandby != !isPowerOn) {
    mWriteBuffer->isStandby = !isPowerOn;
    mIsWriteBufferDirty = true;
  }
}

bool TEA5767::isPowerOn() {
  return mWriteBuffer->isStandby == FALSE;
}

int TEA5767::getRFLevel() {
  return mReadBuffer->RFADClevel;
}

int TEA5767::getChipID() {
  return mReadBuffer->chipID;
}

bool TEA5767::readData() {
  if (mReadBuffer == NULL) {
    mReadBuffer = new ReadBuffer();
  }

  Wire.requestFrom(ADDRESS, BUFFER_SIZE);
  byte data[BUFFER_SIZE];
  byte counter = 0;
  memset(data, 0, BUFFER_SIZE);
  while (Wire.available()) {
    data[counter] = Wire.read();
    counter++;
  }
  if (counter = BUFFER_SIZE) {
    memcpy(mReadBuffer, data, BUFFER_SIZE);
  }
}

bool TEA5767::writeData() {
  if (!mIsWriteBufferDirty) return;
  byte* data = (byte*) mWriteBuffer;
  Wire.beginTransmission(ADDRESS);
  Wire.write(data, BUFFER_SIZE);
  Wire.endTransmission();
  mIsWriteBufferDirty = false;
}
