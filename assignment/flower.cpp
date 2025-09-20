#include "flower.hpp"
#include "utils.hpp"
#include <cmath>
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

FlowerBloomState flowerBloom;

static inline float smooth01(float t) { return t * t * (3.0f - 2.0f * t); }

void triggerFlowerBloom() {
    flowerBloom.isActive = true;
    flowerBloom.time = 0.0f;
    flowerBloom.progress = 0.0f;
}

void updateFlowerBloomAnimation() {
    if (!flowerBloom.isActive) return;
    flowerBloom.time += 0.016f; // ~60 FPS
    float t = flowerBloom.time / flowerBloom.duration;
    if (t >= 1.0f) {
        t = 1.0f;
        flowerBloom.isActive = false;
        flowerBloom.progress = 0.0f; // disappear at end
    }
    else {
        flowerBloom.progress = smooth01(t);
    }
}

// Draw a flat blooming flower on the ground at world position (x,y,z)
void drawFlowerBloomAt(float x, float y, float z) {
    if (flowerBloom.progress <= 0.0f) return;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(x, y + 0.01f, z);   // avoid z-fight
    glRotatef(-90.0f, 1, 0, 0);      // draw in XZ plane

    const int petals = flowerBloom.petals;
    const float R = flowerBloom.maxRadius * flowerBloom.progress;

    // center disk (untextured)
    glColor3f(0.98f, 0.86f, 0.20f);
    glutSolidTorus(0.01f * R, 0.10f * R, 10, 24);

    // petals: use lotus texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTex.lotus);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1, 1, 1);

    for (int ring = 0; ring < 2; ++ring) {
        const float ringScale = (ring == 0 ? 0.75f : 1.0f);
        for (int i = 0; i < petals; ++i) {
            float a = (2.0f * (float)M_PI * i) / petals + (ring == 0 ? (float)M_PI / petals : 0.0f);
            glPushMatrix();
            glRotatef(a * 180.0f / (float)M_PI, 0, 0, 1);
            glTranslatef(0.65f * R * ringScale, 0.0f, 0.0f);
            glScalef(0.40f * R * ringScale, 0.20f * R * ringScale, 1.0f);

            // textured "petal" sphere
            drawSpherePrim(1.0f, 16, 12);

            glPopMatrix();
        }
    }

    glPopMatrix();
    glPopAttrib();
}
