#include "c_types.h"
#include <sys/_stdint.h>
#include <Adafruit_NeoPixel.h>
#include "Led.h"

Led::Led(float * brightness):
  p_brightness(brightness){
  ledCount = 9;
  strip = Adafruit_NeoPixel(ledCount, D7, NEO_GRB + NEO_KHZ800);
}

void Led::Begin(){
  // Turn off all LEDs at begin
  strip.begin();
  strip.clear();
  strip.show();
}

uint16_t Led::GetLedCount(){
  return ledCount;
}

// Fade all LEDs by shifting its value (divide by 2)
void Led::Fade(){
  for(uint8_t fadeLed = 0; fadeLed < 9; fadeLed++){
    uint32_t color = strip.getPixelColor(fadeLed);
    strip.setPixelColor(fadeLed, ((color >> 16) & 0xff)>>1, ((color >> 8) & 0xff)>>1, ((color) & 0xff)>>1);
  }
}

void Led::SetPixel(uint16_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  if (pixel < ledCount){
    strip.setPixelColor(pixel, *p_brightness * r, *p_brightness * g, *p_brightness * b);
  }
}

void Led::SetPixel(uint16_t pixel, uint32_t color) {
  if (pixel < ledCount){
    this->SetPixel(pixel, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
  }
}

void Led::Fill(uint32_t color){
  uint32_t r = *p_brightness * ((color >> 16) & 0XFF);
  uint16_t g = *p_brightness * ((color >> 8) & 0xFF);
  uint8_t b = *p_brightness * (color & 0xFF);
  strip.fill(r << 16 | g << 8 | b, 0, ledCount);
}

void Led::Show(){
  strip.show();
}