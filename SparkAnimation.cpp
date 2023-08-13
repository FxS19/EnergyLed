#include "core_esp8266_features.h"
#include <cstdint>
#include "Animation.h"
#include "SparkAnimation.h"
#include "Log.h"

SparkAnimation::SparkAnimation(Led * led,  uint32_t color, uint8_t fps, bool isDouble):
  Animation(led),
  color(color),
  isDouble(isDouble){
    Log::write("Creating spark animation");
    if (fps > 100) {
      fps = 100;
    }
    this->fps = 1000 - (fps*10);
    Log::writeln(" (created)");
  }

void SparkAnimation::Update() {
  delay(1);
  if (millis() > lastUpdate + fps ){
    //Log::write("Updating spark animantion");
    lastUpdate = millis();
    p_led->Fade();
    p_led->SetPixel(activeLed, color);
    if (isDouble){
      p_led->SetPixel(p_led->GetLedCount() - (activeLed + 1), color);
    }
    p_led->Show();
    if (activeLed < p_led->GetLedCount() - 1){
      activeLed++;
    } else {
      activeLed = 0;
      cycles++;
    }
    //Log::writeln(" (compete)");
  }
}

uint8_t SparkAnimation::GetCycles() {
  return cycles;
}
