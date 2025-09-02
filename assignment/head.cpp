#include "head.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// --- simple materials ---
static void matWhite() {
    const GLfloat amb[] = { 0.35f,0.35f,0.35f,1 };
    const GLfloat diff[] = { 1,1,1,1 };
    const GLfloat spec[] = { 0.3f,0.3f,0.3f,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24);
}
static void matBlack() {
    const GLfloat amb[] = { 0.05f,0.05f,0.05f,1 };
    const GLfloat diff[] = { 0.08f,0.08f,0.08f,1 };
    const GLfloat spec[] = { 0.05f,0.05f,0.05f,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8);
}
static void matBrown() { // soft dark for pupils/mouth
    const GLfloat amb[] = { 0.12f,0.08f,0.06f,1 };
    const GLfloat diff[] = { 0.20f,0.14f,0.10f,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 4);
}

static void matIrisPastel() {                 // light blue/gray iris
    const GLfloat amb[] = { 0.20f,0.25f,0.35f,1 };
    const GLfloat diff[] = { 0.45f,0.55f,0.80f,1 };
    const GLfloat spec[] = { 0.15f,0.15f,0.20f,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16);
}
static void matPureWhite() {
    const GLfloat amb[] = { 0.50f,0.50f,0.50f,1 };
    const GLfloat diff[] = { 1,1,1,1 };
    const GLfloat spec[] = { 0.40f,0.40f,0.40f,1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32);
}

// --- add this helper near the top of head.cpp (after includes/materials) ---
static void drawTaperCupY(float rBottom, float rTop, float h, int slices = 36) {
    // Upright tapered cup (closed at the bottom, open at the top) along +Y
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);                    // GLU cylinders are Z-up
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    // wall
    gluCylinder(q, rBottom, rTop, h, slices, 1);

    // bottom cap so you never see inside
    gluDisk(q, 0.0f, rBottom, slices, 1);

    gluDeleteQuadric(q);
    glPopMatrix();
}

