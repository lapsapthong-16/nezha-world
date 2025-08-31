#include "legs.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

namespace {
    // internal helper: simple bare foot
    void drawFoot(float sideX, float yBottom, float zOffset) {
        matSkin(); // Use skin material for bare feet
        
        // Main foot body (elongated sphere)
        glPushMatrix();
        glTranslatef(sideX, yBottom + 0.08f, zOffset + 0.12f);
        glScalef(0.18f, 0.12f, 0.32f);
        drawSpherePrim(1.0f, 24, 16);
        glPopMatrix();
        
        // Heel (smaller sphere)
        glPushMatrix();
        glTranslatef(sideX, yBottom + 0.06f, zOffset - 0.08f);
        glScalef(0.16f, 0.10f, 0.18f);
        drawSpherePrim(1.0f, 20, 14);
        glPopMatrix();
        
        // Ankle connection (small cylinder)
        glPushMatrix();
        glTranslatef(sideX, yBottom + 0.12f, zOffset);
        glRotatef(-90, 1, 0, 0);
        drawCappedCylinder(0.18f, 0.08f, 20);
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

    // bare foot (replacing boot)
    drawFoot(side * MS.hipX,
        -0.98f - MS.upperLegH - MS.lowerLegH - 0.06f,  // Raised slightly
        0.0f);
}
