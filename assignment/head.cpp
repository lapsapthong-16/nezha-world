#include "head.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// Material definitions for head components
static void matWhite() {
    const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1 };
    const GLfloat diff[] = { 1, 1, 1, 1 };
    const GLfloat spec[] = { 0.3f, 0.3f, 0.3f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24);
}

static void matBlack() {
    const GLfloat amb[] = { 0.05f, 0.05f, 0.05f, 1 };
    const GLfloat diff[] = { 0.08f, 0.08f, 0.08f, 1 };
    const GLfloat spec[] = { 0.05f, 0.05f, 0.05f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8);
}

static void matBrown() {
    const GLfloat amb[] = { 0.12f, 0.08f, 0.06f, 1 };
    const GLfloat diff[] = { 0.20f, 0.14f, 0.10f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 4);
}

static void matIrisPastel() {
    const GLfloat amb[] = { 0.20f, 0.25f, 0.35f, 1 };
    const GLfloat diff[] = { 0.45f, 0.55f, 0.80f, 1 };
    const GLfloat spec[] = { 0.15f, 0.15f, 0.20f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16);
}

static void matPureWhite() {
    const GLfloat amb[] = { 0.50f, 0.50f, 0.50f, 1 };
    const GLfloat diff[] = { 1, 1, 1, 1 };
    const GLfloat spec[] = { 0.40f, 0.40f, 0.40f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32);
}

// Create tapered cylinder for lower head
static void drawTaperCupY(float rBottom, float rTop, float h, int slices = 36) {
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBottom, rTop, h, slices, 1);
    gluDisk(q, 0.0f, rBottom, slices, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

static inline float zSurf(float r) { return r + 0.010f; }

static void drawNeckStump(float R) {
    matWhite();
    glPushMatrix();
    glTranslatef(0.0f, -0.40f * R, 0.0f);
    glScalef(0.55f * R, 0.30f * R, 0.55f * R);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Black oval patches behind eyes
static void drawPatchOval(float R, float x, float y, float z) {
    matBlack();
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.22f * R, 0.28f * R, 0.070f * R);
    glutSolidSphere(1.0f, 24, 18);
    glPopMatrix();
}

// Large anime-style eyes with highlights
static void drawCuteEye(float R, float x, float y, float zPatchCenter, float lookX = 0.0f, float lookY = 0.0f) {
    const float zEye = zPatchCenter + 0.075f * R;
    const float offX = lookX * 0.020f * R;
    const float offY = lookY * 0.020f * R;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // White sclera
    matPureWhite();
    glPushMatrix();
    glTranslatef(x - 0.010f * R + offX, y + 0.020f * R + offY, zEye);
    glScalef(0.165f * R, 0.165f * R, 0.060f * R);
    glutSolidSphere(1.0f, 28, 22);
    glPopMatrix();

    // Blue iris
    matIrisPastel();
    glPushMatrix();
    glTranslatef(x + offX, y + 0.012f * R + offY, zEye + 0.012f * R);
    glScalef(0.125f * R, 0.125f * R, 0.046f * R);
    glutSolidSphere(1.0f, 28, 22);
    glPopMatrix();

    // Black pupil
    matBlack();
    glPushMatrix();
    glTranslatef(x + 0.004f * R + offX, y + 0.014f * R + offY, zEye + 0.020f * R);
    glScalef(0.070f * R, 0.070f * R, 0.028f * R);
    glutSolidSphere(1.0f, 24, 18);
    glPopMatrix();

    // Eye highlights
    matPureWhite();
    glPushMatrix();
    glTranslatef(x - 0.030f * R + offX, y + 0.040f * R + offY, zEye + 0.026f * R);
    glScalef(0.024f * R, 0.024f * R, 0.016f * R);
    glutSolidSphere(1.0f, 18, 14);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(x + 0.020f * R + offX, y - 0.014f * R + offY, zEye + 0.022f * R);
    glScalef(0.010f * R, 0.010f * R, 0.008f * R);
    glutSolidSphere(1.0f, 14, 10);
    glPopMatrix();

    glPopAttrib();
}

// High-positioned panda ears
static void drawEarPairHigh(float R) {
    matBlack();
    const float earR = 0.34f * R;
    const float earX = 0.92f * R;
    const float earY = 0.8f * R;
    const float earZ = -0.05f * R;
    const float sx = 1.00f, sy = 0.86f, sz = 0.94f;

    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * earX, earY, earZ);
        glScalef(sx, sy, sz);
        drawSpherePrim(earR, 18, 12);
        glPopMatrix();
    }
}

// Nose and mouth features
static void drawNoseMouth(float R, float zSurface) {
    const float zN = zSurface + 0.098f * R;
    const float zM = zSurface + 0.092f * R;

    // Black nose
    matBlack();
    glPushMatrix();
    glTranslatef(0.0f, -0.116f * R, zN);
    glScalef(0.060f * R, 0.040f * R, 0.032f * R);
    glutSolidSphere(1.0f, 12, 10);
    glPopMatrix();

    // Brown mouth
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    matBrown();
    glPushMatrix();
    glTranslatef(0.0f, -0.182f * R, zM);
    glScalef(0.088f * R, 0.022f * R, 0.014f * R);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPopAttrib();
}

// Main head assembly function
void drawHeadUnit() {
    glPushMatrix();

    const float R = 0.66f;
    const float z = zSurf(R);

    // Lower head (neck connection)
    matWhite();
    glPushMatrix();
    glTranslatef(0.0f, -0.25f * R, 0.0f);
    drawTaperCupY(0.78f * R, 0.95f * R, 0.55f * R, 40);
    glPopMatrix();

    // Main head dome
    glPushMatrix();
    glTranslatef(0.0f, 0.16f * R, 0.0f);
    glScalef(1.05f, 0.95f, 1.05f);
    drawSpherePrim(R, 32, 24);
    glPopMatrix();

    // Panda ears
    drawEarPairHigh(R);

    // Eye patches and eyes
    const float zFace = zSurf(R);
    const float ex = 0.205f * R;
    const float ey = 0.065f * R;

    drawPatchOval(R, -ex, ey, zFace);
    drawPatchOval(R, ex, ey, zFace);
    drawCuteEye(R, -ex, ey, zFace, 0.7f, 0.15f);
    drawCuteEye(R, ex, ey, zFace, -0.7f, 0.15f);

    // Nose and mouth
    drawNoseMouth(R, z);

    // Neck scarf ring
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, -0.36f * R, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.030f, 0.34f, 16, 36);
    glPopMatrix();

    // Neck connection
    drawNeckStump(R);

    glPopMatrix();
}
