#pragma once
#include <GL/freeglut.h>
#include "dragonHead.hpp"

// Animation types
enum AnimationType {
    ANIM_IDLE = 0,
    ANIM_FIRE_WHEEL_DASH = 1,
    ANIM_FIRE_DRAGON_COIL = 2,
    ANIM_CRANE_POSE = 3,  // New crane pose animation
    ANIM_NONE = 4
};

// Animation state structure
struct AnimationState {
    // Current animation
    AnimationType currentAnim = ANIM_NONE;
    AnimationType previousAnim = ANIM_NONE;

    // Animation timing
    float animTime = 0.0f;
    float animDuration = 0.0f;
    bool isAnimating = false;

    // Idle animation variables
    float idleSway = 0.0f;
    float idleBob = 0.0f;
    float idleArmSwing = 0.0f;

    // Fire Wheel Dash animation variables
    float fireWheelHeight = 0.0f;        // How high Nezha flies
    float fireWheelRotation = 0.0f;      // Wheel rotation speed
    float fireWheelScale = 0.0f;         // Wheel size scaling
    bool fireWheelActive = false;

    // Body part rotations for animations
    float headNod = 0.0f;
    float leftArmSwing = 0.0f;
    float rightArmSwing = 0.0f;
    float leftLegLift = 0.0f;
    float rightLegLift = 0.0f;
    
    // Dragon fade control
    float dragonFade = 1.0f;             // 1.0 = fully visible, 0.0 = invisible
    
    // Crane pose animation variables
    float cranePoseProgress = 0.0f;      // 0-1 for transition into crane pose
    float cranePoseHold = 0.0f;          // 0-1 for holding the pose
    float craneFlyHeight = 0.0f;         // How high panda flies before crane pose
    
    // Crane pose specific rotations
    float cranePelvisShift = 0.0f;       // Pelvis translation toward stance foot
    float cranePelvisYaw = 0.0f;         // Pelvis yaw rotation
    float cranePelvisRoll = 0.0f;        // Pelvis roll tilt
    float craneSpineExtension = 0.0f;    // Spine extension
    float craneSpineSideBend = 0.0f;     // Spine side bend
    float craneChestYaw = 0.0f;          // Chest/shoulder yaw
    float craneHeadYaw = 0.0f;           // Head yaw back to camera
    float craneHeadPitch = 0.0f;         // Head pitch up
    
    // Arm rotations for crane pose
    float craneLeftShoulderAbduction = 0.0f;   // Left arm abduction (-90°)
    float craneLeftShoulderRotation = 0.0f;    // Left arm external rotation
    float craneLeftElbow = 0.0f;               // Left elbow extension
    float craneRightShoulderAbduction = 0.0f;  // Right arm abduction (+90°)
    float craneRightShoulderRotation = 0.0f;   // Right arm external rotation
    float craneRightElbow = 0.0f;              // Right elbow extension
    
    // Leg rotations for crane pose (left leg lifted, right leg stance)
    float craneRightHipFlexion = 0.0f;         // Right stance hip flexion
    float craneRightHipAdduction = 0.0f;       // Right hip adduction
    float craneRightHipRotation = 0.0f;        // Right hip external rotation
    float craneRightKnee = 0.0f;               // Right knee flexion
    float craneRightAnkle = 0.0f;              // Right ankle dorsiflexion
    
    float craneLeftHipFlexion = 0.0f;          // Left lifted hip flexion
    float craneLeftHipAbduction = 0.0f;        // Left hip abduction
    float craneLeftHipRotation = 0.0f;         // Left hip external rotation
    float craneLeftKnee = 0.0f;                // Left knee flexion
    float craneLeftAnkle = 0.0f;               // Left ankle plantarflexion
};

// Global animation state
extern AnimationState animState;

// Animation functions
void updateAnimations();
void updateIdleAnimation();
void updateFireWheelDashAnimation();
void updateFireDragonCoilAnimation();
void updateCranePoseAnimation();  // New function
void triggerIdleAnimation();
void triggerFireWheelDashAnimation();
void triggerFireDragonCoilAnimation();
void triggerCranePoseAnimation();  // New function
void drawFireWheels();
void drawFireWheelParticles(float x, float y, float z);  // New function for fire wheel particles
void drawFireDragon();