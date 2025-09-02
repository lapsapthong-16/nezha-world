#include "legs.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

namespace {
    // Draw foot at ankle position
    void drawFootAt(float x, float ankleY, float z) {
        matBlackFur();

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
    }
}

void drawLeg(bool left) {
    const float side = left ? -1.f : 1.f;
    const float x = side * MS.hipX;
    const float hipY = -0.98f;

    // Upper leg (thigh)
    matBlackFur();
    glPushMatrix();
    glTranslatef(x, hipY - MS.upperLegH, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.23f, MS.upperLegH, 28);
    glPopMatrix();

    const float kneeY = hipY - MS.upperLegH;

    // Knee joint
    glPushMatrix();
    glTranslatef(x, kneeY, 0.0f);
    drawSpherePrim(0.17f);
    glPopMatrix();

    // Lower leg (shin)
    glPushMatrix();
    glTranslatef(x, kneeY - MS.lowerLegH, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.21f, MS.lowerLegH, 28);
    glPopMatrix();

    const float ankleY = kneeY - MS.lowerLegH;

    // Foot
    drawFootAt(x, ankleY, 0.02f);
}