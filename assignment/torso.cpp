#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// ---------------- helpers ----------------
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

// Gold trim pipes - more prominent for Nezha style
static void drawEdgePipeAt(float deg) {
    const float a = deg2rad(deg);
    const float rAvg = 0.5f * (MS.torsoTopR + MS.torsoBotR) + 0.006f;
    const float x = rAvg * cosf(a);
    const float z = rAvg * sinf(a);

    matRope(); // Gold color
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.022f, MS.torsoH * 0.85f, 24); // Slightly thicker trim
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
    // Adjust vest to be more front-facing for better visibility
    constexpr float VEST_GAP_DEG = 75.0f;  // Smaller gap, more coverage
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float startDeg = edgeL;
    const float sweepDeg = 360.0f - VEST_GAP_DEG;

    // Vest outer shell - positioned to show more from front
    matVest();
    glPushMatrix();
    // Rotate the entire vest slightly to show more front coverage
    glRotatef(-10.0f, 0, 1, 0); // Rotate vest to face more forward
    drawOpenCylinderY(MS.torsoBotR * 0.95f, MS.torsoTopR * 0.92f, MS.torsoH, startDeg, sweepDeg, 64);
    glPopMatrix();

    // Solid inner body
    drawTorsoCore(/*asSkin=*/true);
    
    // Modify side panels to extend more to the front
    matVest();
    
    // Left side panel - extend more forward
    glPushMatrix();
    glTranslatef(-0.45f, 0.0f, 0.15f); // Move closer to center and forward
    glRotatef(25, 0, 1, 0); // Angle more toward front
    glScalef(0.18f, MS.torsoH * 0.75f, 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Right side panel - extend more forward  
    glPushMatrix();
    glTranslatef(0.45f, 0.0f, 0.15f); // Move closer to center and forward
    glRotatef(-25, 0, 1, 0); // Angle more toward front
    glScalef(0.18f, MS.torsoH * 0.75f, 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Enhanced front panels - make them larger and more prominent
    const float panelY = 0.1f;
    const float panelZ = 0.38f;  // Even more forward
    const float flare = 5.0f;    // Less flare, more straight

    // LEFT front panel - larger
    matVest();
    glPushMatrix();
    glTranslatef(-0.22f, panelY, panelZ);
    glRotatef(flare, 0, 1, 0);
    glScalef(0.32f, MS.torsoH * 0.85f, 0.05f); // Wider panels
    glutSolidCube(1.0f);
    glPopMatrix();

    // RIGHT front panel - larger
    glPushMatrix();
    glTranslatef(0.22f, panelY, panelZ);
    glRotatef(-flare, 0, 1, 0);
    glScalef(0.32f, MS.torsoH * 0.85f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Center chest detail
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.40f); // More forward
    glScalef(0.08f, 0.25f, 0.03f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Close the top
    drawVestTopYoke();

    // Gold piping - adjust for new vest position
    drawEdgePipeAt(90.0f + VEST_GAP_DEG * 0.5f - 10.0f);   // Adjust for rotation
    drawEdgePipeAt(90.0f - VEST_GAP_DEG * 0.5f - 10.0f);   // Adjust for rotation

    // Waist area
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.68f, 0.0f);
    glScalef(1.15f, 0.22f, 0.55f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Waist trim
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, -0.68f, 0.0f);
    glScalef(1.18f, 0.08f, 0.58f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawHipWrap() {
    // More fitted hip wrap to match Nezha's proportions
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.80f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float h = 0.35f;  // Shorter for better proportions
    const float r = MS.torsoBotR * 0.92f; // More fitted

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r * 0.95f, h, 32, 1); // Slight taper
    gluDisk(q, 0.0f, r, 32, 1);
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0.0f, r * 0.95f, 32, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}
