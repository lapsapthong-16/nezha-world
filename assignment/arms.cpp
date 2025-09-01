#include "arms.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawArmChain(bool left) {
    const float side = left ? -1.f : 1.f;

    // --- Shoulder anchor computed from the torso measurements ---
    const float yShoulder = MS.torsoH * 0.5f - 0.10f;      // 10 cm under top
    const float xShoulder = side * (MS.torsoTopR * 0.92f + 0.03f); // just outside shell
    const float zShoulder = 0.06f;                          // slight forward offset

    glPushMatrix();
    glTranslatef(xShoulder, yShoulder, zShoulder);

    // A tiny outward splay so the upper arm clears the torso nicely
    glRotatef(side * 8.0f, 0, 0, 1);    // yaw around Z, ±8°
    // Small forward tilt so the forearm hangs in front of the body a bit
    glRotatef(6.0f, 1, 0, 0);

    // --- Shoulder ball (black fur like real panda) ---
    const float shoulderR = 0.23f;
    matBlackFur();
    drawSpherePrim(shoulderR);

    // --- Upper arm: make it LONGER so it goes INTO the shoulder and INTO the elbow ---
    const float upperArmLength = MS.upperArmH + shoulderR + MS.jointR;  // Extended length
    glPushMatrix();
    glTranslatef(0.0f, -shoulderR - upperArmLength * 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);            
    drawCappedCylinder(MS.upperArmR, upperArmLength, 32);  // LONGER cylinder
    glPopMatrix();

    // --- Elbow joint: positioned where upper arm ends ---
    const float elbowY = -shoulderR - MS.upperArmH;
    glPushMatrix();
    glTranslatef(0.0f, elbowY, 0.0f);
    drawSpherePrim(MS.jointR);
    glPopMatrix();

    // --- Forearm: make it LONGER so it goes INTO the elbow and INTO the wrist ---
    const float forearmLength = MS.lowerArmH + MS.jointR + MS.jointR * 0.9f;  // Extended length
    glPushMatrix();
    glTranslatef(0.0f, elbowY - forearmLength * 0.5f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.lowerArmR, forearmLength, 32);  // LONGER cylinder
    glPopMatrix();

    // --- Wrist joint ---
    const float wristY = elbowY - MS.lowerArmH;
    glPushMatrix();
    glTranslatef(0.0f, wristY, 0.0f);
    drawSpherePrim(MS.jointR * 0.90f);
    glPopMatrix();

    // --- Hand/fist (BLACK like real panda paws) ---
    matBlackFur();  // Changed to black like real panda paws!
    glPushMatrix();
    glTranslatef(0.0f, wristY - MS.jointR * 0.90f - 0.1f, 0.0f);
    glScalef(1.05f, 0.90f, 1.05f);
    drawSpherePrim(0.25f, 28, 18);      // fist
    glPopMatrix();

    glPopMatrix(); // end shoulder anchor
}

