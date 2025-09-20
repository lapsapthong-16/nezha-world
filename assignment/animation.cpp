#include "animation.hpp"
#include "utils.hpp"
#include <cmath>

// Global animation state
AnimationState animState;

// Forward declarations for the helper functions
void updateIdleAnimation();
void updateFireWheelDashAnimation();
void updateFireDragonCoilAnimation();
void updateCranePoseAnimation();  // New function

void updateAnimations() {
    if (!animState.isAnimating) return;

    // Update animation time
    animState.animTime += 0.016f; // ~60 FPS

    switch (animState.currentAnim) {
    case ANIM_IDLE:
        updateIdleAnimation();
        break;
    case ANIM_FIRE_WHEEL_DASH:
        updateFireWheelDashAnimation();
        break;
    case ANIM_FIRE_DRAGON_COIL:
        updateFireDragonCoilAnimation();
        break;
    case ANIM_CRANE_POSE:  // New case
        updateCranePoseAnimation();
        break;
    default:
        break;
    }
}

void updateIdleAnimation() {
    // Continuous idle animation - never ends
    float time = animState.animTime;

    // Gentle swaying motion
    animState.idleSway = sin(time * 0.5f) * 2.0f; // 2 degrees sway

    // Gentle bobbing motion
    animState.idleBob = sin(time * 0.8f) * 0.02f; // Small vertical movement

    // Arm swinging
    animState.idleArmSwing = sin(time * 0.6f) * 3.0f; // 3 degrees arm swing
    animState.leftArmSwing = animState.idleArmSwing;
    animState.rightArmSwing = -animState.idleArmSwing; // Opposite direction

    // Head gentle nod
    animState.headNod = sin(time * 0.4f) * 1.0f; // 1 degree head nod

    // Leg weight shifting
    animState.leftLegLift = sin(time * 0.3f) * 0.5f;
    animState.rightLegLift = -animState.leftLegLift;
}
void updateFireWheelDashAnimation() {
    float time = animState.animTime;
    float duration = 4.0f; // 4 seconds total animation

    if (time < 1.0f) {
        // Phase 1: Fly up (first second)
        float phase = time / 1.0f;
        animState.fireWheelHeight = phase * 2.0f; // Fly up 2 units
        animState.fireWheelScale = 0.0f; // Wheels not visible yet
    }
    else if (time < 2.0f) {
        // Phase 2: Wheels appear and start spinning (second second)
        float phase = (time - 1.0f) / 1.0f;
        animState.fireWheelHeight = 2.0f; // Stay at height
        animState.fireWheelScale = phase * 1.0f; // Wheels grow in
        animState.fireWheelRotation += 50.0f; // Faster spinning
    }
    else if (time < 3.5f) {
        // Phase 3: Very fast spinning wheels (1.5 seconds)
        animState.fireWheelHeight = 2.0f;
        animState.fireWheelScale = 1.0f;
        animState.fireWheelRotation += 80.0f; // Very fast spinning
        animState.fireWheelActive = true;
    }
    else if (time < 4.0f) {
        // Phase 4: Land down (last 0.5 seconds)
        float phase = (time - 3.5f) / 0.5f;
        animState.fireWheelHeight = 2.0f * (1.0f - phase); // Come down
        animState.fireWheelScale = 1.0f - phase; // Wheels shrink
        animState.fireWheelRotation += 50.0f; // Still spinning while landing
    }
    else {
        // Animation complete
        animState.isAnimating = false;
        animState.currentAnim = ANIM_NONE;
        animState.fireWheelHeight = 0.0f;
        animState.fireWheelScale = 0.0f;
        animState.fireWheelActive = false;
    }
}

void triggerFireWheelDashAnimation() {
    animState.currentAnim = ANIM_FIRE_WHEEL_DASH;
    animState.isAnimating = true;
    animState.animTime = 0.0f;
    animState.animDuration = 4.0f;
    animState.fireWheelHeight = 0.0f;
    animState.fireWheelRotation = 0.0f;
    animState.fireWheelScale = 0.0f;
    animState.fireWheelActive = false;
    animState.dragonFade = 1.0f; // Reset dragon fade
}
void updateFireDragonCoilAnimation() {
    // Update dragon animation
    updateDragonHeadAnimation();
    
    // After dragon finishes, transition to crane pose
    static float dragonTime = 0.0f;
    static bool wasInactive = true;
    
    // Reset timer when animation starts
    if (wasInactive && animState.isAnimating && animState.currentAnim == ANIM_FIRE_DRAGON_COIL) {
        dragonTime = 0.0f;
        wasInactive = false;
    }
    
    // Track if animation becomes inactive
    if (!animState.isAnimating || animState.currentAnim != ANIM_FIRE_DRAGON_COIL) {
        wasInactive = true;
        return;
    }
    
    dragonTime += 0.016f;
    
    if (dragonTime >= 5.5f) { // After dragon animation completes
        triggerCranePoseAnimation();
        dragonTime = 0.0f; // Reset for next time
    }
}

