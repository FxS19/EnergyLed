#include "HardwareSerial.h"
#include "FlashAnimation.h"

FlashAnimation::FlashAnimation(Led * led, uint32_t color, uint8_t fps, uint8_t cycles):
  Animation(led),
  color(color),
  cycles(cycles){
    Log::write("Creating flash animation");
    if (fps > 100) {
      fps = 100;
    }
    this->fps = 1000 - (fps*10);
    Log::writeln(" (created)");
  }

void FlashAnimation::Update(){
  delay(1);
  if (millis() > lastUpdate + fps && cycle < cycles ){
    //Log::write("Updating flash animation");
    lastUpdate = millis();
    if (counter == 0){
      //Log::write(" Fill");
      p_led->Fill(color);
      counter++;
    } else if (counter < 8) {
      //Log::write(" Fade");
      p_led->Fade();
      counter++;
    } else {
      //Log::write(" Fade Cycle");
      p_led->Fade();
      counter = 0;
      cycle++;
    }
    p_led->Show();
    //Log::writeln(" (complete)");
  }
}

bool FlashAnimation::IsComplete(){
  return cycles <= cycle;
}