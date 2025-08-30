// BMCS2173 Character (blockout)
// FreeGLUT (immediate mode)
// ------------------------------------------------------------
// How this file is organized:
//
// 1) Includes & globals
// 2) Palette + material helpers  [EDIT COLORS HERE]
// 3) Primitive helpers           [OK TO REUSE]
// 4) Model parameters            [TWEAK PROPORTIONS HERE]
// 5) Body-part builders          [REPLACE SINGLE FUNCS LATER]
//      - Torso
//      - HeadUnit
//      - ArmChain
//      - Shorts
//      - Leg
//      - BraidedBelt
// 6) Character assembly          [usually no changes]
// 7) GL/GLUT setup + callbacks   [camera, lighting, input]
// ------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define SHOW_HEAD 0 
#endif

// ============================================================
// 1) CAMERA / VIEW GLOBALS
// ============================================================
double camDist = 8.0;
double camYaw = 25.0;
double camPitch = 15.0;

// ============================================================
// 2) PALETTE + MATERIAL HELPERS
//    Keep all colors in one place and use tiny wrappers.
// ============================================================
namespace Palette {
    // Base colors
    constexpr float SKIN[3] = { 1.00f, 0.85f, 0.75f };
    constexpr float HAIR[3] = { 0.08f, 0.08f, 0.08f };
    constexpr float HAIR_RIM[3] = { 0.06f, 0.06f, 0.06f };
    constexpr float VEST[3] = { 0.55f, 0.20f, 0.12f };
    constexpr float VEST_EDGE[3] = { 0.65f, 0.12f, 0.10f };
    constexpr float SHIRT[3] = { 0.62f, 0.62f, 0.62f };
    constexpr float BOOT[3] = { 0.15f, 0.15f, 0.15f };
    constexpr float ROPE[3] = { 0.90f, 0.70f, 0.15f };
    constexpr float SCARF[3] = { 0.80f, 0.10f, 0.10f };
    constexpr float GROUND[3] = { 0.10f, 0.12f, 0.14f };
    constexpr float CLEAR[4] = { 0.85f, 0.85f, 0.88f, 1.0f };
}

// Generic material (diffuse/spec set; quick to call)
inline void setMaterial(float r, float g, float b, float shininess = 16.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.90f, 0.90f, 0.90f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// Hair-specific (low specular so highlights don’t bloom)
inline void setMaterialHair(float r, float g, float b, float shininess = 8.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// Convenience wrappers (so calls read like the concept)
inline void matSkin() { setMaterial(Palette::SKIN[0], Palette::SKIN[1], Palette::SKIN[2]); }
inline void matHair() { setMaterialHair(Palette::HAIR[0], Palette::HAIR[1], Palette::HAIR[2]); }
inline void matHairRim() { setMaterialHair(Palette::HAIR_RIM[0], Palette::HAIR_RIM[1], Palette::HAIR_RIM[2]); }
inline void matVest() { setMaterial(Palette::VEST[0], Palette::VEST[1], Palette::VEST[2]); }
inline void matVestEdge() { setMaterial(Palette::VEST_EDGE[0], Palette::VEST_EDGE[1], Palette::VEST_EDGE[2]); }
inline void matShirt() { setMaterial(Palette::SHIRT[0], Palette::SHIRT[1], Palette::SHIRT[2]); }
inline void matBoot() { setMaterial(Palette::BOOT[0], Palette::BOOT[1], Palette::BOOT[2]); }
inline void matRope() { setMaterial(Palette::ROPE[0], Palette::ROPE[1], Palette::ROPE[2]); }
inline void matScarf() { setMaterial(Palette::SCARF[0], Palette::SCARF[1], Palette::SCARF[2]); }
inline void matGround() { setMaterial(Palette::GROUND[0], Palette::GROUND[1], Palette::GROUND[2]); }

// ============================================================
// 3) PRIMITIVE HELPERS
// ============================================================
inline float deg2rad(float d) { return d * (float)M_PI / 180.0f; }

// Curved vest wall: a tapered open cylinder whose axis is Y (up)
inline void drawOpenCylinderY(float rBot, float rTop, float h,
    float startDeg, float sweepDeg, int slices = 64) {
    const float y0 = -h * 0.5f;
    const float y1 = h * 0.5f;

    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float t = startDeg + sweepDeg * (float)i / (float)slices;
        float a = deg2rad(t);
        float c = cosf(a), s = sinf(a);

        // radial normal works well for small taper
        glNormal3f(c, 0.0f, s);

        glVertex3f(rBot * c, y0, rBot * s);   // bottom ring
        glVertex3f(rTop * c, y1, rTop * s);   // top ring
    }
    glEnd();
}

inline void drawSpherePrim(float radius, int slices = 24, int stacks = 18) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluSphere(q, radius, slices, stacks);
    gluDeleteQuadric(q);
}

