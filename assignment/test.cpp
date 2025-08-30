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
    constexpr float CLEAR[4] = { 0.05f, 0.06f, 0.08f, 1.0f };
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

void drawTorso() {
    // vest (slight taper cylinder, capped)
    matVest();
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, MS.torsoBotR, MS.torsoTopR, MS.torsoH, 44, 1);
    gluDisk(q, 0.0, MS.torsoBotR, 44, 1);                      // bottom cap
    glPushMatrix(); glTranslatef(0, 0, MS.torsoH);
    gluDisk(q, 0.0, MS.torsoTopR, 44, 1);                  // top cap
    glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();

    // chest plate (flat panel like ref)
    matShirt();
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.30f);
    glScalef(0.95f, 0.95f, 0.18f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // narrow shoulder yoke
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, MS.torsoH * 0.45f, 0.0f);
    glScalef(1.15f, 0.06f, 1.15f);
    glutSolidCube(1.0f);
    glPopMatrix();

    drawVestEdges();

    // vest skirt flap (shorter)
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.72f, 0.0f);
    glScalef(1.28f, 0.26f, 0.64f);
    glutSolidCube(1.0f);
    glPopMatrix();
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
    matRope();
    const int   N = 18;
    const float R = 0.62f;     // belt radius around torso
    const float y = -0.25f;    // belt height (y)
    for (int i = 0; i < N; i++) {
        float t = (float)i / N * 2.0f * (float)M_PI;
        float x = R * cosf(t);
        float z = R * sinf(t);
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(-t * 180.0f / (float)M_PI, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        glutSolidTorus(0.04f, 0.10f, 10, 14);
        glPopMatrix();
    }

    // two hanging ropes
    glPushMatrix(); glTranslatef(0.10f, y - 0.05f, 0.55f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.04f, 0.10f, 10, 14); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.10f, y - 0.30f, 0.55f); glRotatef(70, 1, 0, 0); glutSolidTorus(0.04f, 0.10f, 10, 14); glPopMatrix();
}

// ============================================================
// 6) CHARACTER ASSEMBLY (compose parts)
// ============================================================
void drawCharacter() {
    glPushMatrix();
    drawTorso();
    glPushMatrix(); glTranslatef(0.0f, MS.headLift, 0.0f); drawHeadUnit(); glPopMatrix();
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
