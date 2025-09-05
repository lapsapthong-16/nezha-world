#pragma once
#include "utils.hpp"

// Cannon state variables
struct CannonState {
    // Animation state
    float canonRot = 0.0f;
    float canonRotSpeed = 0.0f;
    bool weaponOn = false;
    bool startFire = false;
    
    // Shooting animation state
    bool shootOn = false;
    bool endShoot = true;
    float attack = 0.0f;
    float attackLength = 0.0f;
    float powerBall = 0.0f;
    float powerBallSize = 0.0f;
    float attackRadius = 0.0f;
    
    // Timing and control
    bool leftShootOn = false;
    bool rightShootOn = false;
};

// Global cannon state instance
extern CannonState cannonState;

// Cannon component functions
void drawCannonBarrel();
void drawCannonMuzzleDesign();
void drawCannonShell();
void drawCannonStand();
void drawCannonDesignElements();

// Main cannon drawing function
void drawLaserCannon();

// Animation functions
void updateCannonAnimation();
void updateShootingAnimation();

// Laser beam drawing
void drawLaserBeam();

// Control functions
void toggleCannon();
void fireCannon();
