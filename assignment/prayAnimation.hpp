#pragma once
#include <GL/freeglut.h>

// Right leg lift animation state structure
struct RightLegLiftState {
    // Animation timing
    bool isActive = false;
    float animTime = 0.0f;
    float animDuration = 2.0f; // 2 seconds total

    // Right leg lift progress (0-1)
    float legLiftProgress = 0.0f;
    float legLiftHold = 0.0f;

    // Right leg bending rotations for natural lift motion
    float rightHipFlexion = 0.0f;        // Hip flexion (thigh lifts up)
    float rightKneeFlexion = 0.0f;       // Knee flexion (leg bends at knee)

    // Ankle-only toe lift toggle (used by key '8')
    bool toeLiftActive = false;          // When true, keep thigh/shin straight and lift toes
    float toeLiftAngleDeg = 45.0f;       // Toe lift angle in degrees

    // Straight-leg lift (whole right leg lifts from hip, knee stays straight)
    bool straightLegLiftActive = false;
    float straightLegLiftAngleDeg = 45.0f; // Hip flexion angle for straight leg lift
    // Animation runtime values for straight-leg lift
    float straightLegAnimAngleDeg = 0.0f;  // Current animated angle (0..straightLegLiftAngleDeg)
    float straightLegTime = 0.0f;          // Elapsed time for current phase
    float straightLegDurationUp = 0.6f;    // Seconds to reach full lift
    bool straightLegLowering = false;      // If true, animating back to 0
};

// Kung Fu Kick sequence (right side)
struct KungFuKickState {
    bool isActive = false;
    float time = 0.0f;
    // Phases: forward lift -> swing right -> lift more + face right -> settle
    float phase1Dur = 0.70f;   // forward lift (slower)
    float phase2Dur = 0.55f;   // swing right (slower)
    float phase3Dur = 0.55f;   // swing back toward center (slower)
    float phase4Dur = 0.65f;   // lift more + rotate torso (kick) (slower)
    float holdDur   = 0.40f;   // brief hold (longer)
    // Animated values
    float forwardAngleDeg = 0.0f;   // hip flexion (around X)
    float abductionDeg = 0.0f;      // swing to right (around Z)
    float torsoYawDeg = 0.0f;       // face right (around Y)
    float torsoSideBendDeg = 0.0f;  // slight bend toward right (around Z)
    float headYawDeg = 0.0f;        // head follows during kick phase only (around Y)
    float leftArmRaiseDeg = 0.0f;   // left arm abduction during kick phase only (around Z)
    float rightArmRaiseDeg = 0.0f;  // right arm abduction during kick phase only (around Z)
    // Targets
    float targetForward1 = 45.0f;
    float targetAbduction = 55.0f;   // swing farther to the right
    float targetBack = -15.0f;       // slight hip extension when swinging back
    float targetForward2 = 60.0f;
    float targetTorsoYaw = 45.0f;
    float targetTorsoSide = 10.0f;
    float targetHeadYaw = 35.0f;    // head turns slightly less than torso
    float targetLeftArmRaise = -90.0f; // left arm straight out to the side
    float targetRightArmRaise = 90.0f; // right arm straight out to the side

    // Small forward continuation during Phase 2 to avoid a visible pause
    float swingForwardExtra = 8.0f;   // extra degrees added across phase 2

    // Weapon flight (during swing and return on kick)
    bool  weaponDetached = false;     // if true, render weapon offset from hand
    float weaponFlyY = 0.0f;          // upward offset in character space (units)
    float weaponSpinDeg = 0.0f;       // spin around its own axis while flying
    float weaponRiseTargetY = 1.0f;   // max rise height during phase 2 (visible)
    float weaponHoverDrop = 0.20f;    // slight drop in phase 3 before returning

    // Character flight (lift whole character while performing the move)
    float flyHeight = 0.0f;           // applied in main during drawing
    float flyHeightMax = 1.4f;        // maximum lift height
};

// Global right leg lift animation state
extern RightLegLiftState rightLegLiftAnim;
extern KungFuKickState kungFuKick;

// Right leg lift animation functions
void updateRightLegLiftAnimation();
void triggerRightLegLiftAnimation();
void drawRightLegLift();
// Toggle right foot toe lift (ankle dorsiflexion) without moving thigh/shin
void toggleRightToeLift();
// Toggle straight-leg lift (right leg raised from hip with knee straight)
void toggleRightStraightLegLift();
// Update straight-leg lift animation (call each frame)
void updateRightStraightLegLift();

// Kung Fu Kick controls
void triggerKungFuKick();
void updateKungFuKickAnimation();