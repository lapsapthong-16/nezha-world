#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// ---------------- helpers ----------------
// Solid inner body so the vest opening shows SKIN, not the floor
static void drawTorsoCore(bool asSkin = true) {
    if (asSkin) matSkin(); else matShirt();

    // Slightly slimmer than the vest so it never pokes through
    const float rBot = MS.torsoBotR * 0.88f;
    const float rTop = MS.torsoTopR * 0.86f;

    // Height tucks under the yoke and down into the waist a touch
    const float h = MS.torsoH - 0.02f;

    // GLU cylinders are along +Z; rotate to stand up on Y and center on origin
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, -h * 0.5f);

    // To be safe against culling on caps, temporarily disable cull
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBot, rTop, h, 44, 1);               // wall
    gluDisk(q, 0.0f, rBot, 44, 1);                      // bottom cap
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 44, 1);                      // top cap
    glPopMatrix();

    gluDeleteQuadric(q);
    if (wasCull) glEnable(GL_CULL_FACE);
    glPopMatrix();
}

// a very thin annulus to close the vest at the shoulders (so no hole)
static void drawVestTopYoke() {
    matVest();
    const float yTop = MS.torsoH * 0.5f;
    const float rOuter = MS.torsoTopR + 0.001f;
    const float rInner = 0.34f; // neck hole placeholder

    glPushMatrix();
    glTranslatef(0.0f, yTop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, rInner, rOuter, 64, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// gold edge pipe placed exactly on the two open rim angles
static void drawEdgePipeAt(float deg) {
    const float a = deg2rad(deg);
    const float rAvg = 0.5f * (MS.torsoTopR + MS.torsoBotR) + 0.004f;
    const float x = rAvg * cosf(a);
    const float z = rAvg * sinf(a);

    matRope();
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-90, 1, 0, 0);                   // cylinder axis -> Y
    drawCappedCylinder(0.018f, MS.torsoH * 0.78f, 28);
    glPopMatrix();
}

// ---------------- public ----------------
void drawTorso() {
    // Open vest shell (back + sides) with a clear front gap for the chest
    constexpr float VEST_GAP_DEG = 95.0f;            // tweak if you want wider/narrower
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float startDeg = edgeL;
    const float sweepDeg = 360.0f - VEST_GAP_DEG;

    // Vest outer shell
    matVest();
    glPushMatrix();
    drawOpenCylinderY(MS.torsoBotR, MS.torsoTopR, MS.torsoH, startDeg, sweepDeg, 72);
    glPopMatrix();

    // Solid inner body so it’s not see-through
    drawTorsoCore(/*asSkin=*/true);            // swap to false to make it a gray shirt

    // Close the top so you can’t see down inside
    drawVestTopYoke();

    // Gold piping at the two open edges
    drawEdgePipeAt(90.0f + VEST_GAP_DEG * 0.5f);   // left edge
    drawEdgePipeAt(90.0f - VEST_GAP_DEG * 0.5f);   // right edge

    // Waist flap
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.72f, 0.0f);
    glScalef(1.28f, 0.26f, 0.64f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawHipWrap() {
    // Solid wrap under the vest so the belt area is never see-through
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
