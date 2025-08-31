#include "arms.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawArmChain(bool left) {
    const float side = left ? -1.f : 1.f;

    // Shoulder anchor: sink slightly into torso and a hair backward
    glTranslatef(side * MS.shoulderX, MS.shoulderY, -0.08f);

    // Shoulder ball (skin) — no sleeve ring
    matSkin();
    drawSpherePrim(0.23f);

    // Upper arm
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.upperArmR, MS.upperArmH, 32);
    glPopMatrix();

    // Elbow joint
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    drawSpherePrim(MS.jointR);

    // Forearm
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.lowerArmR, MS.lowerArmH, 32);
    glPopMatrix();

    // Wrist + simple fist
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    drawSpherePrim(MS.jointR * 0.90f);

    matSkin();
    glPushMatrix();
    glScalef(1.05f, 0.90f, 1.05f);
    drawSpherePrim(0.25f, 28, 18);
    glPopMatrix();
}