// convenience
static inline float zSurf(float r) { return r + 0.010f; }
static void drawNeckStump(float R) {
    matWhite(); // neck same as panda fur
    glPushMatrix();
    glTranslatef(0.0f, -0.40f * R, 0.0f);   // position right under the head
    glScalef(0.55f * R, 0.30f * R, 0.55f * R); // slightly flattened cylinder
    glutSolidCube(1.0f);
    glPopMatrix();
}
// Black panda patch behind the eye (flatter in Z, smaller so snout is visible)
static void drawPatchOval(float R, float x, float y, float z) {
    matBlack();
    glPushMatrix();
    glTranslatef(x, y, z);
    // smaller oval: less wide/tall, still a bit flattened in Z
    glScalef(0.22f * R, 0.28f * R, 0.070f * R);
    glutSolidSphere(1.0f, 24, 18);
    glPopMatrix();
}
// Big cute anime eye (much larger; same pastel blue)
// RENDER ORDER: call after drawing the black patch.
static void drawCuteEye(float R, float x, float y, float zPatchCenter,
    float lookX = 0.0f, float lookY = 0.0f)
{
    // Eye sits clearly in front of the patch
    const float zEye = zPatchCenter + 0.075f * R;

    // Small look offset (inward/up is cute)
    const float offX = lookX * 0.020f * R;
    const float offY = lookY * 0.020f * R;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);               // test against head, but don't write

    // --- Big WHITE sclera ---
    matPureWhite();
    glPushMatrix();
    glTranslatef(x - 0.010f * R + offX, y + 0.020f * R + offY, zEye);
    glScalef(0.165f * R, 0.165f * R, 0.060f * R);   // << much larger
    glutSolidSphere(1.0f, 28, 22);
    glPopMatrix();

    // --- Large pastel-blue IRIS (same shade as before) ---
    matIrisPastel();
    glPushMatrix();
    glTranslatef(x + offX, y + 0.012f * R + offY, zEye + 0.012f * R);
    glScalef(0.125f * R, 0.125f * R, 0.046f * R);
    glutSolidSphere(1.0f, 28, 22);
    glPopMatrix();

    // --- Big PUPIL ---
    matBlack();
    glPushMatrix();
    glTranslatef(x + 0.004f * R + offX, y + 0.014f * R + offY, zEye + 0.020f * R);
    glScalef(0.070f * R, 0.070f * R, 0.028f * R);
    glutSolidSphere(1.0f, 24, 18);
    glPopMatrix();

    // --- Two highlights (sparkle) ---
    matPureWhite();
    // large sparkle
    glPushMatrix();
    glTranslatef(x - 0.030f * R + offX, y + 0.040f * R + offY, zEye + 0.026f * R);
    glScalef(0.024f * R, 0.024f * R, 0.016f * R);
    glutSolidSphere(1.0f, 18, 14);
    glPopMatrix();
    // tiny sparkle
    glPushMatrix();
    glTranslatef(x + 0.020f * R + offX, y - 0.014f * R + offY, zEye + 0.022f * R);
    glScalef(0.010f * R, 0.010f * R, 0.008f * R);
    glutSolidSphere(1.0f, 14, 10);
    glPopMatrix();

    glPopAttrib();
}
// Place a pair of ears higher on the head (toy-style silhouette)
static void drawEarPairHigh(float R) {
    matBlack();

    // Tunables — raise Y, move slightly back in Z, a bit closer to the dome in X
    const float earR = 0.34f * R;  // was 0.28R; slightly smaller looks cuter when higher
    const float earX = 0.92f * R;  // horizontal offset
    const float earY = 0.8f * R;  // HIGHER than before (was ~0.46R)
    const float earZ = -0.05f * R; // a bit back so they hug the crown

    // Slight squash so they read as panda ears (not perfect spheres)
    const float sx = 1.00f, sy = 0.86f, sz = 0.94f;

    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * earX, earY, earZ);
        glScalef(sx, sy, sz);
        drawSpherePrim(earR, 18, 12);
        glPopMatrix();
    }
}
// Nose + mouth (always visible, slightly forward)
static void drawNoseMouth(float R, float zSurface) {
    // push them in front of the (now ~0.070R) patch
    const float zN = zSurface + 0.098f * R;   // nose depth (was ~0.030R)
    const float zM = zSurface + 0.092f * R;   // mouth depth (was ~0.022R)

    // nose
    matBlack();
    glPushMatrix();
    glTranslatef(0.0f, -0.116f * R, zN);
    glScalef(0.060f * R, 0.040f * R, 0.032f * R);
    glutSolidSphere(1.0f, 12, 10);
    glPopMatrix();

    // mouth — keep it drawn last and non-writing so it can’t get buried
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

// Ears, proportional
static void drawEars(float R) {
    matBlack();
    const float earR = 0.28f * R;
    const float earY = 0.8f * R;
    const float earX = 0.70f * R;
    const float earZ = -0.10f * R;
    glPushMatrix(); glTranslatef(-earX, earY, earZ); drawSpherePrim(earR, 18, 12); glPopMatrix();
    glPushMatrix(); glTranslatef(earX, earY, earZ); drawSpherePrim(earR, 18, 12); glPopMatrix();
}

// --- Head assembly ---
void drawHeadUnit() {
    glPushMatrix();

    const float R = 0.66f;               // panda head "radius"
    const float z = zSurf(R);            // surface Z for facial features

    // (1) LOWER HEAD (slimmer neck connection)
    matWhite();
    glPushMatrix();
    glTranslatef(0.0f, -0.25f * R, 0.0f);
    drawTaperCupY(0.78f * R, 0.95f * R, 0.55f * R, 40);
    glPopMatrix();

    // (2) TOP DOME
    glPushMatrix();
    glTranslatef(0.0f, 0.16f * R, 0.0f);
    glScalef(1.05f, 0.95f, 1.05f);
    drawSpherePrim(R, 32, 24);
    glPopMatrix();

    // (3) EARS
    drawEarPairHigh(R);

    // (4) EYE PATCHES + EYES — now patches smaller so snout shows
    {
        const float zFace = zSurf(R);
        const float ex = 0.205f * R;
        const float ey = 0.065f * R;

        // smaller patches first
        drawPatchOval(R, -ex, ey, zFace);
        drawPatchOval(R, ex, ey, zFace);

        // big cute eyes on top
        drawCuteEye(R, -ex, ey, zFace, 0.7f, 0.15f);
        drawCuteEye(R, ex, ey, zFace, -0.7f, 0.15f);
    }

    // (5) NOSE + MOUTH
    drawNoseMouth(R, z);

    // (6) optional neck ring
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, -0.36f * R, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.030f, 0.34f, 16, 36);
    glPopMatrix();

    // (7) NECK STUMP
    drawNeckStump(R);

    glPopMatrix();
}
