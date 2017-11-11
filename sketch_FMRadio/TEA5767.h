#ifndef TEA5767_H
#define TEA5767_H

#include "Arduino.h"

#define MIN_FREQ                     76.0
#define SPLIT_FREQ                   87.5
#define MAX_FREQ                     108.0

#define JAPAN_BAND                    1
#define EUROPE_BAND                   0
#define XTAL_32KHz                    0
#define XTAL_13MHz                    1
#define HIGH_SIDE_BAND                1
#define LOW_SIDE_BAND                 0
#define FORWARD                       1
#define BACKWARD                      0
#define NO_AUTOSTOP_SEARCH            0
#define AUTOSTOP_SEARCH_LOW_RF_LEVEL  1
#define AUTOSTOP_SEARCH_MID_RF_LEVEL  2
#define AUTOSTOP_SEARCH_HIGH_RF_LEVEL 3
#define DTC_75US                      1
#define DTC_50US                      0
#define DTC_USA                       DTC_75US
#define DTC_COMMON                    DTC_50US
#define TRUE                          1
#define FALSE                         0

#define I2C_CLOCK        400000 // 400kHz
#define IF               225000 // 225kHz
#define FREF_32KHz       32768  // 32.768kHz
#define FREF_13MHz       50000  // 50kHz
#define ADDRESS          0xC0
#define BUFFER_SIZE      5

struct WriteBuffer {
    bool isMute                  : 1; // 静音 = TRUE，正常 = FALSE
    bool isSearchMode            : 1; // 搜索模式 = TRUE，放音模式 = FALSE
    int  prePLL                  : 6; // 频率PLL值的前6bits
    int  postPLL                 : 8; // 频率PLL值的后8bits
    int  searchDirection         : 1; // 向前搜索 = 0，向后搜索 = 1
    int  stopSearchLevel         : 2; // 搜索模式不自动停止 = 0，搜索到微弱信号（信号强度ADC输出5）时中断搜索 = 1，搜索到中等信号（信号强度ADC输出7）时中断搜索 = 2，搜索到强烈信号（信号强度ADC输出10）时中断搜索 = 3
    int  sideBand                : 1; // 上边带接收 = 1，下边带接收 = 0
    bool isMono                  : 1; // 强制单声道输出 = TRUE，立体声输出 = FALSE
    bool isMuteLeft              : 1; // 立体声合成到右声道 = TRUE，正常声道输出 = FALSE
    bool isMuteRight             : 1; // 立体声合成到左声道 = TRUE，正常声道输出 = FALSE
    int  softwarePort1Output     : 1; // SWP1作为可编程端口使用，SWP1口输出高电平 = 1，SWP1口输出低电平 = 0
    int  softwarePort2Output     : 1; // SWP2作为可编程端口使用，SWP2口输出高电平 = 1，SWP2口输出低电平=0
    bool isStandby               : 1; // 待机模式 = TRUE，工作模式 = FALSE
    int  bandLimit               : 1; // 日本调频制式 = 1，欧美调频制式 = 0
    int  xtal                    : 1; // 使用外置13MHz晶振 = 1，使用外置32.768kHz晶振 = 0
    bool isSoftwareMute          : 1; // 开启软件静音 = TRUE，关闭软件静音 = FALSE
    bool isHighCutControl        : 1; // 开启高频旁路 = TRUE，关闭高频旁路 = FALSE
    bool isStereoNoiseCanceling  : 1; // 开启立体声噪音消除 = TRUE，关闭立体声噪音消除 = FALSE
    bool isEnableSearchIndicator : 1; // 搜索到频道是SWP1输出高电平 = TRUE，SWP1作为可编程端口使用 = FALSE
    bool isEnablePLLReference    : 1; // 启动6.5MHz锁相环参考频率 = TRUE，关闭6.5MHz锁相环参考频率 = FALSE
    int  DTC                     : 1; // 美国制去加重时间75μs = 1，其他国家去加重时间 50μs = 0

  public:
    WriteBuffer() {
      memset(this, 0, sizeof(WriteBuffer));
    }

    inline unsigned long getPLL() {
      unsigned long pll = (((unsigned long)prePLL) << 8) | postPLL;
      return pll;
    }

    inline void setPLL(unsigned long pll) {
      prePLL  = (unsigned int)((pll & 0xF0) >> 8);
      postPLL = (unsigned int)(pll & 0x0F);
    }
};

struct ReadBuffer {
    bool isReady               : 1; // 已搜索到频道或搜索到波段边界 = TRUE，否则 = FALSE
    bool isBandLimit           : 1; // 已搜索到波段边界 = TRUE，否则 = FALSE
    int  prePLL                : 6; // 当前频率的PLL值的前6bits
    int  postPLL               : 8; // 当前频率的PLL值的后8bits
    bool isStereo              : 1; // 当前信号为立体声 = TRUE，否则 = FALSE
    int  intermediateFrequency : 7; // 当前中频，正确调谐时在0x31~0x3E之间
    int  RFADClevel            : 4; // ADC输出的RF信号等级
    int  chipID                : 4; // 芯片ID

  public:
    ReadBuffer() {
      memset(this, 0, sizeof(ReadBuffer));
    }

    inline unsigned long getPLL() {
      unsigned long pll = (((unsigned long)prePLL) << 8) | postPLL;
      return pll;
    }
};

class TEA5767 {
  public:
    TEA5767();
    ~TEA5767();

    void         setFrequency(unsigned int frequencey);
    unsigned int getFrequency();
    void         setStereo(bool isStereo);
    bool         isStereo();
    void         setHighCutControl(bool enabled);
    void         setStereoNoiseControl(bool enabled);
    void         setSoftwareMute(bool enabled);
    void         search(int direction, int stopLevel);
    void         setBand(int band);
    void         setPowerOn(bool isPowerOn);
    bool         isPowerOn();
    int          getRFLevel();
    int          getChipID();

    void         setup();
    void         update();

  private:
    bool readData();
    bool writeData();

  private:
    inline unsigned int convertFrequencyToPLL(unsigned int frequency) {
      unsigned int intermediateFrequency = (mWriteBuffer->sideBand == HIGH_SIDE_BAND) ? IF : -IF;
      unsigned int referenceFrequency    = (mWriteBuffer->xtal == XTAL_32KHz) ? FREF_32KHz : FREF_13MHz;
      unsigned int pll = (4 * (frequency + intermediateFrequency)) / referenceFrequency;
      return pll;
    }

    inline unsigned int convertPLLToFrequency(unsigned int pll) {
      unsigned int intermediateFrequency = (mWriteBuffer->sideBand == HIGH_SIDE_BAND) ? IF : -IF;
      unsigned int referenceFrequency    = (mWriteBuffer->xtal == XTAL_32KHz) ? FREF_32KHz : FREF_13MHz;
      unsigned int frequency = (pll * referenceFrequency) / 4 - intermediateFrequency;
      return frequency;
    }

  private:
    bool         mIsWriteBufferDirty;
    WriteBuffer* mWriteBuffer;
    ReadBuffer*  mReadBuffer;
};

#endif