// New crane pose animation function
void updateCranePoseAnimation() {
    float time = animState.animTime;
    float flyDuration = 1.0f; // 1 second to fly up
    float transitionDuration = 1.0f; // 1 second to transition into pose
    float holdDuration = 2.5f; // 2.5 seconds to hold the pose
    float landDuration = 0.5f; // 0.5 seconds to land and release pose
    
    if (time < flyDuration) {
        // Phase 1: Fly up gracefully (0-1s)
        float phase = time / flyDuration;
        animState.craneFlyHeight = phase * 1.5f; // Fly up 1.5 units
        animState.cranePoseProgress = 0.0f;
        animState.cranePoseHold = 0.0f;
        
        // Smooth flying motion with slight sway
        float flyPhase = phase * phase * (3.0f - 2.0f * phase); // Ease-in-out
        animState.craneFlyHeight = flyPhase * 1.5f;
        
        // Add gentle floating motion
        animState.idleBob = sin(time * 2.0f) * 0.05f; // Gentle floating
        animState.idleSway = sin(time * 1.5f) * 1.0f; // Gentle swaying while flying
        
        // Keep fire particles active during flying
        dragonHead.isBreathingFire = true;
        dragonHead.fireIntensity = 1.0f;
        dragonHead.fireParticleCount = 50.0f;
        
        // Reset all pose values during flying
        animState.cranePelvisShift = 0.0f;
        animState.cranePelvisYaw = 0.0f;
        animState.cranePelvisRoll = 0.0f;
        animState.craneSpineExtension = 0.0f;
        animState.craneSpineSideBend = 0.0f;
        animState.craneChestYaw = 0.0f;
        animState.craneHeadYaw = 0.0f;
        animState.craneHeadPitch = 0.0f;
        
        // Reset arm values
        animState.craneLeftShoulderAbduction = 0.0f;
        animState.craneLeftShoulderRotation = 0.0f;
        animState.craneLeftElbow = 0.0f;
        animState.craneRightShoulderAbduction = 0.0f;
        animState.craneRightShoulderRotation = 0.0f;
        animState.craneRightElbow = 0.0f;
        
        // Reset leg values
        animState.craneRightHipFlexion = 0.0f;
        animState.craneRightHipAdduction = 0.0f;
        animState.craneRightHipRotation = 0.0f;
        animState.craneRightKnee = 0.0f;
        animState.craneRightAnkle = 0.0f;
        animState.craneLeftHipFlexion = 0.0f;
        animState.craneLeftHipAbduction = 0.0f;
        animState.craneLeftHipRotation = 0.0f;
        animState.craneLeftKnee = 0.0f;
        animState.craneLeftAnkle = 0.0f;
    }
    else if (time < flyDuration + transitionDuration) {
        // Phase 2: Transition into crane pose while floating (1-2s)
        float phase = (time - flyDuration) / transitionDuration;
        animState.cranePoseProgress = phase;
        animState.cranePoseHold = 0.0f;
        animState.craneFlyHeight = 1.5f; // Stay at flying height
        
        // Smooth transition using ease-in-out
        float smoothPhase = phase * phase * (3.0f - 2.0f * phase);
        
        // Keep fire particles active during transition
        dragonHead.isBreathingFire = true;
        dragonHead.fireIntensity = 1.0f;
        dragonHead.fireParticleCount = 50.0f;
        
        // Pelvis adjustments
        animState.cranePelvisShift = smoothPhase * 0.05f; // 3-5cm toward stance foot
        animState.cranePelvisYaw = smoothPhase * 10.0f; // +10° toward lifted leg
        animState.cranePelvisRoll = smoothPhase * -5.0f; // -5° roll
        
        // Spine adjustments
        animState.craneSpineExtension = smoothPhase * 5.0f; // +5° extension
        animState.craneSpineSideBend = smoothPhase * -5.0f; // -5° side-bend toward stance foot
        animState.craneChestYaw = smoothPhase * 10.0f; // +10° yaw toward lifted leg
        
        // Head adjustments
        animState.craneHeadYaw = smoothPhase * -10.0f; // -10° yaw back to camera
        animState.craneHeadPitch = smoothPhase * 5.0f; // +5° pitch up
        
        // Arm adjustments - straight out to sides
        animState.craneLeftShoulderAbduction = smoothPhase * -90.0f; // Left arm abduction
        animState.craneLeftShoulderRotation = smoothPhase * 15.0f; // External rotation
        animState.craneLeftElbow = smoothPhase * 5.0f; // Nearly straight
        
        animState.craneRightShoulderAbduction = smoothPhase * 90.0f; // Right arm abduction
        animState.craneRightShoulderRotation = smoothPhase * 15.0f; // External rotation
        animState.craneRightElbow = smoothPhase * 5.0f; // Nearly straight
        
        // Leg adjustments - left leg lifted, right leg stance
        // Right leg (stance)
        animState.craneRightHipFlexion = smoothPhase * 5.0f; // +5° flexion
        animState.craneRightHipAdduction = smoothPhase * 3.0f; // +3° adduction
        animState.craneRightHipRotation = smoothPhase * 5.0f; // +5° external rotation
        animState.craneRightKnee = smoothPhase * 15.0f; // +15° flexion (soft)
        animState.craneRightAnkle = smoothPhase * 5.0f; // +5° dorsiflexion
        
        // Left leg (lifted)
        animState.craneLeftHipFlexion = smoothPhase * 70.0f; // +70° flexion
        animState.craneLeftHipAbduction = smoothPhase * 10.0f; // +10° abduction
        animState.craneLeftHipRotation = smoothPhase * 20.0f; // +20° external rotation
        animState.craneLeftKnee = smoothPhase * 90.0f; // +90° flexion
        animState.craneLeftAnkle = smoothPhase * 20.0f; // +20° plantarflexion
        
        // Continue gentle floating while transitioning
        animState.idleBob = sin(time * 2.0f) * 0.03f; // Reduced floating
        animState.idleSway = sin(time * 1.5f) * 0.5f; // Reduced swaying
    }
    else if (time < flyDuration + transitionDuration + holdDuration) {
        // Phase 3: Hold crane pose while floating (2-4.5s)
        float holdPhase = (time - flyDuration - transitionDuration) / holdDuration;
        animState.cranePoseProgress = 1.0f;
        animState.cranePoseHold = holdPhase;
        animState.craneFlyHeight = 1.5f; // Stay at flying height
        
        // Add subtle breathing/micro-movements during hold
        float breathPhase = sin(holdPhase * 2.0f * M_PI * 2.0f) * 0.1f; // 2 breath cycles
        
        // Keep fire particles active during hold with some variation
        dragonHead.isBreathingFire = true;
        dragonHead.fireIntensity = 0.8f + 0.2f * sin(time * 3.0f); // Varying intensity
        dragonHead.fireParticleCount = 40.0f + 10.0f * sin(time * 2.0f); // Varying particle count
        
        // Keep all pose values at maximum
        animState.cranePelvisShift = 0.05f;
        animState.cranePelvisYaw = 10.0f;
        animState.cranePelvisRoll = -5.0f;
        animState.craneSpineExtension = 5.0f + breathPhase;
        animState.craneSpineSideBend = -5.0f;
        animState.craneChestYaw = 10.0f;
        animState.craneHeadYaw = -10.0f;
        animState.craneHeadPitch = 5.0f + breathPhase * 0.5f;
        
        // Arms
        animState.craneLeftShoulderAbduction = -90.0f;
        animState.craneLeftShoulderRotation = 15.0f;
        animState.craneLeftElbow = 5.0f;
        animState.craneRightShoulderAbduction = 90.0f;
        animState.craneRightShoulderRotation = 15.0f;
        animState.craneRightElbow = 5.0f;
        
        // Legs
        animState.craneRightHipFlexion = 5.0f;
        animState.craneRightHipAdduction = 3.0f;
        animState.craneRightHipRotation = 5.0f;
        animState.craneRightKnee = 15.0f;
        animState.craneRightAnkle = 5.0f;
        
        animState.craneLeftHipFlexion = 70.0f;
        animState.craneLeftHipAbduction = 10.0f;
        animState.craneLeftHipRotation = 20.0f;
        animState.craneLeftKnee = 90.0f;
        animState.craneLeftAnkle = 20.0f;
        
        // Gentle floating while holding pose
        animState.idleBob = sin(time * 1.5f) * 0.02f; // Very gentle floating
        animState.idleSway = sin(time * 1.0f) * 0.3f; // Very gentle swaying
    }
    else if (time < flyDuration + transitionDuration + holdDuration + landDuration) {
        // Phase 4: Gently land and release the pose (4.5-5s)
        float landPhase = (time - flyDuration - transitionDuration - holdDuration) / landDuration;
        
        // Smooth landing with ease-out curve
        float smoothLandPhase = 1.0f - (1.0f - landPhase) * (1.0f - landPhase); // Ease-out
        
        // Gradually descend
        animState.craneFlyHeight = 1.5f * (1.0f - smoothLandPhase);
        
        // Gradually release the crane pose
        float poseRelease = 1.0f - smoothLandPhase;
        animState.cranePoseProgress = poseRelease;
        animState.cranePoseHold = 1.0f - landPhase; // Hold phase decreases
        
        // Gradually fade out the dragon
        animState.dragonFade = 1.0f - smoothLandPhase;
        
        // Update dragon animation during landing phase to trigger retraction
        // Map crane pose time (4.5-5s) to dragon retraction time (5.5-6.5s)
        float dragonTime = 5.5f + (time - 4.5f) * 2.0f; // Scale 0.5s crane time to 1.0s dragon time
        updateDragonHeadAnimationWithTime(dragonTime);
        
        // Fire particles fade out during landing
        dragonHead.isBreathingFire = true;
        dragonHead.fireIntensity = (1.0f - smoothLandPhase) * 0.8f;
        dragonHead.fireParticleCount = (1.0f - smoothLandPhase) * 40.0f;
        
        // Smoothly transition all pose values back to neutral
        // Pelvis adjustments
        animState.cranePelvisShift = 0.05f * poseRelease;
        animState.cranePelvisYaw = 10.0f * poseRelease;
        animState.cranePelvisRoll = -5.0f * poseRelease;
        
        // Spine adjustments
        animState.craneSpineExtension = 5.0f * poseRelease;
        animState.craneSpineSideBend = -5.0f * poseRelease;
        animState.craneChestYaw = 10.0f * poseRelease;
        
        // Head adjustments
        animState.craneHeadYaw = -10.0f * poseRelease;
        animState.craneHeadPitch = 5.0f * poseRelease;
        
        // Arm adjustments - gradually return to neutral
        animState.craneLeftShoulderAbduction = -90.0f * poseRelease;
        animState.craneLeftShoulderRotation = 15.0f * poseRelease;
        animState.craneLeftElbow = 5.0f * poseRelease;
        
        animState.craneRightShoulderAbduction = 90.0f * poseRelease;
        animState.craneRightShoulderRotation = 15.0f * poseRelease;
        animState.craneRightElbow = 5.0f * poseRelease;
        
        // Leg adjustments - gradually return to neutral
        // Right leg (stance)
        animState.craneRightHipFlexion = 5.0f * poseRelease;
        animState.craneRightHipAdduction = 3.0f * poseRelease;
        animState.craneRightHipRotation = 5.0f * poseRelease;
        animState.craneRightKnee = 15.0f * poseRelease;
        animState.craneRightAnkle = 5.0f * poseRelease;
        
        // Left leg (lifted) - gradually lower
        animState.craneLeftHipFlexion = 70.0f * poseRelease;
        animState.craneLeftHipAbduction = 10.0f * poseRelease;
        animState.craneLeftHipRotation = 20.0f * poseRelease;
        animState.craneLeftKnee = 90.0f * poseRelease;
        animState.craneLeftAnkle = 20.0f * poseRelease;
        
        // Gentle landing motion - reduce floating as we land
        animState.idleBob = sin(time * 2.0f) * 0.01f * (1.0f - landPhase); // Fade out floating
        animState.idleSway = sin(time * 1.0f) * 0.1f * (1.0f - landPhase); // Fade out swaying
    }
    else {
        // Animation complete - reset everything
        animState.isAnimating = false;
        animState.currentAnim = ANIM_NONE;
        animState.craneFlyHeight = 0.0f;
        animState.cranePoseProgress = 0.0f;
        animState.cranePoseHold = 0.0f;
        animState.dragonFade = 0.0f; // Ensure dragon is fully hidden
        
        // Reset dragon state completely
        dragonHead.isActive = false;
        dragonHead.headY = 0.0f;
        dragonHead.scale = 0.0f;
        dragonHead.featureSize = 0.0f;
        dragonHead.hornSize = 0.0f;
        dragonHead.teethSize = 0.0f;
        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;
        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
        
        // Reset all crane pose values
        animState.cranePelvisShift = 0.0f;
        animState.cranePelvisYaw = 0.0f;
        animState.cranePelvisRoll = 0.0f;
        animState.craneSpineExtension = 0.0f;
        animState.craneSpineSideBend = 0.0f;
        animState.craneChestYaw = 0.0f;
        animState.craneHeadYaw = 0.0f;
        animState.craneHeadPitch = 0.0f;
        
        // Reset arm values
        animState.craneLeftShoulderAbduction = 0.0f;
        animState.craneLeftShoulderRotation = 0.0f;
        animState.craneLeftElbow = 0.0f;
        animState.craneRightShoulderAbduction = 0.0f;
        animState.craneRightShoulderRotation = 0.0f;
        animState.craneRightElbow = 0.0f;
        
        // Reset leg values
        animState.craneRightHipFlexion = 0.0f;
        animState.craneRightHipAdduction = 0.0f;
        animState.craneRightHipRotation = 0.0f;
        animState.craneRightKnee = 0.0f;
        animState.craneRightAnkle = 0.0f;
        animState.craneLeftHipFlexion = 0.0f;
        animState.craneLeftHipAbduction = 0.0f;
        animState.craneLeftHipRotation = 0.0f;
        animState.craneLeftKnee = 0.0f;
        animState.craneLeftAnkle = 0.0f;
    }
}

