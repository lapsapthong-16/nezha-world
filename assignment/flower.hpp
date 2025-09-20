#pragma once
#include <GL/freeglut.h>

struct FlowerBloomState {
    bool  isActive = false;
    float time = 0.0f;
    float duration = 2.5f;     // total bloom time (slow)
    int   petals = 12;         // number of petals
    float maxRadius = 1.8f;    // final bloom radius in world units (bigger reach)
    float progress = 0.0f;     // 0..1
};

extern FlowerBloomState flowerBloom;

void triggerFlowerBloom();
void updateFlowerBloomAnimation();
void drawFlowerBloomAt(float x, float y, float z);


