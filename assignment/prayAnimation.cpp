#include "prayAnimation.hpp"
#include <cmath>
#include "animation.hpp"
#include "utils.hpp"
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global right leg lift animation state
RightLegLiftState rightLegLiftAnim;
KungFuKickState   kungFuKick;

void updateRightLegLiftAnimation() {
    if (!rightLegLiftAnim.isActive) return;

    rightLegLiftAnim.animTime += 0.016f; // ~60 FPS
    float time = rightLegLiftAnim.animTime;
    float duration = rightLegLiftAnim.animDuration;
    (void)duration; // silence unused warning if any

    if (time < 0.5f) {
        float phase = time / 0.5f;
        rightLegLiftAnim.legLiftProgress = phase;
        rightLegLiftAnim.legLiftHold = 0.0f;

        float smoothPhase = phase * phase * (3.0f - 2.0f * phase);
        rightLegLiftAnim.rightHipFlexion = smoothPhase * 45.0f;
        rightLegLiftAnim.rightKneeFlexion = smoothPhase * 60.0f;
    }
    else if (time < 1.5f) {
        rightLegLiftAnim.legLiftProgress = 1.0f;
        rightLegLiftAnim.legLiftHold = (time - 0.5f) / 1.0f;

        float holdPhase = sinf((time - 0.5f) * 2.0f * (float)M_PI) * 0.5f;
        rightLegLiftAnim.rightHipFlexion = 45.0f + holdPhase;
        rightLegLiftAnim.rightKneeFlexion = 60.0f + holdPhase;
    }
    else if (time < 2.0f) {
        float phase = (time - 1.5f) / 0.5f;
        rightLegLiftAnim.legLiftProgress = 1.0f - phase;
        rightLegLiftAnim.legLiftHold = 1.0f;

        float legRelease = 1.0f - phase;
        rightLegLiftAnim.rightHipFlexion = 45.0f * legRelease;
        rightLegLiftAnim.rightKneeFlexion = 60.0f * legRelease;
    }
    else {
        rightLegLiftAnim.isActive = false;
        rightLegLiftAnim.animTime = 0.0f;
        rightLegLiftAnim.legLiftProgress = 0.0f;
        rightLegLiftAnim.legLiftHold = 0.0f;

        rightLegLiftAnim.rightHipFlexion = 0.0f;
        rightLegLiftAnim.rightKneeFlexion = 0.0f;
    }
}

void triggerRightLegLiftAnimation() {
    rightLegLiftAnim.isActive = true;
    rightLegLiftAnim.animTime = 0.0f;
    rightLegLiftAnim.animDuration = 2.0f;
    rightLegLiftAnim.legLiftProgress = 0.0f;
    rightLegLiftAnim.legLiftHold = 0.0f;

    rightLegLiftAnim.rightHipFlexion = 0.0f;
    rightLegLiftAnim.rightKneeFlexion = 0.0f;
}

void toggleRightToeLift() {
    rightLegLiftAnim.toeLiftActive = !rightLegLiftAnim.toeLiftActive;
    if (rightLegLiftAnim.toeLiftActive) {
        rightLegLiftAnim.isActive = false;
        rightLegLiftAnim.rightHipFlexion = 0.0f;
        rightLegLiftAnim.rightKneeFlexion = 0.0f;
    }
}

void toggleRightStraightLegLift() {
    rightLegLiftAnim.straightLegLiftActive = !rightLegLiftAnim.straightLegLiftActive;
    if (rightLegLiftAnim.straightLegLiftActive) {
        rightLegLiftAnim.isActive = false;
        rightLegLiftAnim.toeLiftActive = false;
        rightLegLiftAnim.rightHipFlexion = 0.0f;
        rightLegLiftAnim.rightKneeFlexion = 0.0f;
        rightLegLiftAnim.straightLegLowering = false;
        rightLegLiftAnim.straightLegTime = 0.0f;
    }
    else {
        rightLegLiftAnim.straightLegLowering = true;
        rightLegLiftAnim.straightLegTime = 0.0f;
    }
}

void updateRightStraightLegLift() {
    const float dt = 0.016f;
    auto smooth = [](float t) { return t * t * (3.0f - 2.0f * t); };

    if (rightLegLiftAnim.straightLegLiftActive && !rightLegLiftAnim.straightLegLowering) {
        rightLegLiftAnim.straightLegTime += dt;
        float t = rightLegLiftAnim.straightLegTime / rightLegLiftAnim.straightLegDurationUp;
        if (t > 1.0f) t = 1.0f;
        float eased = smooth(t);
        rightLegLiftAnim.straightLegAnimAngleDeg = eased * rightLegLiftAnim.straightLegLiftAngleDeg;
    }
    else if (rightLegLiftAnim.straightLegLowering) {
        rightLegLiftAnim.straightLegTime += dt;
        float t = rightLegLiftAnim.straightLegTime / rightLegLiftAnim.straightLegDurationUp;
        if (t > 1.0f) t = 1.0f;
        float eased = smooth(t);
        rightLegLiftAnim.straightLegAnimAngleDeg = (1.0f - eased) * rightLegLiftAnim.straightLegLiftAngleDeg;
        if (t >= 1.0f) {
            rightLegLiftAnim.straightLegLowering = false;
            rightLegLiftAnim.straightLegAnimAngleDeg = 0.0f;
        }
    }
}

static inline float smoothstep01(float t) { return t * t * (3.0f - 2.0f * t); }

void triggerKungFuKick() {
    kungFuKick = KungFuKickState();
    kungFuKick.isActive = true;
    rightLegLiftAnim.straightLegLiftActive = false;
    rightLegLiftAnim.straightLegLowering = false;
    rightLegLiftAnim.toeLiftActive = false;
}

