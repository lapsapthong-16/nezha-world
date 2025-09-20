#pragma once
#include <GL/freeglut.h>

struct MeditationState {
    bool  isActive = false;
    float time = 0.0f;
    float duration = 6.0f;        // total meditation time (seconds)

    // Character pose
    float meditationHeight = 0.0f;     // how high character floats above platform
    float meditationBob = 0.0f;        // gentle up/down bobbing
    float armPose = 0.0f;              // arm positioning for meditation (forward raise)
    float headTilt = 0.0f;             // slight head tilt

    // Prayer-hand movement (friend version)
    float leftHandSpread = 0.0f;      // left hand spread to side (deg)
    float rightHandSpread = 0.0f;      // right hand spread to side (deg)
    float handTouch = 0.0f;      // hands come together (deg toward center)

    // Left hand / arm extras (friend version)
    float leftHandBendBack = 0.0f;     // left hand bends up backward (deg)
    float leftArmLift = 0.0f;     // left arm lifts high (deg)

    // Eye closing animation (friend version)
    float eyeClose = 0.0f;             // 0.0=open, 1.0=closed

    // Legs (cross-legged pose)
    float leftLegBend = 0.0f;         // deg
    float rightLegBend = 0.0f;         // deg
    float leftFootRotate = 0.0f;      // deg
    float rightFootRotate = 0.0f;      // deg

    // Pants bending to follow leg movement (friend version)
    float leftPantsBend = 0.0f;       // deg
    float rightPantsBend = 0.0f;       // deg

    // Platform / lotus visual effects
    float platformRotate = 0.0f;       // deg/sec integrated
    float platformPulse = 0.0f;       // 0..1
    float platformGlow = 0.0f;       // 0..1
    float petalGlow = 0.0f;       // 0..1

    // Particle effects
    bool  particlesActive = false;
    float particleTime = 0.0f;
    float particleDuration = 2.0f;     // seconds per particle fade cycle
};

extern MeditationState meditation;

void triggerMeditation();
void updateMeditationAnimation();
void drawLotusPlatform(float x, float y, float z);
void drawMeditationParticles(float x, float y, float z);
