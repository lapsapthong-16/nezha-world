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
// Stop <windows.h> from defining min/max macros
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
// ==========================================================================

#include "cannon.hpp"
#include "utils.hpp"        // draw* helpers, PrimitiveCounter, gTex
#include <cmath>
#include <cstdio>
#include <GL/freeglut.h>
#include <GL/glu.h>

// ===== Global state =====
CannonState cannonState;

// ===== Materials =====
static void setCannonMaterial(float r, float g, float b) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    const GLfloat spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
}
static void matCannonMetal() { setCannonMaterial(0.75f, 0.75f, 0.75f); }
static void matCannonAccent() { setCannonMaterial(1.00f, 0.00f, 0.00f); }
static void matCannonOrange() { setCannonMaterial(1.00f, 0.50f, 0.00f); }

// ===== Local GLU helper: textured cylinder + both caps =====
static void drawTexturedCylinderCaps(float br, double tr, double h, int slices = 30) {
    // sides
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    GLUquadric* cyl = gluNewQuadric();
    gluQuadricNormals(cyl, GLU_SMOOTH);
    gluQuadricTexture(cyl, GL_TRUE);
    gluCylinder(cyl, br, tr, h, slices, 1);

    // caps
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM, 2);
    GLUquadric* disk = gluNewQuadric();
    gluQuadricNormals(disk, GLU_SMOOTH);
    gluQuadricTexture(disk, GL_TRUE);
    gluDisk(disk, 0.0, br, slices, 1);
    glPushMatrix();
    glTranslatef(0, 0, (GLfloat)h);
    gluDisk(disk, 0.0, tr, slices, 1);
    glPopMatrix();

    gluDeleteQuadric(disk);
    gluDeleteQuadric(cyl);
}

// ===== Private geometry pieces =====
static void drawCannonMuzzleDesign() {
    matCannonAccent();
    const float pos[4][2] = { {0,2.9f},{-2.9f,0},{0,-2.9f},{2.9f,0} };
    const float rot[4] = { 0, 90, 180, -90 };
    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glTranslatef(pos[i][0], pos[i][1], 7.0f);
        glRotatef(rot[i], 0, 0, 1);
        glRotatef(-3.576f, 1, 0, 0);

        glPushMatrix();
        glTranslatef(0, -0.5f, -1.5f);
        glRotatef(180, 0, 1, 0);
        drawTriangularPrism(0.5f, 1.0f, 0.5f);
        glPopMatrix();

        drawCuboidCannon(0.5f, 0.5f, 1.0f);

        glPushMatrix();
        glTranslatef(0, -0.5f, 1.0f);
        glRotatef(90, 1, 0, 0);
        drawTriangularPrism(0.5f, 1.0f, 1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, -1.0f, 0.25f);
        drawCuboidCannon(0.5f, 0.5f, 0.75f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, -1.0f, -0.5f);
        glRotatef(180, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        drawTriangularPrism(0.5f, 0.5f, 0.5f);
        glPopMatrix();

        glPopMatrix();
    }
}

static void drawCannonShell() {
    matCannonAccent();

    glPushMatrix();
    glRotatef(45, 0, 1, 0);
    glRotatef(-45, 0, 0, 1);
    glTranslatef(0, 3.0f, 1.5f);
    drawCuboidCannon(0.25f, 2.0f, 0.25f);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-45, 0, 1, 0);
    glRotatef(45, 0, 0, 1);
    glTranslatef(0, 3.0f, 1.5f);
    drawCuboidCannon(0.25f, 2.0f, 0.25f);
    glPopMatrix();
}

static void drawCannonDesignElements() {
    matCannonAccent();

    glPushMatrix();
    glTranslatef(0, -0.5f, -0.75f);
    glTranslatef(0, 2.9f, 7.0f);
    glRotatef(-3.576f, 1, 0, 0);

    glPushMatrix();
    glTranslatef(0, -0.5f, -3.5f);
    drawTriangularPrism(0.5f, 1.0f, 0.5f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -5.0f);
    drawCuboidCannon(0.5f, 0.5f, 1.0f);
    glPopMatrix();

    glPopMatrix();
}

