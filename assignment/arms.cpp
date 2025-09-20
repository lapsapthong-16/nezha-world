#include "arms.hpp"
#include "utils.hpp"
#include "model.hpp"
#include "weapon.hpp"
#include "animation.hpp"
#include "prayAnimation.hpp"
#include "meditation.hpp"
#include <GL/freeglut.h>

// ====== State (from your friend's version) ======
float gLeftArmLiftAngle = 0.0f;   // Left arm abduction (+ = down in our mapping below)
float gRightArmLiftAngle = 0.0f;   // Right arm abduction (+ = up)

float gLeftElbowBendAngle = 0.0f; // Left elbow bends sideways (Z axis)
float gRightElbowBendAngle = 0.0f; // Right elbow bends up/down (X axis)

// ====== Panda black-fur texturing helpers (your version) ======
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
    glBindTexture(GL_TEXTURE_2D, gTex.pandaBlack);      // uses panda_black_fur.bmp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    fur_enable_objlinear(3.0f, 3.0f);                   // gentle tiling
}
static inline void fur_unbind() {
    fur_disable_objlinear();
    glBindTexture(GL_TEXTURE_2D, 0);
}
#define FUR_BEGIN_BLACK() do{ fur_bind_black(); }while(0)
#define FUR_END()         do{ fur_unbind();     }while(0)

void drawArmChain(bool left) {
    // Count both polygons and primitives for this body part
    PolygonCounter::setCurrentPart(BodyPart::ARMS);
    PrimitiveCounter::setCurrentPart(BodyPart::ARMS);

    const float side = left ? -1.f : 1.f;

    // Shoulder anchor relative to torso
    const float yShoulder = MS.torsoH * 0.5f - 0.10f;
    const float xShoulder = side * (MS.torsoTopR * 0.92f + 0.03f);
    const float zShoulder = 0.06f;

    glPushMatrix();
    glTranslatef(xShoulder, yShoulder, zShoulder);

    // ===== Shoulder pose (friend's logic preserved) =====
    if (animState.currentAnim == ANIM_CRANE_POSE) {
        if (left) {
            glRotatef(animState.craneLeftShoulderAbduction, 0, 0, 1);
            glRotatef(animState.craneLeftShoulderRotation, 1, 0, 0);
        }
        else {
            glRotatef(animState.craneRightShoulderAbduction, 0, 0, 1);
            glRotatef(animState.craneRightShoulderRotation, 1, 0, 0);
        }
    }
    else {
        if (kungFuKick.isActive &&
            ((left && (kungFuKick.torsoYawDeg > 0.0f || kungFuKick.leftArmRaiseDeg != 0.0f)) ||
                (!left && (kungFuKick.torsoYawDeg > 0.0f || kungFuKick.rightArmRaiseDeg != 0.0f)))) {
            glRotatef(left ? kungFuKick.leftArmRaiseDeg : kungFuKick.rightArmRaiseDeg, 0, 0, 1);
        }
        else {
            // ===== Individual arm lifting (friend feature) =====
            const float armLift = left ? gLeftArmLiftAngle : gRightArmLiftAngle;
            if (armLift != 0.0f) {
                // Friend’s mapping: right positive = up, left negative = down
                glRotatef(left ? -armLift : armLift, 0, 0, 1);
            }
            else {
                // Neutral pose
                glRotatef(side * 8.0f, 0, 0, 1);
                glRotatef(6.0f, 1, 0, 0);
            }

            // Extra: meditation lifts left arm across body slightly (friend feature)
            if (meditation.isActive && left) {
                glRotatef(meditation.leftArmLift, 0, 1, 0); // swing across (Y axis)
            }
        }
    }

    // ===== Shoulder joint (textured black fur) =====
    const float shoulderR = 0.23f;
    FUR_BEGIN_BLACK();
    drawSpherePrim(shoulderR);
    FUR_END();

    // ===== Upper arm (textured) =====
    const float upperArmLength = MS.upperArmH + shoulderR + MS.jointR;
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(0.0f, -shoulderR - upperArmLength * 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.upperArmR, upperArmLength, 32);
    glPopMatrix();
    FUR_END();

    // ===== Optional whole lower-arm assembly bend around the shoulder =====
    const float elbowLiftAngle = left ? gLeftElbowBendAngle : gRightElbowBendAngle;
    bool pushedLower = false;
    if (elbowLiftAngle != 0.0f) {
        glPushMatrix();
        pushedLower = true;
        if (left) {
            // Left elbow bends sideways around Z
            glRotatef(elbowLiftAngle, 0, 0, 1);
        }
        else {
            // Right elbow bends up/down around X
            glRotatef(elbowLiftAngle, 1, 0, 0);
        }
    }

    // ===== Elbow joint (textured) =====
    const float elbowY = -shoulderR - MS.upperArmH;
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(0.0f, elbowY, 0.0f);
    drawSpherePrim(MS.jointR);
    glPopMatrix();
    FUR_END();

    // ===== Forearm (textured) =====
    const float forearmLength = MS.lowerArmH + MS.jointR + MS.jointR * 0.9f;
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(0.0f, elbowY - forearmLength * 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glRotatef(left ? animState.craneLeftElbow : animState.craneRightElbow, 1, 0, 0);
    }
    drawCappedCylinder(MS.lowerArmR, forearmLength, 32);
    glPopMatrix();
    FUR_END();

    // ===== Wrist joint (textured) =====
    const float wristY = elbowY - MS.lowerArmH;
    FUR_BEGIN_BLACK();
    glPushMatrix();
    glTranslatef(0.0f, wristY, 0.0f);
    drawSpherePrim(MS.jointR * 0.90f);
    glPopMatrix();
    FUR_END();

    // ===== Hand (textured sphere) & hand pose (friend’s meditation controls) =====
    glPushMatrix();
    glTranslatef(0.0f, wristY - MS.jointR * 0.90f - 0.1f, 0.0f);

    if (meditation.isActive) {
        if (left) {
            // spread out to the left, bend back, then come forward to touch
            glRotatef(meditation.leftHandSpread, 0, 0, 1); // Z
            glRotatef(meditation.leftHandBendBack, 1, 0, 0); // X back
            glRotatef(meditation.handTouch, 1, 0, 0); // X forward
        }
        else {
            glRotatef(-meditation.rightHandSpread, 0, 0, 1); // Z opposite side
            glRotatef(meditation.handTouch, 1, 0, 0); // X forward
        }
    }

    // Attach weapon on RIGHT hand (unchanged orientation)
    if (!left) {
        glPushMatrix();
        glTranslatef(0.02f, -0.02f, -0.02f); // move to palm center
        glRotatef(-12.0f, 0, 1, 0);          // yaw outward
        glRotatef(8.0f, 0, 0, 1);          // slight roll
        glTranslatef(0.0f, 0.0f, 0.09f);     // sink grip into palm
        drawWeaponInRightHand();
        glPopMatrix();
    }

    // Hand: texture with panda black fur (fallback to material if texture missing)
    if (gTex.pandaBlack) {
        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, gTex.pandaBlack);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // bright fur look
        glScalef(0.85f, 0.75f, 0.85f);
        drawSpherePrim(0.25f, 28, 18);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopAttrib();
    }
    else {
        // fallback if texture not loaded
        matBlackFur();
        glScalef(0.85f, 0.75f, 0.85f);
        drawSpherePrim(0.25f, 28, 18);
    }

    glPopMatrix(); // end hand

    // Close the optional lower-arm transform
    if (pushedLower) glPopMatrix();

    glPopMatrix(); // end arm root
}

