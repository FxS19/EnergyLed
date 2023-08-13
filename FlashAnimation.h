#pragma once
#include <cstdint>
#include "Led.h"
#include "Animation.h"
#include "Log.h"

class FlashAnimation : public virtual Animation {
  private:
    uint32_t color {0};
    uint8_t fps {0};
    uint8_t cycles {0};
    uint8_t cycle {0};
    unsigned long lastUpdate {0};
    uint8_t counter {0};

  public:
    FlashAnimation(Led * led, uint32_t color, uint8_t fps, uint8_t cycles);
    void Update() override;
    bool IsComplete();
};