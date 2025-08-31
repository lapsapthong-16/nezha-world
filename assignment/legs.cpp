#include "legs.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

namespace {
    // internal helper: rounded boot (heel + toe)
    void drawBoot(float sideX, float yBottom, float zOffset) {
        matBoot();
        // heel (rounded)
        glPushMatrix();
        glTranslatef(sideX, yBottom + 0.10f, zOffset - 0.02f);
        glScalef(0.42f, 0.22f, 0.46f);
        drawSpherePrim(0.55f, 28, 20);
        glPopMatrix();
        // toe (capsule-ish)
        glPushMatrix();
        glTranslatef(sideX, yBottom + 0.10f, zOffset + 0.22f);
        glRotatef(-90, 1, 0, 0);
        drawCappedCylinder(0.22f, 0.40f, 28);
        glPopMatrix();
    }
}

void drawLeg(bool left) {
    const float side = left ? -1.f : 1.f;
    matSkin();

    // upper leg
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.23f, MS.upperLegH, 28);
    glPopMatrix();

    // knee
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f - MS.upperLegH, 0.0f);
    drawSpherePrim(0.17f);
    glPopMatrix();

    // lower leg
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f - MS.upperLegH, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.21f, MS.lowerLegH, 28);
    glPopMatrix();

    // boot
    drawBoot(side * MS.hipX,
        -0.98f - MS.upperLegH - MS.lowerLegH - 0.12f,
        0.10f);
}
