// ====================== FORCE MULTI-BYTE (MBCS) HERE =======================
#if defined(_WIN32)
  // Disable Unicode for this translation unit
#if defined(UNICODE) || defined(_UNICODE)
#undef UNICODE
#undef _UNICODE
#endif
// Ensure MBCS is defined
#ifndef _MBCS
#define _MBCS
#endif
// Stop <windows.h> from defining min/max macros that break std::min/std::max
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
// ==========================================================================

#include "legs.hpp"
#include "utils.hpp"
#include "model.hpp"
#include "animation.hpp"     // animState, crane pose, etc.
#include "prayAnimation.hpp" // rightLegLiftAnim, kungFuKick
#include "meditation.hpp"    // meditation pose
#include <GL/freeglut.h>
#include <algorithm>         // std::min/std::max

// ================= Fur texture helpers (your version) =================
static inline void fur_enable_objlinear(float sScale, float tScale) {
    GLfloat sPlane[4] = { sScale, 0.0f,   0.0f,   0.0f };
    GLfloat tPlane[4] = { 0.0f,   tScale, 0.0f,   0.0f };
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}
static inline void fur_disable_objlinear() {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
}
static inline void fur_bind_black() {
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, gTex.pandaBlack);  // loaded in main.cpp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    fur_enable_objlinear(3.0f, 3.0f);
}
static inline void fur_unbind() {
    fur_disable_objlinear();
    glBindTexture(GL_TEXTURE_2D, 0);
}
#define FUR_BEGIN_BLACK() do{ fur_bind_black(); }while(0)
#define FUR_END()         do{ fur_unbind();     }while(0)

// ============== Friend's manual per-leg movement state ==============
static float gLeftLegLiftAngle = 0.0f;  // forward (+)/backward (-) hip rotation
static float gRightLegLiftAngle = 0.0f;

// ------------------- Foot builder (with black fur) -------------------
namespace {
    // Draw foot at ankle-local origin
    void drawFootAt(float x, float ankleY, float z) {
        matBlackFur();
        FUR_BEGIN_BLACK();

        // Ankle sleeve
        glPushMatrix();
        glTranslatef(x, ankleY, z);
        glRotatef(-90, 1, 0, 0);
        drawCappedCylinder(0.21f, 0.14f, 24);
        glPopMatrix();

        // Heel
        glPushMatrix();
        glTranslatef(x, ankleY - 0.10f, z - 0.03f);
        glScalef(0.18f, 0.12f, 0.20f);
        drawSpherePrim(1.0f, 24, 16);
        glPopMatrix();

        // Toes
        glPushMatrix();
        glTranslatef(x, ankleY - 0.12f, z + 0.16f);
        glScalef(0.22f, 0.12f, 0.30f);
        drawSpherePrim(1.0f, 24, 16);
        glPopMatrix();

        FUR_END();
    }
}