void updateKungFuKickAnimation() {
    if (!kungFuKick.isActive) return;
    const float dt = 0.016f;
    kungFuKick.time += dt;

    float t = kungFuKick.time;
    float p1 = kungFuKick.phase1Dur;
    float p2 = p1 + kungFuKick.phase2Dur;
    float p3 = p2 + kungFuKick.phase3Dur;
    float p4 = p3 + kungFuKick.phase4Dur;
    float p5 = p4 + kungFuKick.holdDur;

    if (t <= p1) {
        float u = smoothstep01(t / p1);
        kungFuKick.forwardAngleDeg = u * kungFuKick.targetForward1;
        kungFuKick.abductionDeg = 0.0f;
        kungFuKick.torsoYawDeg = 0.0f;
        kungFuKick.torsoSideBendDeg = 0.0f;
        kungFuKick.headYawDeg = 0.0f;
        kungFuKick.leftArmRaiseDeg = 0.0f;
        kungFuKick.rightArmRaiseDeg = 0.0f;
        kungFuKick.flyHeight = u * (0.40f * kungFuKick.flyHeightMax);
    }
    else if (t <= p2) {
        float u = smoothstep01((t - p1) / (p2 - p1));
        kungFuKick.forwardAngleDeg = kungFuKick.targetForward1 + u * kungFuKick.swingForwardExtra;
        kungFuKick.abductionDeg = u * kungFuKick.targetAbduction;

        const float pre = 0.60f;
        kungFuKick.torsoYawDeg = u * (pre * kungFuKick.targetTorsoYaw);
        kungFuKick.torsoSideBendDeg = u * (pre * kungFuKick.targetTorsoSide);
        kungFuKick.headYawDeg = u * (pre * kungFuKick.targetHeadYaw);
        kungFuKick.leftArmRaiseDeg = u * (pre * kungFuKick.targetLeftArmRaise);
        kungFuKick.rightArmRaiseDeg = u * (pre * kungFuKick.targetRightArmRaise);

        kungFuKick.flyHeight = (0.40f + 0.30f * u) * kungFuKick.flyHeightMax;
    }
    else if (t <= p3) {
        float u = smoothstep01((t - p2) / (p3 - p2));
        kungFuKick.forwardAngleDeg = (1.0f - u) * kungFuKick.targetForward1 + u * kungFuKick.targetBack;
        kungFuKick.abductionDeg = (1.0f - u) * kungFuKick.targetAbduction;

        const float pre = 0.60f;
        kungFuKick.torsoYawDeg = pre * kungFuKick.targetTorsoYaw;
        kungFuKick.torsoSideBendDeg = pre * kungFuKick.targetTorsoSide;
        kungFuKick.headYawDeg = pre * kungFuKick.targetHeadYaw;
        kungFuKick.leftArmRaiseDeg = pre * kungFuKick.targetLeftArmRaise;
        kungFuKick.rightArmRaiseDeg = pre * kungFuKick.targetRightArmRaise;

        kungFuKick.flyHeight = (0.60f - 0.20f * u) * kungFuKick.flyHeightMax;
    }
    else if (t <= p4) {
        float u = smoothstep01((t - p3) / (p4 - p3));
        kungFuKick.forwardAngleDeg = (1.0f - u) * kungFuKick.targetBack + u * kungFuKick.targetForward2;
        kungFuKick.abductionDeg = u * kungFuKick.targetAbduction;

        const float pre = 0.60f;
        kungFuKick.torsoYawDeg = ((1.0f - u) * pre + u) * kungFuKick.targetTorsoYaw;
        kungFuKick.torsoSideBendDeg = ((1.0f - u) * pre + u) * kungFuKick.targetTorsoSide;
        kungFuKick.headYawDeg = ((1.0f - u) * pre + u) * kungFuKick.targetHeadYaw;
        kungFuKick.leftArmRaiseDeg = ((1.0f - u) * pre + u) * kungFuKick.targetLeftArmRaise;
        kungFuKick.rightArmRaiseDeg = ((1.0f - u) * pre + u) * kungFuKick.targetRightArmRaise;

        kungFuKick.flyHeight = ((1.0f - u) * 0.40f + u * 1.00f) * kungFuKick.flyHeightMax;
    }
    else if (t <= p5) {
        kungFuKick.forwardAngleDeg = kungFuKick.targetForward2;
        kungFuKick.abductionDeg = kungFuKick.targetAbduction;
        kungFuKick.torsoYawDeg = kungFuKick.targetTorsoYaw;
        kungFuKick.torsoSideBendDeg = kungFuKick.targetTorsoSide;
        kungFuKick.headYawDeg = kungFuKick.targetHeadYaw;
        kungFuKick.leftArmRaiseDeg = kungFuKick.targetLeftArmRaise;
        kungFuKick.rightArmRaiseDeg = kungFuKick.targetRightArmRaise;
        kungFuKick.flyHeight = kungFuKick.flyHeightMax;
    }
    else {
        kungFuKick.isActive = false;
        kungFuKick.flyHeight = 0.0f;
    }
}

void drawRightLegLift() {
    // Intentionally empty; transforms are applied in the main character draw.
}

// Optional handy effect: a small textured cloud puff on the ground.
void drawKickCloudAt(float x, float y, float z, float scale, float alpha) {
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTex.cloud);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);

    glPushMatrix();
    glTranslatef(x, y + 0.02f, z);
    glScalef(scale, scale, scale);

    const float r = 0.5f;
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-r, 0.0f, -r);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(r, 0.0f, -r);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(r, 0.0f, r);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-r, 0.0f, r);
    glEnd();

    glPopMatrix();
    glPopAttrib();
}
