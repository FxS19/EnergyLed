#pragma once
#include "Led.h"

class Animation {
  protected:
    Led * p_led {nullptr};
  public:
    Animation(Led * led);
    virtual void Update() = 0;
    void CLear();
};