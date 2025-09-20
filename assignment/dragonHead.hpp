#pragma once
#include <GL/freeglut.h>

// Dragon head state structure
struct DragonHeadState {
    bool isActive = false;
    float headY = 0.0f;           // Head position
    float scale = 0.0f;           // Overall size
    float featureSize = 0.0f;     // Feature animation
    float hornSize = 0.0f;        // Horn animation
    float teethSize = 0.0f;       // Teeth animation
    float headTilt = 0.0f;        // Head tilt
    
    // Phase 2 - Dragon body spiral animation
    float bodyProgress = 0.0f;    // How much of the body has appeared (0-1)
    float spiralAngle = 0.0f;     // Current spiral rotation angle
    float bodyScale = 0.0f;       // Body scaling during animation
    
    // Phase 3 - Fire breathing animation
    bool isBreathingFire = false; // Whether dragon is breathing fire
    float fireIntensity = 0.0f;   // Fire intensity (0-1)
    float fireParticleCount = 0.0f; // Number of fire particles
    
    // Phase 4 - Dragon retraction and disappearance
    bool isRetracting = false;    // Whether dragon is retracting
    float retractionProgress = 0.0f; // How much the body has retracted (0-1)
    float headShrinkProgress = 0.0f; // How much the head has shrunk (0-1)
    float finalHeadScale = 0.0f;  // Final head scale before disappearing
};

// Global dragon head state
extern DragonHeadState dragonHead;

// Dragon head functions
void drawDragonHead();
void updateDragonHeadAnimation();
void updateDragonHeadAnimationWithTime(float animTime); // New function with time parameter
void triggerDragonHead();
void resetDragonHeadAnimation(); // New function to reset animation timer
void drawDragonBody();
void drawFireParticles();  // New function for drawing fire particles