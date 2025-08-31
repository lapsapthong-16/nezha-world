#include "head.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// All helpers here are private to this translation unit
namespace {
    // optionally tweak hair shape here
    constexpr float kHairLift = 0.84f;          // vertical placement of hair cap center
    constexpr float kHairScaleY = 0.90f;          // vertical scale (slightly squashed)
    constexpr float kHairGrow = 0.05f;          // hair radius minus head radius
}

void drawHeadUnit() {
    // --- neck (short) ---
    matSkin();
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.22f, 0.24f, 24);
    glPopMatrix();

    // --- scarf ring ---
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.08f, 0.5f, 20, 40);
    glPopMatrix();

    // --- head (slightly squashed) ---
    matSkin();
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, 0.0f);
    glScalef(1.0f, 0.92f, 1.0f);
    drawSpherePrim(MS.headR, 36, 24);
    glPopMatrix();

    // --- hair cap (covers skull apex; darker, low spec) ---
    matHair();
    glPushMatrix();
    glTranslatef(0.0f, kHairLift, 0.0f);
    glScalef(1.08f, kHairScaleY, 1.08f);
    drawSpherePrim(MS.headR + kHairGrow, 36, 24);
    glPopMatrix();

    // --- thin hairline rim (to separate hair/skin) ---
    matHairRim();
    glPushMatrix();
    glTranslatef(0.0f, 0.64f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.01f, MS.headR * 0.94f, 16, 48);
    glPopMatrix();

    // --- hair buns ---
    matHair();
    glPushMatrix(); glTranslatef(-0.66f, 1.06f, -0.02f); drawSpherePrim(0.24f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.66f, 1.06f, -0.02f); drawSpherePrim(0.24f); glPopMatrix();

    // --- ears ---
    matSkin();
    glPushMatrix(); glTranslatef(-0.83f, 0.60f, 0.00f); drawSpherePrim(0.18f, 24, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.83f, 0.60f, 0.00f); drawSpherePrim(0.18f, 24, 16); glPopMatrix();
}
