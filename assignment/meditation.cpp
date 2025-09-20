#include "meditation.hpp"
#include "utils.hpp"
#include <GL/freeglut.h>
#include <cmath>
#include <algorithm>

// =====================================================
// Toggle: 1 = float up during meditation, 0 = stay put
// =====================================================
#ifndef MEDITATION_FLOATING
#define MEDITATION_FLOATING 0
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

MeditationState meditation;

static inline float smooth01(float t) { return t * t * (3.0f - 2.0f * t); }
static inline float smoothstep(float edge0, float edge1, float x) {
    float t = (x - edge0) / (edge1 - edge0);
    t = std::max(0.0f, std::min(1.0f, t));
    return t * t * (3.0f - 2.0f * t);
}

void triggerMeditation() {
    meditation = MeditationState{};
    meditation.isActive = true;
    meditation.time = 0.0f;
}

void updateMeditationAnimation() {
    if (!meditation.isActive) return;

    const float dt = 0.016f; // ~60 FPS
    meditation.time += dt;

    float t = meditation.time / meditation.duration;
    if (t >= 1.0f) {
        meditation.isActive = false;
        meditation.time = 0.0f;
        return;
    }

    // ---------- Character core pose ----------
    float posePhase = smooth01(t);

#if MEDITATION_FLOATING
    meditation.meditationHeight = posePhase * 0.3f;  // friend: float up a bit
#else
    meditation.meditationHeight = 0.0f;              // yours: keep planted
#endif

    meditation.meditationBob = 0.05f * std::sinf(meditation.time * 2.0f); // gentle bobbing
    meditation.armPose = posePhase * -45.0f;  // arms raise forward (invert)
    meditation.headTilt = 0.1f * std::sinf(meditation.time * 1.5f);  // subtle head movement

    // ---------- Friend extras: left arm & hand ----------
    meditation.leftArmLift = posePhase * 90.0f;  // left arm lifts up high
    meditation.leftHandBendBack = posePhase * 60.0f;  // left hand bends backward

    // ---------- Eye close / open cycle (3s loop) ----------
    {
        float eyePhase = std::fmod(meditation.time * 0.3f, 3.0f); // 0..3
        if (eyePhase < 1.0f)        meditation.eyeClose = eyePhase;                 // closing 0->1
        else if (eyePhase < 2.0f)   meditation.eyeClose = 1.0f;                     // hold closed
        else                        meditation.eyeClose = 1.0f - (eyePhase - 2.0f); // opening 1->0
    }

    // ---------- Prayer-hand cycle (4s): spread -> hold -> together -> hold ----------
    {
        float prayerPhase = std::fmod(meditation.time * 0.5f, 4.0f); // 0..4
        if (prayerPhase < 1.0f) {
            float spreadT = prayerPhase;       // 0..1
            meditation.leftHandSpread = spreadT * 60.0f;
            meditation.rightHandSpread = spreadT * 60.0f;
            meditation.handTouch = 0.0f;
        }
        else if (prayerPhase < 2.0f) {
            meditation.leftHandSpread = 60.0f;
            meditation.rightHandSpread = 60.0f;
            meditation.handTouch = 0.0f;
        }
        else if (prayerPhase < 3.0f) {
            float touchT = (prayerPhase - 2.0f);  // 0..1
            meditation.leftHandSpread = 60.0f * (1.0f - touchT);
            meditation.rightHandSpread = 60.0f * (1.0f - touchT);
            meditation.handTouch = touchT * 30.0f; // move toward center
        }
        else {
            meditation.leftHandSpread = 0.0f;
            meditation.rightHandSpread = 0.0f;
            meditation.handTouch = 30.0f;         // hold together
        }
    }

    // ---------- Legs (cross-legged) & pants follow ----------
    meditation.leftLegBend = -posePhase * 40.0f;   // inward
    meditation.rightLegBend = posePhase * 40.0f;   // outward (opposite)
    meditation.leftFootRotate = posePhase * 45.0f;
    meditation.rightFootRotate = -posePhase * 45.0f;
    meditation.leftPantsBend = -posePhase * 65.0f;
    meditation.rightPantsBend = posePhase * 65.0f;

    // ---------- Platform / petals ----------
    meditation.platformRotate += dt * 15.0f; // deg/sec
    meditation.platformPulse = 0.3f + 0.2f * std::sinf(meditation.time * 3.0f);
    meditation.platformGlow = 0.6f + 0.4f * std::sinf(meditation.time * 2.5f);
    meditation.petalGlow = 0.4f + 0.6f * std::sinf(meditation.time * 4.0f);

    // ---------- Particles ----------
    if (meditation.time > 1.0f) {
        meditation.particlesActive = true;
        meditation.particleTime += dt;
        if (meditation.particleTime >= meditation.particleDuration) {
            meditation.particleTime = 0.0f; // loop
        }
    }
}