inline void drawCappedCylinder(float r, float h, int slices = 24) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, slices, 1);
    gluDisk(q, 0.0, r, slices, 1);                 // bottom cap
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0.0, r, slices, 1); glPopMatrix(); // top cap
    gluDeleteQuadric(q);
}

// ============================================================
// 4) MODEL PARAMETERS (proportions)
//    Central place to tweak; everything reads from here.
// ============================================================
struct ModelScale {
    // torso: shorter, a bit boxy with small taper
    float torsoH = 1.02f;
    float torsoTopR = 0.70f;
    float torsoBotR = 0.82f;

    // head: bigger and a hair lower (chibi)
    float headLift = 0.92f;
    float headR = 0.95f;

    // shoulders a touch wider / higher
    float shoulderX = 0.92f;
    float shoulderY = 0.66f;

    // arms: short & chunky
    float upperArmR = 0.26f;
    float upperArmH = 0.56f;
    float lowerArmR = 0.24f;
    float lowerArmH = 0.48f;
    float jointR = 0.18f;

    // shorts top & length (for continuity)
    float shortSleeveDrop = 0.86f;
    float shortSleeveLen = 0.44f;

    // hips/legs: closer together, short legs
    float hipX = 0.34f;
    float upperLegH = 0.40f;
    float lowerLegH = 0.42f;
} MS;

// ============================================================
// 5) BODY-PART BUILDERS (each is a self-contained unit)
//    You can replace any single function below later.
// ============================================================

// ---------- Torso ----------
void drawVestEdges() {
    matVestEdge();
    // left edge
    glPushMatrix();
    glTranslatef(-0.42f, 0.05f, 0.35f);
    glScalef(0.08f, 1.05f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // right edge
    glPushMatrix();
    glTranslatef(0.42f, 0.05f, 0.35f);
    glScalef(0.08f, 1.05f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();
}
void drawInnerTorsoShell() {
    matShirt();

    glPushMatrix();
    // Align cylinder vertically
    glRotatef(-90, 1, 0, 0);
    const float down = 0.55f;                 // how much to extend downward
    glTranslatef(0, 0, -down);

    // Height chosen so top <= torsoH - 0.02 (never peeks above the vest)
    const float hTopSafe = MS.torsoH - 0.02f; // top inside the vest
    const float h = hTopSafe + down;   // total height from the shifted base

    // Slimmer than vest so it stays inside everywhere
    const float rTop = MS.torsoTopR * 0.88f;
    const float rBot = MS.torsoBotR * 0.88f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBot, rTop, h, 36, 1);
    gluDisk(q, 0.0f, rBot, 36, 1);                 // bottom cap (deep inside body)
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 36, 1);             // top cap (hidden under vest)
    glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();
}

