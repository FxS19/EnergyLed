#include "Animation.h"
#include "Led.h"

Animation::Animation(Led * led):
  p_led(led){}

void Animation::CLear(){
  p_led->Fill(0);
  p_led->Show();
}