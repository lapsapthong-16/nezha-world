#include "arms.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawArmChain(bool left) {
    const float side = left ? -1.f : 1.f;

    // Calculate shoulder position
    const float yShoulder = MS.torsoH * 0.5f - 0.10f;
    const float xShoulder = side * (MS.torsoTopR * 0.92f + 0.03f);
    const float zShoulder = 0.06f;

    glPushMatrix();
    glTranslatef(xShoulder, yShoulder, zShoulder);

    // Arm positioning
    glRotatef(side * 8.0f, 0, 0, 1);
    glRotatef(6.0f, 1, 0, 0);

    // Shoulder joint
    const float shoulderR = 0.23f;
    matBlackFur();
    drawSpherePrim(shoulderR);

    // Upper arm
    const float upperArmLength = MS.upperArmH + shoulderR + MS.jointR;
    glPushMatrix();
    glTranslatef(0.0f, -shoulderR - upperArmLength * 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.upperArmR, upperArmLength, 32);
    glPopMatrix();

    // Elbow joint
    const float elbowY = -shoulderR - MS.upperArmH;
    glPushMatrix();
    glTranslatef(0.0f, elbowY, 0.0f);
    drawSpherePrim(MS.jointR);
    glPopMatrix();

    // Forearm
    const float forearmLength = MS.lowerArmH + MS.jointR + MS.jointR * 0.9f;
    glPushMatrix();
    glTranslatef(0.0f, elbowY - forearmLength * 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.lowerArmR, forearmLength, 32);
    glPopMatrix();

    // Wrist joint
    const float wristY = elbowY - MS.lowerArmH;
    glPushMatrix();
    glTranslatef(0.0f, wristY, 0.0f);
    drawSpherePrim(MS.jointR * 0.90f);
    glPopMatrix();

    // Hand
    matBlackFur();
    glPushMatrix();
    glTranslatef(0.0f, wristY - MS.jointR * 0.90f - 0.1f, 0.0f);
    glScalef(0.85f, 0.75f, 0.85f);
    drawSpherePrim(0.25f, 28, 18);
    glPopMatrix();

    glPopMatrix();
}