void drawLotusPlatform(float x, float y, float z) {
    if (!meditation.isActive) return;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);

    // Base (slightly scaled sphere)
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(meditation.platformRotate, 0, 1, 0);
    glColor4f(0.8f, 0.9f, 1.0f, 0.7f * meditation.platformGlow);
    glScalef(1.2f, 0.1f, 1.2f);
    glutSolidSphere(1.0, 24, 12);
    glPopMatrix();

    // Lotus petals (3 layers)
    glPushMatrix();
    glTranslatef(x, y + 0.05f, z);
    glRotatef(meditation.platformRotate, 0, 1, 0);

    for (int layer = 0; layer < 3; ++layer) {
        float layerScale = 0.8f + 0.2f * layer;
        float layerHeight = 0.02f * layer;
        int   petals = 8 + layer * 4;

        glPushMatrix();
        glTranslatef(0, layerHeight, 0);
        glScalef(layerScale, 1.0f, layerScale);

        for (int i = 0; i < petals; ++i) {
            float angle = (2.0f * float(M_PI) * i) / float(petals);
            float petalGlow = meditation.petalGlow * (1.0f - 0.3f * layer);

            glPushMatrix();
            glRotatef(angle * 180.0f / float(M_PI), 0, 1, 0);
            glTranslatef(0.6f, 0, 0);
            glRotatef(-15.0f, 0, 0, 1);

            // Colors per layer
            if (layer == 0)        glColor4f(1.0f, 0.9f, 0.8f, 0.8f * petalGlow); // warm white
            else if (layer == 1)   glColor4f(0.9f, 0.8f, 1.0f, 0.7f * petalGlow); // light purple
            else                   glColor4f(0.8f, 0.9f, 1.0f, 0.6f * petalGlow); // light blue

            glScalef(0.4f, 0.15f, 0.8f);
            glutSolidSphere(1.0, 16, 8);
            glPopMatrix();
        }

        glPopMatrix();
    }

    // Central energy core
    glPushMatrix();
    glTranslatef(0, 0.08f, 0);
    glColor4f(1.0f, 1.0f, 0.9f, 0.9f * meditation.platformPulse);
    glScalef(0.3f, 0.1f, 0.3f);
    glutSolidSphere(1.0, 20, 10);
    glPopMatrix();

    glPopMatrix();
    glPopAttrib();
}

void drawMeditationParticles(float x, float y, float z) {
    if (!meditation.particlesActive) return;

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(x, y + meditation.meditationHeight + 0.5f, z);

    const int kCount = 12;
    for (int i = 0; i < kCount; ++i) {
        float angle = (2.0f * float(M_PI) * i) / float(kCount) + meditation.time * 0.5f;
        float radius = 1.2f + 0.3f * std::sinf(meditation.time * 2.0f + i);
        float height = 0.5f * std::sinf(meditation.time * 1.5f + i * 0.5f);

        float px = radius * std::cosf(angle);
        float py = height;
        float pz = radius * std::sinf(angle);

        glPushMatrix();
        glTranslatef(px, py, pz);

        float phase = std::fmod(meditation.particleTime / meditation.particleDuration, 1.0f);
        float particleAlpha = 0.8f * (1.0f - phase);
        glColor4f(0.9f, 0.95f, 1.0f, particleAlpha);

        glutSolidSphere(0.03, 8, 6);
        glPopMatrix();
    }

    glPopMatrix();
    glPopAttrib();
}
