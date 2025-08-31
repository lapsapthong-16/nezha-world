#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// ======== internal helpers for torso ========

// undershirt (cylinder inside the vest)
static void drawInnerTorsoShell() {
    matShirt();

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    const float down = 0.55f;
    glTranslatef(0, 0, -down);

    const float hTopSafe = MS.torsoH - 0.02f;
    const float h = hTopSafe + down;

    const float rTop = MS.torsoTopR * 0.88f;
    const float rBot = MS.torsoBotR * 0.88f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBot, rTop, h, 36, 1);
    gluDisk(q, 0.0f, rBot, 36, 1);
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0.0f, rTop, 36, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();
}

// curved shirt plate + emblem
static void drawShirtFrontWithEmblem() {
    // curved shirt plate
    matShirt();
    glPushMatrix();
    glTranslatef(0.0f, 0.00f, 0.26f);
    glRotatef(90, 1, 0, 0);
    const float R = 0.43f, TH = 0.04f;
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, R, R, TH, 40, 1);
    gluDisk(q, 0.0, R, 40, 1);
    glPushMatrix(); glTranslatef(0, 0, TH); gluDisk(q, 0.0, R, 40, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();

    // emblem disk (use palette wrapper instead of raw setMaterial)
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, 0.07f, 0.33f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluQuadricNormals(q2, GLU_SMOOTH);
    gluDisk(q2, 0.0f, 0.14f, 36, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();
}

// vest panel (flat piece in front)
static void drawVestPanel(bool left) {
    const float side = left ? -1.f : 1.f;

    // body panel
    matVest();
    glPushMatrix();
    glTranslatef(side * 0.34f, 0.00f, 0.325f);
    glRotatef(side * 8.0f, 0, 1, 0);
    glScalef(0.35f, MS.torsoH * 0.80f, 0.06f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // outer trim
    matRope();
    glPushMatrix();
    glTranslatef(side * 0.50f, 0.00f, 0.325f);
    glRotatef(side * 8.0f, 0, 1, 0);
    glScalef(0.04f, MS.torsoH * 0.86f, 0.065f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // inner trim
    matRope();
    glPushMatrix();
    glTranslatef(side * 0.19f, 0.00f, 0.325f);
    glRotatef(side * 8.0f, 0, 1, 0);
    glScalef(0.03f, MS.torsoH * 0.78f, 0.065f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// thin annulus that closes vest top
static void drawVestTopYoke() {
    matVest();
    const float yTop = MS.torsoH * 0.5f;
    const float rOuter = MS.torsoTopR + 0.001f;
    const float rInner = 0.34f;

    glPushMatrix();
    glTranslatef(0.0f, yTop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, rInner, rOuter, 64, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// ======== public functions ========

void drawTorso() {
    // open shell with front gap
    constexpr float VEST_GAP_DEG = 100.0f;
    const float gapDeg = VEST_GAP_DEG;
    const float edgeL = 90.0f + gapDeg * 0.5f;
    const float startDeg = edgeL;
    const float sweepDeg = 360.0f - gapDeg;

    matVest();
    glPushMatrix();
    drawOpenCylinderY(MS.torsoBotR, MS.torsoTopR, MS.torsoH, startDeg, sweepDeg, 72);
    glPopMatrix();

    drawVestTopYoke();
    drawShirtFrontWithEmblem();
    drawVestPanel(true);
    drawVestPanel(false);

    // waist flap
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.72f, 0.0f);
    glScalef(1.28f, 0.26f, 0.64f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawHipWrap() {
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.82f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float h = 0.48f;
    const float r = MS.torsoBotR * 0.99f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, 44, 1);
    gluDisk(q, 0.0f, r, 44, 1);
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0.0f, r, 44, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}
