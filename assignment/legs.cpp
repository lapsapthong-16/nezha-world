#include "legs.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>
namespace {
    // Foot built relative to the ANKLE position (ankleY = top of foot / end of shin)
    void drawFootAt(float x, float ankleY, float z) {
        matSkin();

        // --- ankle sleeve (overlaps shin a bit so the joint is seamless) ---
        glPushMatrix();
        glTranslatef(x, ankleY, z);
        glRotatef(-90, 1, 0, 0);              // GLU cylinder points +Z; rotate to +Y
        drawCappedCylinder(0.21f, 0.14f, 24);  // same radius as lower-leg (0.21)
        glPopMatrix();

        // --- heel ---
        glPushMatrix();
        glTranslatef(x, ankleY - 0.10f, z - 0.03f);
        glScalef(0.18f, 0.12f, 0.20f);
        drawSpherePrim(1.0f, 24, 16);
        glPopMatrix();

        // --- forefoot / toes ---
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
    const float hipY = -0.98f;           // where the thigh attaches to the shorts/hip

    // ===== upper leg: from hip DOWN to knee =====
    matSkin();
    glPushMatrix();
    glTranslatef(x, hipY - MS.upperLegH, 0.0f);     // place base so top ends at hipY
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.23f, MS.upperLegH, 28);    // grows upward to hipY
    glPopMatrix();

    const float kneeY = hipY - MS.upperLegH;        // top of shin / bottom of thigh

    // knee
    glPushMatrix();
    glTranslatef(x, kneeY, 0.0f);
    drawSpherePrim(0.17f);
    glPopMatrix();

    // ===== lower leg (shin): from knee DOWN to ankle =====
    glPushMatrix();
    glTranslatef(x, kneeY - MS.lowerLegH, 0.0f);    // base at ANKLE; top will be kneeY
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.21f, MS.lowerLegH, 28);    // grows upward to kneeY
    glPopMatrix();

    const float ankleY = kneeY - MS.lowerLegH;

    // ===== foot (sleeve overlaps shin) =====
    drawFootAt(x, ankleY, 0.02f);
}