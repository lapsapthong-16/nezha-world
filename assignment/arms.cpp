#include "arms.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawArmChain(bool left) {
    const float side = left ? -1.f : 1.f;

    // --- Shoulder anchor computed from the torso measurements ---
    // Put the joint a little below the yoke, just outside the vest shell,
    // and a touch forward so it doesn't clip the chest panel.
    const float yShoulder = MS.torsoH * 0.5f - 0.10f;      // 10 cm under top
    const float xShoulder = side * (MS.torsoTopR * 0.92f + 0.03f); // just outside shell
    const float zShoulder = 0.06f;                          // slight forward offset

    glPushMatrix();
    glTranslatef(xShoulder, yShoulder, zShoulder);

    // A tiny outward splay so the upper arm clears the torso nicely
    glRotatef(side * 8.0f, 0, 0, 1);    // yaw around Z, ±8°
    // Small forward tilt so the forearm hangs in front of the body a bit
    glRotatef(6.0f, 1, 0, 0);

    // --- Shoulder ball (skin) ---
    const float shoulderR = 0.23f;
    matSkin();
    drawSpherePrim(shoulderR);

    // --- Upper arm (capped cylinder, pointing down) ---
    glTranslatef(0.0f, -MS.upperArmH * 0.5f - shoulderR * 0.20f, 0.0f); // start just below ball
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);            // make cylinder Y-down
    drawCappedCylinder(MS.upperArmR, MS.upperArmH, 32);
    glPopMatrix();

    // --- Elbow joint ---
    glTranslatef(0.0f, -MS.upperArmH * 0.5f - 0.01f, 0.0f);
    drawSpherePrim(MS.jointR);

    // --- Forearm ---
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f - MS.jointR * 0.20f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.lowerArmR, MS.lowerArmH, 32);
    glPopMatrix();

    // --- Wrist + simple fist ---
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f - 0.01f, 0.0f);
    drawSpherePrim(MS.jointR * 0.90f);

    matSkin();
    glPushMatrix();
    glScalef(1.05f, 0.90f, 1.05f);
    drawSpherePrim(0.25f, 28, 18);      // fist
    glPopMatrix();

    glPopMatrix(); // end shoulder anchor
}

