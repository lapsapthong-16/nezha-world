#include "head.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>
#include <cmath>

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

static void matRibbon() {
    const GLfloat amb[] = { 0.4f, 0.05f, 0.05f, 1.0f };
    const GLfloat diff[] = { 0.8f, 0.1f, 0.1f, 1.0f };
    const GLfloat spec[] = { 0.3f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);
}
// slim ribbon that wraps around the ear; sizes driven by earR
static void drawEarRibbon(float R, float earR) {
    glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_CURRENT_BIT);
    matRibbon();

    // --- band around ear (major radius ~= ear radius) ---
    glPushMatrix();
    glRotatef(90.f, 0, 1, 0);                 // torus axis -> X (ear points along X)
    glRotatef(12.f, 1, 0, 0);                 // slight tilt for charm
    const float minor = 0.06f * earR;         // band thickness
    const float major = 0.98f * earR;         // sits just outside the ear
    glutSolidTorus(minor, major, 16, 48);
    glPopMatrix();

    // --- bow loops (small petals sitting on the outer side) ---
    glPushMatrix();
    glTranslatef(0.35f * earR, 0.08f * earR, 0.0f);
    glScalef(0.30f * earR, 0.45f * earR, 0.12f * earR);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.35f * earR, 0.08f * earR, 0.0f);
    glScalef(0.30f * earR, 0.45f * earR, 0.12f * earR);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- tails ---
    glPushMatrix();
    glTranslatef(0.18f * earR, -0.75f * earR, -0.05f * earR);
    glRotatef(12.f, 0, 0, 1);
    glScalef(0.14f * earR, 0.85f * earR, 0.09f * earR);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.18f * earR, -0.75f * earR, -0.05f * earR);
    glRotatef(-12.f, 0, 0, 1);
    glScalef(0.14f * earR, 0.85f * earR, 0.09f * earR);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopAttrib();
}
// Narrow ribbon band that hugs the head sphere at a fixed elevation.
// Angles: phi 0°=+X, 90°=+Z (front), 180°=-X, 270°=-Z. elevDeg is latitude (0=equator, +90=top).
static void drawHeadRibbonArc(float R, float elevDeg, float startDeg, float sweepDeg, float width)
{
    const int   segs = 64;
    const float epsR = 0.004f * R;     // lift off the surface to avoid z-fighting
    const float elev = deg2rad(elevDeg);
    const float cosE = cosf(elev), sinE = sinf(elev);
    const float dElev = (width / R);   // small angular width for the band
    const float elevOut = elev + dElev * 0.5f;
    const float elevIn = elev - dElev * 0.5f;

    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.5f, -1.5f);

    matRibbon();
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float t = (float)i / (float)segs;
        float phi = deg2rad(startDeg + sweepDeg * t);
        float c = cosf(phi), s = sinf(phi);

        // outer edge (slightly higher latitude)
        float co = cosf(elevOut), so = sinf(elevOut);
        float xO = (R + epsR) * co * c;
        float yO = (R + epsR) * so;
        float zO = (R + epsR) * co * s;

        // inner edge (slightly lower latitude)
        float ci = cosf(elevIn), si = sinf(elevIn);
        float xI = (R + epsR) * ci * c;
        float yI = (R + epsR) * si;
        float zI = (R + epsR) * ci * s;

        // approximate normal = position on sphere
        glNormal3f(co * c, so, co * s);
        glVertex3f(xO, yO, zO);
        glNormal3f(ci * c, si, ci * s);
        glVertex3f(xI, yI, zI);
    }
    glEnd();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();
    glPopMatrix();
}
/// Small bow glued to the head near an ear.
// (ax,ay,az) must be a point on or near the head sphere.
static void drawHeadBowAt(float ax, float ay, float az, float R)
{
    matRibbon();

    // push slightly out along the surface normal so it doesn't sink into the head
    float len = sqrtf(ax * ax + ay * ay + az * az);
    if (len < 1e-6f) len = 1.0f;
    float nx = ax / len, ny = ay / len, nz = az / len;
    float px = ax + nx * (0.006f * R);
    float py = ay + ny * (0.006f * R);
    float pz = az + nz * (0.006f * R);

    // loops
    glPushMatrix();
    glTranslatef(px + 0.035f * R, py + 0.010f * R, pz);
    glScalef(0.060f * R, 0.090f * R, 0.024f * R);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(px - 0.035f * R, py + 0.010f * R, pz);
    glScalef(0.060f * R, 0.090f * R, 0.024f * R);
    glutSolidCube(1.0f);
    glPopMatrix();

    // tails
    glPushMatrix();
    glTranslatef(px + 0.020f * R, py - 0.120f * R, pz - 0.010f * R);
    glRotatef(12.f, 0, 0, 1);
    glScalef(0.028f * R, 0.150f * R, 0.020f * R);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(px - 0.020f * R, py - 0.120f * R, pz - 0.010f * R);
    glRotatef(-12.f, 0, 0, 1);
    glScalef(0.028f * R, 0.150f * R, 0.020f * R);
    glutSolidCube(1.0f);
    glPopMatrix();
}
static void drawEarPairHigh(float R) {
    const float earR = 0.34f * R;
    const float earX = 0.92f * R;
    const float earY = 0.80f * R;
    const float earZ = -0.05f * R;

    const float sx = 1.00f, sy = 0.86f, sz = 0.94f;

    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * earX, earY, earZ);

        // ear
        matBlack();
        glPushMatrix();
        glScalef(sx, sy, sz);
        drawSpherePrim(earR, 18, 12);
        glPopMatrix();

        // crossed ribbons that HUG the ear (torus axis -> X)
        matRibbon();

        glPushMatrix();
        glRotatef(90, 0, 1, 0);          // axis Z -> X  (wrap around ear)
        glRotatef(+12, 0, 0, 1);         // slight tilt
        glutSolidTorus(0.05f * earR, 0.90f * earR, 16, 36);
        glPopMatrix();

        glPushMatrix();
        glRotatef(90, 0, 1, 0);          // axis Z -> X
        glRotatef(-12, 0, 0, 1);         // opposite tilt
        glutSolidTorus(0.05f * earR, 0.90f * earR, 16, 36);
        glPopMatrix();

        // small knot near outer/top of ear
        glPushMatrix();
        glTranslatef(0.00f, 0.95f * earR, 0.25f * earR);
        glScalef(0.20f * earR, 0.30f * earR, 0.08f * earR);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPopMatrix();
    }
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

    // Ears + local ribbons/bows
    drawEarPairHigh(R);

    // Head-hugging ribbon arcs around each ear (on the head sphere)
    {
        const float elevDeg = 47.0f;        // slightly lower to intersect ear root
        const float bandWidth = 0.025f * R;   // thinner

        // Right ear: arc around +X (phi=0°)
        drawHeadRibbonArc(R, elevDeg, 330.0f, 60.0f, bandWidth);
        drawHeadRibbonArc(R, elevDeg, 30.0f, 60.0f, bandWidth);

        // Left ear: arc around -X (phi=180°)
        drawHeadRibbonArc(R, elevDeg, 150.0f, 60.0f, bandWidth);
        drawHeadRibbonArc(R, elevDeg, 210.0f, 60.0f, bandWidth);
    } // <— close block here; no glPopMatrix() needed

    // Eye patches + eyes
    const float zFace = zSurf(R);
    const float ex = 0.205f * R;
    const float ey = 0.065f * R;
    drawPatchOval(R, -ex, ey, zFace);
    drawPatchOval(R, ex, ey, zFace);
    drawCuteEye(R, -ex, ey, zFace, 0.7f, 0.15f);
    drawCuteEye(R, ex, ey, zFace, -0.7f, 0.15f);

    // Nose + mouth
    drawNoseMouth(R, z);

    // Scarf ring
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, -0.36f * R, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.030f, 0.34f, 16, 36);
    glPopMatrix();

    // Neck stump
    drawNeckStump(R);

    glPopMatrix();
}