// ======================= Controls (friend features) =======================

void liftLeftArm() {
    gLeftArmLiftAngle += 15.0f;
    if (gLeftArmLiftAngle > 90.0f) gLeftArmLiftAngle = 90.0f;
    glutPostRedisplay();
}
void liftRightArm() {
    gRightArmLiftAngle += 15.0f;
    if (gRightArmLiftAngle > 90.0f) gRightArmLiftAngle = 90.0f;
    glutPostRedisplay();
}
void lowerLeftArm() {
    gLeftArmLiftAngle -= 15.0f;
    if (gLeftArmLiftAngle < 0.0f) gLeftArmLiftAngle = 0.0f;
    glutPostRedisplay();
}
void lowerRightArm() {
    gRightArmLiftAngle -= 15.0f;
    if (gRightArmLiftAngle < 0.0f) gRightArmLiftAngle = 0.0f;
    glutPostRedisplay();
}
void resetLeftArm() { gLeftArmLiftAngle = 0.0f; glutPostRedisplay(); }
void resetRightArm() { gRightArmLiftAngle = 0.0f; glutPostRedisplay(); }
void resetBothArms() {
    gLeftArmLiftAngle = 0.0f;
    gRightArmLiftAngle = 0.0f;
    glutPostRedisplay();
}

void bendLeftElbow() {
    gLeftElbowBendAngle += 15.0f;
    if (gLeftElbowBendAngle > 90.0f) gLeftElbowBendAngle = 90.0f;
    glutPostRedisplay();
}
void straightenLeftElbow() {
    gLeftElbowBendAngle -= 15.0f;
    if (gLeftElbowBendAngle < -90.0f) gLeftElbowBendAngle = -90.0f;
    glutPostRedisplay();
}
void bendRightElbow() {
    gRightElbowBendAngle += 15.0f;
    if (gRightElbowBendAngle > 90.0f) gRightElbowBendAngle = 90.0f;
    glutPostRedisplay();
}
void straightenRightElbow() {
    gRightElbowBendAngle -= 15.0f;
    if (gRightElbowBendAngle < -90.0f) gRightElbowBendAngle = -90.0f;
    glutPostRedisplay();
}
void resetLeftElbow() { gLeftElbowBendAngle = 0.0f; glutPostRedisplay(); }
void resetRightElbow() { gRightElbowBendAngle = 0.0f; glutPostRedisplay(); }
void resetBothElbows() {
    gLeftElbowBendAngle = 0.0f;
    gRightElbowBendAngle = 0.0f;
    glutPostRedisplay();
}