void drawFireDragon() {
    // Call both dragon head and body drawing
    drawDragonHead();
    drawDragonBody();
    drawFireParticles();  // Add fire particles
}

void triggerIdleAnimation() {
    animState.currentAnim = ANIM_IDLE;
    animState.isAnimating = true;
    animState.animTime = 0.0f;
    animState.animDuration = 0.0f; // Infinite duration
    animState.dragonFade = 1.0f; // Reset dragon fade
    triggerDragonHead();
}


void triggerFireDragonCoilAnimation() {
    animState.currentAnim = ANIM_FIRE_DRAGON_COIL;
    animState.isAnimating = true;
    animState.animTime = 0.0f;
    animState.animDuration = 1.0f;
    animState.dragonFade = 1.0f; // Reset dragon fade
    triggerDragonHead(); // Call the dragon head trigger
}

// New crane pose trigger function
void triggerCranePoseAnimation() {
    animState.currentAnim = ANIM_CRANE_POSE;
    animState.isAnimating = true;
    animState.animTime = 0.0f;
    animState.animDuration = 5.0f; // 1s fly + 1s transition + 2.5s hold + 0.5s land
    
    // Reset all crane pose values
    animState.cranePoseProgress = 0.0f;
    animState.cranePoseHold = 0.0f;
    animState.craneFlyHeight = 0.0f;
    animState.dragonFade = 1.0f; // Start with dragon fully visible
    
    // Reset all crane pose values
    animState.cranePelvisShift = 0.0f;
    animState.cranePelvisYaw = 0.0f;
    animState.cranePelvisRoll = 0.0f;
    animState.craneSpineExtension = 0.0f;
    animState.craneSpineSideBend = 0.0f;
    animState.craneChestYaw = 0.0f;
    animState.craneHeadYaw = 0.0f;
    animState.craneHeadPitch = 0.0f;
    
    // Reset arm values
    animState.craneLeftShoulderAbduction = 0.0f;
    animState.craneLeftShoulderRotation = 0.0f;
    animState.craneLeftElbow = 0.0f;
    animState.craneRightShoulderAbduction = 0.0f;
    animState.craneRightShoulderRotation = 0.0f;
    animState.craneRightElbow = 0.0f;
    
    // Reset leg values
    animState.craneRightHipFlexion = 0.0f;
    animState.craneRightHipAdduction = 0.0f;
    animState.craneRightHipRotation = 0.0f;
    animState.craneRightKnee = 0.0f;
    animState.craneRightAnkle = 0.0f;
    animState.craneLeftHipFlexion = 0.0f;
    animState.craneLeftHipAbduction = 0.0f;
    animState.craneLeftHipRotation = 0.0f;
    animState.craneLeftKnee = 0.0f;
    animState.craneLeftAnkle = 0.0f;
}