static void drawCannonStand() {
    matCannonAccent();

    glPushMatrix();
    glTranslatef(-0.5f, -2.5f, 2.0f);
    glRotatef(90, 0, 1, 0);
    drawCylinderCannon(1.0f, 1.0f, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -6.0f, -1.5f);
    glRotatef(-45, 1, 0, 0);
    drawCuboidBasedZero(0.5f, 0.5f, 5.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5f, -6.0f, -1.5f);
    glRotatef(90, 0, 1, 0);
    drawCylinderCannon(1.0f, 1.0f, 1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -9.5f, 2.0f);
    glRotatef(-135, 1, 0, 0);
    drawCuboidBasedZero(0.5f, 0.5f, 5.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.5f, -9.5f, 2.0f);
    glRotatef(90, 0, 1, 0);
    drawCylinderCannon(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// ===== Barrel (textured outer, inner liner, bronze tip with fallback) =====
static void drawCannonBarrel() {
    matCannonMetal();

    glPushMatrix();
    glScalef(1.25f, 1.25f, 1.25f);

    // 1) Inner liner (slightly smaller/shorter so it never overdraws the outer)
    {
        const double innerLen = 7.6;
        drawCylinderCannon(1.80f, 2.30, innerLen);
    }

    // 2) Outer barrel (bamboo texture), with caps
    {
        const double outerLen = 8.0;

        GLboolean wasTex = glIsEnabled(GL_TEXTURE_2D);
        if (!wasTex) glEnable(GL_TEXTURE_2D);

        glColor3f(1, 1, 1); // do not tint texture
        if (gTex.bamboo) glBindTexture(GL_TEXTURE_2D, gTex.bamboo);
        else             glBindTexture(GL_TEXTURE_2D, 0);

        drawTexturedCylinderCaps(2.0f, 2.5, outerLen, 30);

        glBindTexture(GL_TEXTURE_2D, 0);
        if (!wasTex) glDisable(GL_TEXTURE_2D);
    }

    // Muzzle red accents
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    drawCannonMuzzleDesign();
    glPopMatrix();

    // 3) Bullet tip (prefer bronze texture; fallback to orange, culling-safe)
    {
        glPushMatrix();
        glTranslatef(0, 0, 7.5f);

        GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
        if (wasCull) glDisable(GL_CULL_FACE);

        GLboolean wasTex = glIsEnabled(GL_TEXTURE_2D);
        if (!wasTex) glEnable(GL_TEXTURE_2D);

        glColor3f(1, 1, 1);
        if (gTex.bronze) {
            glBindTexture(GL_TEXTURE_2D, gTex.bronze);
            glPushMatrix();
            glScalef(2.2f, 2.2f, 2.0f);  // ellipsoid tip (GLU sphere has UVs)
            drawSpherePrim(1.0f, 28, 22);
            glPopMatrix();
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            // No texture available: draw solid orange without GLU
            matCannonOrange();
            glPushMatrix();
            glScalef(2.2f, 2.2f, 2.0f);
            drawSphereWithoutGLU(1.0f, 1.0f, 1.0f, 1.0f);
            glPopMatrix();
        }

        if (!wasTex) glDisable(GL_TEXTURE_2D);
        if (wasCull) glEnable(GL_CULL_FACE);

        glPopMatrix();
    }

    // Side design + crossed shell
    drawCannonDesignElements();
    drawCannonShell();

    glPopMatrix();
}

// ===== Public drawing =====
void drawLaserCannonMounted() {
    if (!cannonState.visible) return;
    PolygonCounter::setCurrentPart(BodyPart::CANNON);
    glPushMatrix();
    drawCannonBarrel();             // no stand on shoulders
    glPopMatrix();
}

void drawLaserCannonStandalone() {
    if (!cannonState.visible) return;
    PolygonCounter::setCurrentPart(BodyPart::CANNON);
    glPushMatrix();
    drawCannonBarrel();
    drawCannonStand();              // with stand when placed on ground
    glPopMatrix();
}

// ===== Beam FX =====
void drawLaserBeam() {
    if (!cannonState.visible) return;
    if (!(cannonState.shootOn || cannonState.powerBall > 0.0f || cannonState.attack > 0.0f))
        return;

    matCannonAccent();
    glPushMatrix();

    if (cannonState.powerBall > 0.0f) {
        glPushMatrix();
        float s = cannonState.powerBall * 0.02f + 0.5f;
        glScalef(s, s, s);
        drawSphereWithoutGLU(1.0f, 1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }

    if (cannonState.attack > 0.0f && cannonState.attackRadius > 0.0f) {
        glPushMatrix();
        float L = cannonState.attack * 0.01f + 1.0f;
        glScalef(cannonState.attackRadius * 0.5f, cannonState.attackRadius * 0.5f, L);
        drawCylinderCannon(1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }

    if (cannonState.shootOn) {
        glPushMatrix();
        glScalef(2.0f, 2.0f, 2.0f);
        drawSphereWithoutGLU(1.0f, 1.0f, 1.0f, 1.0f);
        glPopMatrix();
    }

    glPopMatrix();
}

// ===== Animation & controls =====
void updateCannonAnimation() {
    if (!cannonState.weaponOn) {
        if (cannonState.canonRot <= 45.0f) { cannonState.canonRotSpeed = 5.0f;  cannonState.startFire = false; }
        else { cannonState.canonRotSpeed = 0.0f;  cannonState.startFire = true; }
    }
    else {
        if (cannonState.canonRot > 0.0f) { cannonState.canonRotSpeed = -5.0f; cannonState.startFire = false; }
        else { cannonState.canonRotSpeed = 0.0f;  cannonState.startFire = true; }
    }

    cannonState.canonRot += cannonState.canonRotSpeed;
    if (cannonState.canonRot < 0.0f)  cannonState.canonRot = 0.0f;
    if (cannonState.canonRot > 45.0f) cannonState.canonRot = 45.0f;
}

void updateShootingAnimation() {
    if (cannonState.shootOn) {
        if (cannonState.powerBall <= 30.0f) {
            cannonState.powerBallSize = 5.0f;
            cannonState.endShoot = false;
        }
        else {
            cannonState.powerBallSize = 0.0f;
            if (cannonState.attack <= 1000.0f) {
                cannonState.attackLength = 100.0f;
                if (cannonState.attackRadius <= 2.0f) {
                    cannonState.attackRadius += 0.5f;
                }
                cannonState.endShoot = false;
            }
            else {
                cannonState.attackLength = 0.0f;
                cannonState.endShoot = true;
                cannonState.shootOn = false;
            }
        }
    }
    else {
        cannonState.powerBallSize = (cannonState.powerBall > 0.0f) ? -5.0f : 0.0f;
        cannonState.attackLength = (cannonState.attack > 0.0f) ? -50.0f : 0.0f;
        cannonState.attackRadius = (cannonState.attackRadius > 0.0f) ? (cannonState.attackRadius - 0.5f) : 0.0f;
    }

    // Integrate
    cannonState.attack += cannonState.attackLength;
    cannonState.powerBall += cannonState.powerBallSize;

    // Clamp
    if (cannonState.attack < 0.0f)       cannonState.attack = 0.0f;
    if (cannonState.powerBall < 0.0f)    cannonState.powerBall = 0.0f;
    if (cannonState.attackRadius < 0.0f) cannonState.attackRadius = 0.0f;
}

void toggleCannon() {
    cannonState.weaponOn = !cannonState.weaponOn;
    std::printf("Cannon toggled: %s\n", cannonState.weaponOn ? "ON" : "OFF");
}

void fireCannon() {
    std::printf("Fire button pressed! weaponOn:%d startFire:%d canonRot:%.1f\n",
        cannonState.weaponOn, cannonState.startFire, cannonState.canonRot);

    if (cannonState.startFire && cannonState.weaponOn) {
        cannonState.shootOn = true;
        std::printf("Cannon fired! shootOn:%d powerBall:%.1f attack:%.1f\n",
            cannonState.shootOn, cannonState.powerBall, cannonState.attack);
    }
    else {
        cannonState.shootOn = true; // dev fire
        std::printf("Force firing for debug - shootOn:%d\n", cannonState.shootOn);
    }
}

void toggleCannonVisibility() {
    cannonState.visible = !cannonState.visible;
    std::printf("Cannon visibility toggled: %s\n", cannonState.visible ? "ON" : "OFF");
}
