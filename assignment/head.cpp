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

static void drawEyePatch(float R, float x, float y, float z) {
    // patch size ~ (0.20R, 0.26R, 0.10R)
    matBlack();
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.20f*R, 0.26f*R, 0.10f*R);
    glutSolidSphere(1.0f, 18, 12);
    glPopMatrix();

    // eye white on patch (~0.066R)
    matWhite();
    glPushMatrix();
    glTranslatef(x, y + 0.016f*R, z + 0.013f*R);
    glScalef(0.066f*R, 0.066f*R, 0.030f*R);
    glutSolidSphere(1.0f, 12, 8);
    glPopMatrix();

    // pupil (~0.026R)
    matBrown();
    glPushMatrix();
    glTranslatef(x + 0.010f*R, y + 0.010f*R, z + 0.026f*R);
    glScalef(0.026f*R, 0.026f*R, 0.016f*R);
    glutSolidSphere(1.0f, 10, 8);
    glPopMatrix();
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

    // (3) EARS — a bit high and slightly forward
    matBlack();
    const float earR = 0.28f * R;
    glPushMatrix(); glTranslatef(-0.78f * R, 0.42f * R, 0.12f * R); drawSpherePrim(earR, 18, 12); glPopMatrix();
    glPushMatrix(); glTranslatef(0.78f * R, 0.42f * R, 0.12f * R); drawSpherePrim(earR, 18, 12); glPopMatrix();

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

    glPopMatrix();
}