void drawFireWheels() {
    if (!animState.fireWheelActive || animState.fireWheelScale <= 0.0f) return;

    // Set golden material for wheels
    const GLfloat goldAmbient[] = { 0.24725f, 0.1995f, 0.0745f, 1.0f };
    const GLfloat goldDiffuse[] = { 0.75164f, 0.60648f, 0.22648f, 1.0f };
    const GLfloat goldSpecular[] = { 0.628281f, 0.555802f, 0.366065f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, goldAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, goldDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, goldSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 51.2f);

    // Draw left wheel
    glPushMatrix();
    glTranslatef(-0.3f, -0.5f, 0.0f); // Position under left foot
    glRotatef(animState.fireWheelRotation, 0, 1, 0); // Rotate around Y axis (sideways)
    glScalef(animState.fireWheelScale, animState.fireWheelScale, animState.fireWheelScale);
    
    // Draw circular wheel with empty center using torus
    glutSolidTorus(0.08f, 0.4f, 16, 32); // Inner radius, outer radius, sides, rings

    // Add wheel spokes for more detail
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
        glRotatef(i * 30.0f, 0, 1, 0); // Rotate around Y axis for sideways spokes
        glTranslatef(0.3f, 0, 0);
        glScalef(0.6f, 0.02f, 0.02f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    // Draw fire particles around the wheel
    drawFireWheelParticles(-0.3f, -0.5f, 0.0f);
    
    glPopMatrix();

    // Draw right wheel
    glPushMatrix();
    glTranslatef(0.3f, -0.5f, 0.0f); // Position under right foot
    glRotatef(animState.fireWheelRotation, 0, 1, 0); // Rotate around Y axis (sideways)
    glScalef(animState.fireWheelScale, animState.fireWheelScale, animState.fireWheelScale);
    
    // Draw circular wheel with empty center using torus
    glutSolidTorus(0.08f, 0.4f, 16, 32); // Inner radius, outer radius, sides, rings

    // Add wheel spokes for more detail
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
        glRotatef(i * 30.0f, 0, 1, 0); // Rotate around Y axis for sideways spokes
        glTranslatef(0.3f, 0, 0);
        glScalef(0.6f, 0.02f, 0.02f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }
    
    // Draw fire particles around the wheel
    drawFireWheelParticles(0.3f, -0.5f, 0.0f);
    
    glPopMatrix();
}

// Updated function to draw fire particles around the wheels
void drawFireWheelParticles(float x, float y, float z) {
    if (!animState.fireWheelActive || animState.fireWheelScale <= 0.0f) return;
    
    // Enable blending for fire effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Disable depth writing for transparency
    glDepthMask(GL_FALSE);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Set fire material
    const GLfloat fireAmbient[] = { 0.8f, 0.3f, 0.0f, 0.8f };
    const GLfloat fireDiffuse[] = { 1.0f, 0.4f, 0.0f, 0.8f };
    const GLfloat fireSpecular[] = { 1.0f, 0.6f, 0.0f, 0.8f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fireAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fireDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fireSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
    
    // Draw fire particles in a circle around the wheel (sideways)
    int numParticles = 20;
    float wheelRadius = 0.4f * animState.fireWheelScale;
    
    for (int i = 0; i < numParticles; i++) {
        float angle = (float)i / (float)numParticles * 2.0f * M_PI;
        float radius = wheelRadius + 0.1f + ((float)rand() / RAND_MAX) * 0.2f; // Random distance from wheel
        
        // Random height variation
        float height = ((float)rand() / RAND_MAX) * 0.3f - 0.15f;
        
        // Random size for each particle
        float particleSize = 0.02f + ((float)rand() / RAND_MAX) * 0.05f;
        
        glPushMatrix();
        // Position particles in a circle around the wheel (sideways)
        glTranslatef(cos(angle) * radius, height, sin(angle) * radius);
        glScalef(particleSize, particleSize, particleSize);
        
        // Draw particle as a small sphere
        glutSolidSphere(1.0f, 8, 6);
        
        glPopMatrix();
    }
    
    // Reset blending
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    glPopMatrix();
}