// Gray undershirt (curved) + simple red emblem
static void drawShirtFrontWithEmblem() {
    // Curved shirt plate (thin capped cylinder), tucked slightly inside
    matShirt();
    glPushMatrix();
    glTranslatef(0.0f, 0.00f, 0.26f);  // sits behind the vest panels
    glRotatef(90, 1, 0, 0);
    const float R = 0.43f, TH = 0.04f;
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, R, R, TH, 40, 1);
    gluDisk(q, 0.0, R, 40, 1);
    glPushMatrix(); glTranslatef(0, 0, TH); gluDisk(q, 0.0, R, 40, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();

    // Emblem: simple red disk
    setMaterial(0.80f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(0.0f, 0.07f, 0.33f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluQuadricNormals(q2, GLU_SMOOTH);
    gluDisk(q2, 0.0f, 0.14f, 36, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();
}

// Left/Right vest panels with outward tilt and gold trim
static void drawVestPanel(bool left) {
    const float side = left ? -1.f : 1.f;

    // panel body
    matVest();
    glPushMatrix();
    glTranslatef(side * 0.34f, 0.00f, 0.325f);  // move to front
    glRotatef(side * 8.0f, 0, 1, 0);            // open outward a bit
    glScalef(0.35f, MS.torsoH * 0.80f, 0.06f);  // thin slab
    glutSolidCube(1.0f);
    glPopMatrix();

    // gold trim along outer edge
    matRope();
    glPushMatrix();
    glTranslatef(side * 0.50f, 0.00f, 0.325f);
    glRotatef(side * 8.0f, 0, 1, 0);
    glScalef(0.04f, MS.torsoH * 0.86f, 0.065f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // gold trim along inner edge (near the center opening)
    matRope();
    glPushMatrix();
    glTranslatef(side * 0.19f, 0.00f, 0.325f);
    glRotatef(side * 8.0f, 0, 1, 0);
    glScalef(0.03f, MS.torsoH * 0.78f, 0.065f);
    glutSolidCube(1.0f);
    glPopMatrix();
}
// Gold ring around the armhole so the arm looks "sewn" into the vest.
static void drawArmholeTrim(float x, float y, float z, float ringR, float tubeR) {
    matRope(); // gold
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(90, 0, 1, 0);           // ring faces outward (X axis)
    glutSolidTorus(tubeR, ringR, 16, 36);
    glPopMatrix();
}
// --- thin annulus that closes the vest at the shoulders (no hole through body)
static void drawVestTopYoke() {
    matVest();
    const float yTop = MS.torsoH * 0.5f;          // top of the vest shell (drawOpenCylinderY uses Y up)
    const float rOuter = MS.torsoTopR + 0.001f;   // flush with vest top radius
    const float rInner = 0.34f;                   // neck opening (adjust when you add the head back)

    glPushMatrix();
    glTranslatef(0.0f, yTop, 0.0f);
    glRotatef(-90, 1, 0, 0);                      // make GLU disk lie in XZ plane
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, rInner, rOuter, 64, 1);            // annulus => sealed shoulders
    gluDeleteQuadric(q);
    glPopMatrix();
}
void drawTorso() {
    // ---- Open vest shell (back + sides) ----
    // front opening centred on +Z
    constexpr float VEST_GAP_DEG = 100.0f;
    const float gapDeg = VEST_GAP_DEG;
    const float edgeL = 90.0f + gapDeg * 0.5f;   // left rim angle
    const float startDeg = edgeL;                   // start here...
    const float sweepDeg = 360.0f - gapDeg;         // ...sweep around the back to the right rim

    matVest();
    glPushMatrix();
    drawOpenCylinderY(MS.torsoBotR, MS.torsoTopR, MS.torsoH, startDeg, sweepDeg, 72);
    glPopMatrix();

    // ---- NEW: close the top so you can't see inside ----
    drawVestTopYoke();

    // ---- Shirt + emblem (sits slightly inside the opening) ----
    drawShirtFrontWithEmblem();

    // ---- Front vest panels with gold trim (matches the ref look) ----
    drawVestPanel(true);
    drawVestPanel(false);

    // ---- Waist flap (unchanged) ----
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
    // Move it up so it tucks *under* the vest hem
    glTranslatef(0.0f, -0.82f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float h = 0.48f;                     // taller wrap seals the waist
    const float r = MS.torsoBotR * 0.99f;      // closely matches vest bottom

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, 44, 1);            // wall
    gluDisk(q, 0.0f, r, 44, 1);                // top cap (blocks view upward)
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, r, 44, 1);            // bottom cap
    glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}
// ---------- Head Unit ----------
void drawHeadUnit() {
    // neck (short)
    matSkin();
    glPushMatrix();
    glTranslatef(0.0f, 0.02f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.22f, 0.24f, 24);
    glPopMatrix();

    // scarf ring
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, 0.25f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.08f, 0.5f, 20, 40);
    glPopMatrix();

    // head (slightly squashed)
    matSkin();
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, 0.0f);
    glScalef(1.0f, 0.92f, 1.0f);
    drawSpherePrim(MS.headR, 36, 24);
    glPopMatrix();

    // hair cap — taller & larger to fully cover skull
    const float hairLift = 0.84f;
    const float hairR = MS.headR + 0.05f;
    const float hairScaleY = 0.90f;

    matHair();
    glPushMatrix();
    glTranslatef(0.0f, hairLift, 0.0f);
    glScalef(1.08f, hairScaleY, 1.08f);
    drawSpherePrim(hairR, 36, 24);
    glPopMatrix();

    // thin hairline rim
    matHairRim();
    glPushMatrix();
    glTranslatef(0.0f, 0.64f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.01f, MS.headR * 0.94f, 16, 48);
    glPopMatrix();

    // hair buns
    matHair();
    glPushMatrix(); glTranslatef(-0.66f, 1.06f, -0.02f); drawSpherePrim(0.24f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.66f, 1.06f, -0.02f); drawSpherePrim(0.24f); glPopMatrix();

    // ears
    matSkin();
    glPushMatrix(); glTranslatef(-0.83f, 0.60f, 0.00f); drawSpherePrim(0.18f, 24, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.83f, 0.60f, 0.00f); drawSpherePrim(0.18f, 24, 16); glPopMatrix();
}