// ============================= Main draw =============================
void drawLeg(bool left) {
    glPushMatrix();

    // Counters (your PolygonCounter + friend's PrimitiveCounter)
    PolygonCounter::setCurrentPart(BodyPart::LEGS);
    PrimitiveCounter::setCurrentPart(BodyPart::LEGS);

    const float side = left ? -1.f : 1.f;
    const float x = side * MS.hipX;
    const float hipY = -0.98f;

    // Pelvis shift for crane pose
    float adjustedX = x;
    float adjustedHipY = hipY;
    if (animState.currentAnim == ANIM_CRANE_POSE) {
        adjustedX += animState.cranePelvisShift * (left ? 1.0f : -1.0f);
        adjustedHipY += animState.cranePelvisShift * 0.1f;
    }

    // Unified parent hip rotation so multiple features can stack cleanly
    bool  parentHipRotationApplied = false;
    float hipRotXDeg = 0.0f; // forward flex (+ here means "we will apply -X")
    float hipRotZDeg = 0.0f; // ab/adduction

    // Right-side feature stacks
    if (!left) {
        if (rightLegLiftAnim.straightLegLiftActive) {
            hipRotXDeg += rightLegLiftAnim.straightLegAnimAngleDeg;
        }
        if (kungFuKick.isActive) {
            hipRotXDeg += kungFuKick.forwardAngleDeg;
            hipRotZDeg += kungFuKick.abductionDeg;
        }
    }

    // Friend's manual controls: add per-leg hip flex
    if (left)  hipRotXDeg += gLeftLegLiftAngle;
    else       hipRotXDeg += gRightLegLiftAngle;

    if (hipRotXDeg != 0.0f || hipRotZDeg != 0.0f) {
        glPushMatrix();
        glTranslatef(adjustedX, adjustedHipY, 0.0f);
        glRotatef(-hipRotXDeg, 1, 0, 0);      // negative: lift forward
        if (hipRotZDeg != 0.0f) glRotatef(hipRotZDeg, 0, 0, 1);
        glTranslatef(-adjustedX, -adjustedHipY, 0.0f);
        parentHipRotationApplied = true;
    }

    // -------------------- Upper leg (thigh) -- black fur --------------------
    matBlackFur();
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(adjustedX, adjustedHipY - MS.upperLegH, 0.0f);

    // Crane pose hip rotations
    if (animState.currentAnim == ANIM_CRANE_POSE) {
        if (left) {
            glRotatef(animState.craneLeftHipFlexion, 1, 0, 0);
            glRotatef(animState.craneLeftHipAbduction, 0, 0, 1);
            glRotatef(animState.craneLeftHipRotation, 0, 1, 0);
        }
        else {
            glRotatef(animState.craneRightHipFlexion, 1, 0, 0);
            glRotatef(animState.craneRightHipAdduction, 0, 0, 1);
            glRotatef(animState.craneRightHipRotation, 0, 1, 0);
        }
    }
    // Meditation cross-legged hip ad/adduction
    else if (meditation.isActive) {
        if (left)  glRotatef(meditation.leftLegBend, 0, 0, 1);
        else       glRotatef(meditation.rightLegBend, 0, 0, 1);
    }
    // Right-leg-lift bend (non-straight mode)
    else if (rightLegLiftAnim.isActive && !left
        && !rightLegLiftAnim.toeLiftActive
        && !rightLegLiftAnim.straightLegLiftActive) {
        glRotatef(rightLegLiftAnim.rightHipFlexion, 1, 0, 0);
    }

    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.23f, MS.upperLegH, 28);
    glPopMatrix();
    FUR_END();

    const float kneeY = adjustedHipY - MS.upperLegH;

    // ------------------------- Knee joint -- black fur ----------------------
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(adjustedX, kneeY, 0.0f);
    drawSpherePrim(0.17f);
    glPopMatrix();
    FUR_END();

    // -------------------- Lower leg (shin) -- black fur ---------------------
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(adjustedX, kneeY - MS.lowerLegH, 0.0f);
    glRotatef(-90, 1, 0, 0);

    // Crane pose knee
    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glRotatef(left ? animState.craneLeftKnee : animState.craneRightKnee, 1, 0, 0);
    }
    // Right-leg-lift knee flex (non-straight mode)
    else if (rightLegLiftAnim.isActive && !left
        && !rightLegLiftAnim.toeLiftActive
        && !rightLegLiftAnim.straightLegLiftActive) {
        glRotatef(rightLegLiftAnim.rightKneeFlexion, 1, 0, 0);
    }

    drawCappedCylinder(0.21f, MS.lowerLegH, 28);
    glPopMatrix();
    FUR_END();

    const float ankleY = kneeY - MS.lowerLegH;

    // ------------------------------ Foot ------------------------------------
    glPushMatrix();
    glTranslatef(adjustedX, ankleY, 0.02f);

    // Ensure foot always renders after complex rotations
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_CULL_FACE);

    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glRotatef(left ? animState.craneLeftAnkle : animState.craneRightAnkle, 1, 0, 0);
    }
    else if (meditation.isActive) {
        if (left)  glRotatef(meditation.leftFootRotate, 0, 1, 0);
        else       glRotatef(meditation.rightFootRotate, 0, 1, 0);
    }
    else if (rightLegLiftAnim.toeLiftActive && !left) {
        glRotatef(rightLegLiftAnim.toeLiftAngleDeg, 1, 0, 0);
    }
    // Straight-leg lift: ankle stays neutral

    drawFootAt(0.0f, 0.0f, 0.0f);
    glPopAttrib();
    glPopMatrix();

    if (parentHipRotationApplied) glPopMatrix(); // end parent hip transform
    glPopMatrix();
}

// ===================== Friend-style manual control APIs =====================
void liftLeftLeg() { gLeftLegLiftAngle = std::min(gLeftLegLiftAngle + 15.0f, 90.0f); glutPostRedisplay(); }
void lowerLeftLeg() { gLeftLegLiftAngle = std::max(gLeftLegLiftAngle - 15.0f, -90.0f); glutPostRedisplay(); }
void liftRightLeg() { gRightLegLiftAngle = std::min(gRightLegLiftAngle + 15.0f, 90.0f); glutPostRedisplay(); }
void lowerRightLeg() { gRightLegLiftAngle = std::max(gRightLegLiftAngle - 15.0f, -90.0f); glutPostRedisplay(); }
void resetLeftLeg() { gLeftLegLiftAngle = 0.0f; glutPostRedisplay(); }
void resetRightLeg() { gRightLegLiftAngle = 0.0f; glutPostRedisplay(); }
void resetBothLegs() { gLeftLegLiftAngle = 0.0f; gRightLegLiftAngle = 0.0f; glutPostRedisplay(); }
