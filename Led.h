#pragma once
#include <Adafruit_NeoPixel.h>

class Led{
  private:
    float * p_brightness {0};
    uint16_t ledCount {0};
    Adafruit_NeoPixel strip;

  public:
    Led(float * brightness);
    void Begin();
    uint16_t GetLedCount();
    void SetBrightness(uint8_t);
    void Fade();
    void SetPixel(uint16_t, uint8_t, uint8_t, uint8_t);
    void SetPixel(uint16_t, uint32_t);
    void Fill(uint32_t);
    void Show();
};