#include "head.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// Helper material functions for eyes
static void matWhite() {
    const GLfloat diff[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat amb[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat spec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}

static void matBrown() {
    const GLfloat diff[] = { 0.3f, 0.2f, 0.1f, 1.0f };
    const GLfloat amb[] = { 0.1f, 0.05f, 0.03f, 1.0f };
    const GLfloat spec[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0f);
}

// Fixed eye function with proper materials and bigger size
static void drawEye(float x, float y, float z, float scale = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);
    
    // Eye white - made bigger
    matWhite();
    glPushMatrix();
    glScalef(0.08f, 0.06f, 0.03f); // Bigger and more visible
    glutSolidSphere(1.0f, 16, 12);
    glPopMatrix();
    
    // Eye iris - made bigger
    matBrown();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.02f);
    glScalef(0.05f, 0.05f, 0.02f);
    glutSolidSphere(1.0f, 12, 8);
    glPopMatrix();
    
    // Eye pupil - made bigger
    matHair();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.025f);
    glScalef(0.02f, 0.02f, 0.01f);
    glutSolidSphere(1.0f, 8, 6);
    glPopMatrix();
    
    glPopMatrix();
}

// Fixed eyebrow function - made bigger
static void drawEyebrow(float x, float y, float z, float angle, float scale = 1.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angle, 0, 0, 1);
    glScalef(scale, scale, scale);
    
    matHair();
    glPushMatrix();
    glScalef(0.10f, 0.02f, 0.015f); // Made bigger and more visible
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Fixed hair horn function
static void drawHairHorn(float x, float y, float z, float rotY = 0.0f) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);
    
    matHair();
    
    // Main horn body
    glPushMatrix();
    glRotatef(-15, 1, 0, 0);
    glScalef(0.12f, 0.25f, 0.10f);
    glutSolidSphere(1.0f, 16, 12);
    glPopMatrix();
    
    // Tip of horn
    glPushMatrix();
    glTranslatef(0.0f, 0.18f, -0.04f);
    glRotatef(-20, 1, 0, 0);
    glScalef(0.06f, 0.12f, 0.06f);
    glutSolidSphere(1.0f, 12, 8);
    glPopMatrix();
    
    glPopMatrix();
}

// Fixed forehead marking - made bigger
static void drawForeheadMark() {
    glPushMatrix();
    glTranslatef(0.0f, 0.12f, 0.32f); // Adjusted position
    
    matScarf();
    glPushMatrix();
    glScalef(0.05f, 0.05f, 0.01f); // Made bigger
    glutSolidSphere(1.0f, 16, 8);
    glPopMatrix();
    
    glPopMatrix();
}

// Fixed mouth - made bigger
static void drawMouth() {
    glPushMatrix();
    glTranslatef(0.0f, -0.06f, 0.31f); // Adjusted position
    
    matHair();
    glPushMatrix();
    glScalef(0.035f, 0.012f, 0.008f); // Made bigger
    glutSolidCube(1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Fixed hair base
static void drawHairBase(float headRadius) {
    matHair();
    
    // Main hair volume
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, -0.08f);
    glScalef(0.85f, 0.6f, 0.7f);
    glutSolidSphere(headRadius, 20, 16);
    glPopMatrix();
    
    // Hair bangs
    glPushMatrix();
    glTranslatef(0.0f, 0.20f, 0.15f);
    glScalef(0.4f, 0.15f, 0.2f);
    glutSolidSphere(1.0f, 16, 12);
    glPopMatrix();
    
    // Side hair pieces
    glPushMatrix();
    glTranslatef(-0.25f, 0.0f, 0.08f);
    glScalef(0.18f, 0.3f, 0.18f);
    glutSolidSphere(1.0f, 12, 10);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.25f, 0.0f, 0.08f);
    glScalef(0.18f, 0.3f, 0.18f);
    glutSolidSphere(1.0f, 12, 10);
    glPopMatrix();
}

void drawHeadUnit() {
    glPushMatrix();
    
    // Use a better sized head - bigger than 0.45f but smaller than 0.95f
    const float headRadius = 0.65f; // Good balance
    
    // Main head sphere (skin)
    matSkin();
    drawSpherePrim(headRadius, 24, 18);
    
    // Draw hair base
    drawHairBase(headRadius);
    
    // Draw hair horns (properly scaled)
    drawHairHorn(-0.18f, 0.25f, 0.1f, -15.0f);  // Left horn
    drawHairHorn(0.18f, 0.25f, 0.1f, 15.0f);    // Right horn
    
    // Draw facial features (bigger and more visible)
    drawEye(-0.12f, 0.05f, 0.28f);  // Left eye
    drawEye(0.12f, 0.05f, 0.28f);   // Right eye
    
    // Draw eyebrows
    drawEyebrow(-0.12f, 0.15f, 0.29f, 15.0f);   // Left eyebrow
    drawEyebrow(0.12f, 0.15f, 0.29f, -15.0f);   // Right eyebrow
    
    // Draw forehead marking
    drawForeheadMark();
    
    // Draw mouth
    drawMouth();
    
    glPopMatrix();
}
