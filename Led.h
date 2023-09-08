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
    void SetBrightness(uint8_t brightness);
    void Fade();
    void SetPixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b);
    void SetPixel(uint16_t pixel, uint32_t rgb);
    void Fill(uint32_t rgb);
    void Show();
};