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

// Eye patch + eye overlays that never get buried
static void drawEyePatch(float R, float x, float y, float zPatch)
{
    // 1) Black oval patch (a shallow scaled sphere)
    matBlack();
    glPushMatrix();
    glTranslatef(x, y, zPatch);
    const float patchZ = 0.085f * R;        // patch thickness in Z
    glScalef(0.20f * R, 0.26f * R, patchZ);
    glutSolidSphere(1.0f, 18, 12);
    glPopMatrix();

    // 2) Draw eye overlays slightly in front of the patch.
    //    Use depth test ON, but depth WRITE OFF, so they can't be hidden by the patch.
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);                  // <- critical: don't write to depth

    const float zOver = zPatch + patchZ + 0.004f * R;   // just past the patch

    // --- Sclera (white), a bit larger so it reads clearly ---
    matWhite();
    glPushMatrix();
    glTranslatef(x - 0.010f * R, y + 0.016f * R, zOver);
    glScalef(0.075f * R, 0.075f * R, 0.036f * R);
    glutSolidSphere(1.0f, 20, 16);
    glPopMatrix();

    // --- Pupil (black) pushed even a touch further forward than sclera ---
    matBlack();
    glPushMatrix();
    glTranslatef(x + 0.006f * R, y + 0.010f * R, zOver + 0.012f * R);
    glScalef(0.032f * R, 0.032f * R, 0.018f * R);
    glutSolidSphere(1.0f, 16, 12);
    glPopMatrix();

    // --- Small highlight (cute sparkle) ---
    matWhite();
    glPushMatrix();
    glTranslatef(x + 0.018f * R, y + 0.024f * R, zOver + 0.018f * R);
    glScalef(0.012f * R, 0.012f * R, 0.010f * R);
    glutSolidSphere(1.0f, 12, 10);
    glPopMatrix();

    glPopAttrib();                          // restore depth writes
}

// Nose + mouth, proportional
static void drawNoseMouth(float R, float z) {
    // nose center at ~ -0.12R below eye line
    matBlack();
    glPushMatrix();
    glTranslatef(0.0f, -0.116f*R, z + 0.017f*R);
    glScalef(0.058f*R, 0.037f*R, 0.030f*R);
    glutSolidSphere(1.0f, 10, 8);
    glPopMatrix();

    // simple mouth dash at ~ -0.175R
    matBrown();
    glPushMatrix();
    glTranslatef(0.0f, -0.175f*R, z + 0.005f*R);
    glScalef(0.083f*R, 0.020f*R, 0.013f*R);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Ears, proportional
static void drawEars(float R) {
    matBlack();
    const float earR = 0.28f*R;
    const float earY = 0.35f*R;
    const float earX = 0.70f*R;
    const float earZ = -0.10f*R;
    glPushMatrix(); glTranslatef(-earX, earY, earZ); drawSpherePrim(earR, 18, 12); glPopMatrix();
    glPushMatrix(); glTranslatef( earX, earY, earZ); drawSpherePrim(earR, 18, 12); glPopMatrix();
}
// --- REPLACE your drawHeadUnit() with this version ---
void drawHeadUnit() {
    glPushMatrix();

    const float R = 0.70f;               // your chosen panda head "radius"
    const float z = zSurf(R);            // surface Z for facial features

    // (1) LOWER HEAD (slimmer neck connection) — a short tapered cup
    //     Slightly narrower at the bottom, wider toward the face.
    matWhite();
    glPushMatrix();
    glTranslatef(0.0f, -0.25f * R, 0.0f);             // sit a touch below center
    drawTaperCupY(/*rBottom=*/0.78f * R,
        /*rTop   =*/0.95f * R,
        /*h      =*/0.55f * R,
        /*slices =*/40);
    glPopMatrix();

    // (2) TOP DOME (round panda skull) — shifted up so the top feels fuller
    glPushMatrix();
    glTranslatef(0.0f, 0.16f * R, 0.0f);              // lift the dome up
    glScalef(1.05f, 0.95f, 1.05f);                    // a hint of width
    drawSpherePrim(R, 32, 24);
    glPopMatrix();

    // (3) EARS — bigger, higher, more forward
    matBlack();
    const float earR = 0.28f * R;
    glPushMatrix(); glTranslatef(-0.95f * R, 0.46f * R, 0.22f * R); drawSpherePrim(earR, 18, 12); glPopMatrix();
    glPushMatrix(); glTranslatef(0.95f * R, 0.46f * R, 0.22f * R); drawSpherePrim(earR, 18, 12); glPopMatrix();

    // (4) EYE PATCHES + EYES — a little higher on the face
    drawEyePatch(R, -0.18f * R, 0.06f * R, z);
    drawEyePatch(R, 0.18f * R, 0.06f * R, z);

    // (5) NOSE + MOUTH — slightly below patch line
    drawNoseMouth(R, z);

    // (6) optional tiny neck ring (keeps your old vibe — delete if you want)
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