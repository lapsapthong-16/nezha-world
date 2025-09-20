#pragma once
#include "utils.hpp"

// ===== Cannon state =====
struct CannonState {
    // Animation state
    float canonRot = 0.0f;
    float canonRotSpeed = 0.0f;
    bool  weaponOn = false;
    bool  startFire = false;

    // Shooting animation state
    bool  shootOn = false;
    bool  endShoot = true;
    float attack = 0.0f;
    float attackLength = 0.0f;
    float powerBall = 0.0f;
    float powerBallSize = 0.0f;
    float attackRadius = 0.0f;

    // Optional sides
    bool  leftShootOn = false;
    bool  rightShootOn = false;

    // Visibility control
    bool  visible = true;
};

extern CannonState cannonState;

// ===== Public API =====
void drawLaserCannonMounted();       // for shoulder (no stand)
void drawLaserCannonStandalone();    // ground turret (with stand)

// Back-compat alias: mounted version
inline void drawLaserCannon() { drawLaserCannonMounted(); }

void drawLaserBeam();
void updateCannonAnimation();
void updateShootingAnimation();
void toggleCannon();
void fireCannon();
void toggleCannonVisibility();
