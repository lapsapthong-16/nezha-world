#pragma once
#include <GL/freeglut.h>

struct NezhaBGState {
    bool  enabled = true;
    float time = 0.0f;
};

extern NezhaBGState gNezhaBG;

void initNezhaBackground(int seed = 1337);
void updateNezhaBackground(float dt);
void drawNezhaBackground();
void drawNezhaBackdropMountains();
