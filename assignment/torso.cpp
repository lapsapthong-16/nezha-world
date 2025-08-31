#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// ---------------- helpers ----------------
static void drawGoldEdgeAtDeg(float deg)
{
    // follows the tapered vest rim exactly
    const float a = deg2rad(deg);
    const float cs = cosf(a), sn = sinf(a);

    // match the vest taper you use for drawOpenCylinderY(...)
    const float rBot = MS.torsoBotR * 0.95f;
    const float rTop = MS.torsoTopR * 0.92f;

    // push slightly outward so it never z-fights the vest shell
    const float eps = 0.010f;

    // visual thickness across the opening
    const float t = 0.050f;

    // keep a little margin from top/bottom of the vest
    const float y0 = -MS.torsoH * 0.5f + 0.015f;
    const float y1 = MS.torsoH * 0.5f - 0.015f;

    // tangent in XZ (perpendicular to rim direction)
    const float tx = -sn, tz = cs;

    // bottom inner/outer
    const float xBi = (rBot + eps) * cs + (t * -0.5f) * tx;
    const float zBi = (rBot + eps) * sn + (t * -0.5f) * tz;
    const float xBo = (rBot + eps) * cs + (t * 0.5f) * tx;
    const float zBo = (rBot + eps) * sn + (t * 0.5f) * tz;

    // top inner/outer (uses rTop so the strip matches the taper)
    const float xTi = (rTop + eps) * cs + (t * -0.5f) * tx;
    const float zTi = (rTop + eps) * sn + (t * -0.5f) * tz;
    const float xTo = (rTop + eps) * cs + (t * 0.5f) * tx;
    const float zTo = (rTop + eps) * sn + (t * 0.5f) * tz;

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);   // avoid disappearing when viewed edge-on

    matRope();
    glBegin(GL_QUADS);
    glNormal3f(cs, 0.0f, sn);           // outward-ish normal
    glVertex3f(xBi, y0, zBi);
    glVertex3f(xBo, y0, zBo);
    glVertex3f(xTo, y1, zTo);
    glVertex3f(xTi, y1, zTi);
    glEnd();

    if (wasCull) glEnable(GL_CULL_FACE);
}

// Solid inner body so the vest opening shows SKIN, not the floor
static void drawTorsoCore(bool asSkin = true) {
    if (asSkin) matSkin(); else matShirt();

    // More fitted to body - closer to skin
    const float rBot = MS.torsoBotR * 0.88f;  // Slightly more fitted
    const float rTop = MS.torsoTopR * 0.85f;  // More fitted at top

    // Height adjusted for better proportions
    const float h = MS.torsoH - 0.01f;

    // GLU cylinders are along +Z; rotate to stand up on Y and center on origin
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, -h * 0.5f);

    // To be safe against culling on caps, temporarily disable cull
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBot, rTop, h, 32, 1);               // wall
    gluDisk(q, 0.0f, rBot, 32, 1);                      // bottom cap
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 32, 1);                      // top cap
    glPopMatrix();

    gluDeleteQuadric(q);
    if (wasCull) glEnable(GL_CULL_FACE);
    glPopMatrix();
}

// Nezha-style vest collar/yoke - more traditional Chinese style
static void drawVestTopYoke() {
    matVest();
    const float yTop = MS.torsoH * 0.5f;
    const float rOuter = MS.torsoTopR + 0.002f;
    const float rInner = 0.38f; // Slightly smaller neck opening

    glPushMatrix();
    glTranslatef(0.0f, yTop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, rInner, rOuter, 48, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// Redesigned front panels for Nezha - more traditional Chinese vest style
static void drawFrontPanels() {
    const float panelY = 0.1f;   // Slightly higher
    const float panelZ = 0.32f;  // More forward
    const float flare = 8.0f;    // Less flare for cleaner look

    // LEFT panel - more fitted
    matVest();
    glPushMatrix();
    glTranslatef(-0.25f, panelY, panelZ);
    glRotatef(flare, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.85f, 0.04f); // Slightly narrower and thinner
    glutSolidCube(1.0f);
    glPopMatrix();

    // RIGHT panel
    glPushMatrix();
    glTranslatef(0.25f, panelY, panelZ);
    glRotatef(-flare, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.85f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Add center chest detail - traditional Chinese vest often has center decoration
    matRope(); // Gold accent
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.34f);
    glScalef(0.06f, 0.25f, 0.02f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Add traditional Chinese-style side panels
static void drawSidePanels() {
    matVest();
    
    // Left side panel
    glPushMatrix();
    glTranslatef(-0.55f, 0.0f, 0.0f);
    glRotatef(15, 0, 1, 0); // Slight curve following body
    glScalef(0.15f, MS.torsoH * 0.75f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Right side panel
    glPushMatrix();
    glTranslatef(0.55f, 0.0f, 0.0f);
    glRotatef(-15, 0, 1, 0);
    glScalef(0.15f, MS.torsoH * 0.75f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// ---------------- public ----------------
void drawTorso() {
    // Opening width and a tiny yaw so the shell isn’t perfectly straight-on
    constexpr float VEST_GAP_DEG = 75.0f;
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float edgeR = 90.0f - VEST_GAP_DEG * 0.5f;
    const float yawDeg = -8.0f;                 // keep small; used for shell & gold

    // --- Vest outer shell (tapered open cylinder) ---
    matVest();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawOpenCylinderY(
        MS.torsoBotR * 0.95f,                   // bottom radius
        MS.torsoTopR * 0.92f,                   // top radius (smaller -> taper)
        MS.torsoH,                              // height
        edgeL,                                  // start (left rim)
        360.0f - VEST_GAP_DEG,                  // sweep around to right rim
        64);
    glPopMatrix();

    // --- Solid inner core (skin) so the opening shows body, not the floor ---
    drawTorsoCore(/*asSkin=*/true);

    // --- Close the top (yoke) ---
    drawVestTopYoke();

    // --- Side panels: thinner/closer so they don’t protrude at the front ---
    matVest();
    // left
    glPushMatrix();
    glTranslatef(-0.42f, 0.0f, 0.05f);
    glRotatef(14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // right
    glPushMatrix();
    glTranslatef(0.42f, 0.0f, 0.05f);
    glRotatef(-14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Front panels: a touch narrower so the chest reads clearly ---
    // left
    glPushMatrix();
    glTranslatef(-0.20f, 0.10f, 0.37f);
    glRotatef(4.0f, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.84f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // right
    glPushMatrix();
    glTranslatef(0.20f, 0.10f, 0.37f);
    glRotatef(-4.0f, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.84f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Waist band (slightly thinner/raised to avoid intersections) ---
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.675f, 0.0f);
    glScalef(1.12f, 0.20f, 0.54f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Gold edges: draw LAST, with depth test off, inside same yaw as shell ---
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);                  // ensures both trims are visible
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawGoldEdgeAtDeg(edgeL);                  // left rim
    drawGoldEdgeAtDeg(edgeR);                  // right rim
    glPopMatrix();
    glPopAttrib();
}

void drawHipWrap() {

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.74f, 0.0f); 
    glRotatef(-90, 1, 0, 0);

    const float h = 0.12f;              // very short
    const float r = MS.torsoBotR * 0.90f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    gluCylinder(q, r, r, h, 44, 1);     // wall only — no top/bottom disks

    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}


