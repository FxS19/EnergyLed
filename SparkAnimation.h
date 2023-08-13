#pragma once
#include "Led.h"
#include "Animation.h"

class SparkAnimation : public virtual Animation {
  private:
    uint32_t color {0};
    int activeLed {0};
    uint8_t fps {0};
    uint8_t cycles {0};
    unsigned long lastUpdate {0};
    bool isDouble {false};

  public:
    SparkAnimation(Led * led, uint32_t color, uint8_t fps, bool isDouble);
    void Update() override;
    uint8_t GetCycles();

};