// ---------- Arm Chain ----------
void drawArmChain(bool left) {
    float side = left ? -1.f : 1.f;

    // sink shoulder into torso: pull inward (X) and slightly backward (Z)
    glTranslatef(side * MS.shoulderX, MS.shoulderY, -0.08f);

    // shoulder joint (skin only, no sleeve ring)
    matSkin();
    drawSpherePrim(0.23f);

    // upper arm
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.upperArmR, MS.upperArmH, 32);
    glPopMatrix();

    // elbow
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    drawSpherePrim(MS.jointR);

    // forearm
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.lowerArmR, MS.lowerArmH, 32);
    glPopMatrix();

    // wrist + hand (round fist)
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    drawSpherePrim(MS.jointR * 0.90f);
    matSkin();
    glPushMatrix();
    glScalef(1.05f, 0.90f, 1.05f);
    drawSpherePrim(0.25f, 28, 18);
    glPopMatrix();
}

// ---------- Shorts ----------
void drawShorts() {
    setMaterial(Palette::VEST[0] * 0.64f, Palette::VEST[1] * 0.75f, Palette::VEST[2] * 0.66f); // darker cloth
    // left sleeve
    glPushMatrix();
    glTranslatef(-MS.hipX, -MS.shortSleeveDrop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, 0.36f, 0.42f, MS.shortSleeveLen, 28, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
    // right sleeve
    glPushMatrix();
    glTranslatef(MS.hipX, -MS.shortSleeveDrop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluQuadricNormals(q2, GLU_SMOOTH);
    gluCylinder(q2, 0.36f, 0.42f, MS.shortSleeveLen, 28, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();
    // front cloth
    glPushMatrix();
    glTranslatef(0.0f, -0.88f, 0.25f);
    glScalef(0.9f, 0.30f, 0.06f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // back cloth (covers lower back)
    glPushMatrix();
    glTranslatef(0.0f, -0.90f, -0.22f);
    glScalef(0.95f, 0.32f, 0.06f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // side flaps (left & right) to close remaining gaps
    glPushMatrix();
    glTranslatef(-MS.hipX - 0.08f, -0.90f, 0.0f);
    glScalef(0.06f, 0.30f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(MS.hipX + 0.08f, -0.90f, 0.0f);
    glScalef(0.06f, 0.30f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();

}

// ---------- Leg ----------
static void drawBoot(float sideX, float yBottom, float zOffset) {
    matBoot();
    // heel
    glPushMatrix();
    glTranslatef(sideX, yBottom + 0.10f, zOffset - 0.02f);
    glScalef(0.42f, 0.22f, 0.46f);
    drawSpherePrim(0.55f, 28, 20);
    glPopMatrix();
    // toe
    glPushMatrix();
    glTranslatef(sideX, yBottom + 0.10f, zOffset + 0.22f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.22f, 0.40f, 28);
    glPopMatrix();
}

void drawLeg(bool left) {
    float side = left ? -1.f : 1.f;
    matSkin();

    // upper leg (short)
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.23f, MS.upperLegH, 28);
    glPopMatrix();

    // knee
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f - MS.upperLegH, 0.0f);
    drawSpherePrim(0.17f);
    glPopMatrix();

    // lower leg
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f - MS.upperLegH, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.21f, MS.lowerLegH, 28);
    glPopMatrix();

    // boot (rounded)
    drawBoot(side * MS.hipX, -0.98f - MS.upperLegH - MS.lowerLegH - 0.12f, 0.10f);
}

// ---------- Braided Belt ----------
void drawBraidedBelt() {
    // Make sure rope never disappears at glancing angles
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matRope();

    // ===== main rope ring =====
    const int   N = 22;
    const float y = -0.245f;              // belt height
    const float R = 0.60f;                // distance from center
    const float tube = 0.06f;             // rope thickness (bigger than before)

    for (int i = 0; i < N; ++i) {
        float t = (float)i / N * 2.0f * (float)M_PI;
        float x = R * cosf(t);
        float z = R * sinf(t);
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(t * 180.0f / (float)M_PI, 0, 1, 0); // twist a little as it goes
        glRotatef(90, 1, 0, 0);
        glutSolidTorus(tube * 0.35f, tube, 12, 24);
        glPopMatrix();
    }

    // ===== front knot (thicker, sits at +Z) =====
    const float kz = 0.54f;
    const float kx = 0.00f;
    const float ky = y + 0.01f;

    // two interlocked thick rings to fake a knot
    glPushMatrix();
    glTranslatef(kx, ky, kz);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(tube * 0.42f, tube * 1.05f, 14, 28);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(kx + 0.10f, ky, kz - 0.06f);
    glRotatef(35, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(tube * 0.38f, tube * 0.95f, 14, 28);
    glPopMatrix();

    // small bulge in the middle of the knot
    glPushMatrix();
    glTranslatef(kx + 0.04f, ky + 0.01f, kz - 0.01f);
    glutSolidSphere(tube * 0.80f, 18, 14);
    glPopMatrix();

    // ===== rope ends (short tails) =====
    auto ropeEnd = [&](float sx, float sy, float sz, float yawDeg, float pitchDeg) {
        glPushMatrix();
        glTranslatef(sx, sy, sz);
        glRotatef(yawDeg, 0, 1, 0);
        glRotatef(pitchDeg, 1, 0, 0);
        // tapered cylinder: thicker near knot, thinner at tip
        GLUquadric* q = gluNewQuadric();
        gluQuadricNormals(q, GLU_SMOOTH);
        const float h = 0.34f;
        gluCylinder(q, tube * 0.70f, tube * 0.40f, h, 18, 1);
        // little cap at the end
        glPushMatrix(); glTranslatef(0, 0, h); glutSolidSphere(tube * 0.40f, 14, 10); glPopMatrix();
        gluDeleteQuadric(q);
        glPopMatrix();
        };
    ropeEnd(kx - 0.06f, ky - 0.01f, kz + 0.02f, -10.0f, 75.0f); // left end
    ropeEnd(kx + 0.12f, ky - 0.01f, kz - 0.01f, 15.0f, 75.0f); // right end

    if (wasCull) glEnable(GL_CULL_FACE);
}


// ============================================================
// 6) CHARACTER ASSEMBLY (compose parts)
// ============================================================
void drawCharacter() {
    glPushMatrix();
    drawTorso();               // vest + shirt front
    drawHipWrap();
#if SHOW_HEAD
    glPushMatrix();
    glTranslatef(0.0f, MS.headLift, 0.0f);
    drawHeadUnit();
    glPopMatrix();
#endif
    drawBraidedBelt();
    glPopMatrix();

    glPushMatrix(); drawArmChain(true);  glPopMatrix();
    glPushMatrix(); drawArmChain(false); glPopMatrix();

    drawShorts();
    glPushMatrix(); drawLeg(true);  glPopMatrix();
    glPushMatrix(); drawLeg(false); glPopMatrix();
}

// ============================================================
// 7) GLUT CALLBACKS & OPENGL SETUP
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Camera
    const double cx = camDist * cos(camPitch * M_PI / 180.0) * sin(camYaw * M_PI / 180.0);
    const double cy = camDist * sin(camPitch * M_PI / 180.0);
    const double cz = camDist * cos(camPitch * M_PI / 180.0) * cos(camYaw * M_PI / 180.0);
    gluLookAt(cx, cy, cz, 0, 0.5f, 0, 0, 1, 0);

    // Light each frame (after camera)
    GLfloat lightPos[] = { 3.5f, 5.5f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Ground plane
    matGround();
    glPushMatrix();
    glTranslatef(0, -1.50f, 0);
    glScalef(8, 0.05f, 8);
    glutSolidCube(1.0f);
    glPopMatrix();

    drawCharacter();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (float)w / h, 0.1, 100.0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 27: case 'q': exit(0); break;
    case 'w': camPitch += 3.0; break;
    case 's': camPitch -= 3.0; break;
    case 'a': camYaw -= 3.0; break;
    case 'd': camYaw += 3.0; break;
    case 'z': camDist -= 0.3; if (camDist < 3) camDist = 3; break;
    case 'x': camDist += 0.3; break;
    default: break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(960, 720);
    glutCreateWindow("BMCS2173 Character (blockout)");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glClearColor(Palette::CLEAR[0], Palette::CLEAR[1], Palette::CLEAR[2], Palette::CLEAR[3]);

    // Light properties
    const GLfloat ambient[] = { 0.28f, 0.28f, 0.28f, 1.0f };
    const GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    const GLfloat specular[] = { 0.60f, 0.60f, 0.60f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}

