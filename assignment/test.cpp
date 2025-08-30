// BMCS2173 Starter: hierarchical model (torso + head + arms + shorts/legs)
// Uses FreeGLUT (immediate mode)

#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/freeglut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------- Camera state ----------
double camDist = 8.0;
double camYaw = 25.0;
double camPitch = 15.0;

// ---------- Helpers ----------
inline void setMaterial(float r, float g, float b, float shininess = 16.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

inline void setMaterialHair(float r, float g, float b, float shininess = 8.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.15f, 0.15f, 0.15f, 1.0f }; // much weaker highlight
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

inline void drawSphere(float radius, int slices = 24, int stacks = 18) {
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

inline void drawRoundedBox(float sx, float sy, float sz, float r) {
    // very simple rounded box using scaled sphere for “softness”
    glPushMatrix();
    glScalef(sx, sy, sz);
    drawSphere(r);
    glPopMatrix();
}

void drawVestEdges() {
    setMaterial(0.65f, 0.12f, 0.10f); // slightly brighter red
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

// --- proportion knobs (tuned to ref: big head, short torso, chunky limbs) ---
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

// ---------- Body parts ----------
void drawTorso() {
    // vest (slight taper cylinder)
    setMaterial(0.55f, 0.20f, 0.12f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, MS.torsoBotR, MS.torsoTopR, MS.torsoH, 44, 1);
    gluDisk(q, 0.0, MS.torsoBotR, 44, 1); // bottom cap
    glPushMatrix(); glTranslatef(0, 0, MS.torsoH); gluDisk(q, 0.0, MS.torsoTopR, 44, 1); glPopMatrix(); // top cap (avoid light leaks)
    gluDeleteQuadric(q);
    glPopMatrix();

    // chest plate (flat panel like in ref)
    setMaterial(0.62f, 0.62f, 0.62f);
    glPushMatrix();
    glTranslatef(0.0f, 0.06f, 0.30f);
    glScalef(0.95f, 0.95f, 0.18f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // narrow shoulder yoke
    setMaterial(0.55f, 0.20f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, MS.torsoH * 0.45f, 0.0f);
    glScalef(1.15f, 0.06f, 1.15f);
    glutSolidCube(1.0f);
    glPopMatrix();

    drawVestEdges();

    // vest skirt flap (shorter)
    setMaterial(0.55f, 0.20f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, -0.72f, 0.0f);
    glScalef(1.28f, 0.26f, 0.64f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawHeadUnit() {
    // neck (short)
    setMaterial(1.0f, 0.85f, 0.75f);
    glPushMatrix(); glTranslatef(0.0f, 0.02f, 0.0f); glRotatef(-90, 1, 0, 0); drawCappedCylinder(0.22f, 0.24f, 24); glPopMatrix();

    // scarf ring
    setMaterial(0.8f, 0.1f, 0.1f);
    glPushMatrix(); glTranslatef(0.0f, 0.25f, 0.0f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.08f, 0.5f, 20, 40); glPopMatrix();

    // head (slightly squashed)
    setMaterial(1.0f, 0.87f, 0.77f);
    glPushMatrix();
    glTranslatef(0.0f, 0.78f, 0.0f);
    glScalef(1.0f, 0.92f, 1.0f);
    drawSphere(MS.headR, 36, 24);
    glPopMatrix();

    // ---- HAIR CAP (taller + larger so it covers the skull apex) ----
    //   previous version's Y scale put the hair *below* the head apex -> "bald spot".
    //   These numbers guarantee hair_apex_y > head_apex_y.
    auto setMaterialHair = [](float r, float g, float b, float shininess = 8.0f) {
        const GLfloat diff[] = { r, g, b, 1.0f };
        const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
        const GLfloat spec[] = { 0.15f, 0.15f, 0.15f, 1.0f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        };

    const float hairLift = 0.84f;               // higher center
    const float hairR = MS.headR + 0.05f;    // larger radius
    const float hairScaleY = 0.90f;               // taller than before

    setMaterialHair(0.08f, 0.08f, 0.08f);
    glPushMatrix();
    glTranslatef(0.0f, hairLift, 0.0f);
    glScalef(1.08f, hairScaleY, 1.08f);
    drawSphere(hairR, 36, 24);
    glPopMatrix();

    // thin, dark rim to make the boundary obvious
    setMaterialHair(0.06f, 0.06f, 0.06f);
    glPushMatrix(); glTranslatef(0.0f, 0.64f, 0.0f); glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.01f, MS.headR * 0.94f, 16, 48);
    glPopMatrix();

    // hair buns
    setMaterialHair(0.08f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef(-0.66f, 1.06f, -0.02f); drawSphere(0.24f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.66f, 1.06f, -0.02f); drawSphere(0.24f); glPopMatrix();

    // ears
    setMaterial(1.0f, 0.87f, 0.77f);
    glPushMatrix(); glTranslatef(-0.83f, 0.60f, 0.00f); drawSphere(0.18f, 24, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.83f, 0.60f, 0.00f); drawSphere(0.18f, 24, 16); glPopMatrix();
}

void drawArmChain(bool left) {
    float side = left ? -1.f : 1.f;

    // sink shoulder into torso: pull inward (X) and slightly backward (Z)
    glTranslatef(side * MS.shoulderX, MS.shoulderY, -0.08f);

    // shoulder joint (skin only, no sleeve ring)
    setMaterial(1.0f, 0.85f, 0.75f);
    drawSphere(0.23f);

    // upper arm
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.upperArmR, MS.upperArmH, 32);
    glPopMatrix();

    // elbow
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    drawSphere(MS.jointR);

    // forearm
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(MS.lowerArmR, MS.lowerArmH, 32);
    glPopMatrix();

    // wrist + hand (round fist)
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    drawSphere(MS.jointR * 0.90f);
    setMaterial(1.0f, 0.85f, 0.75f);
    glPushMatrix();
    glScalef(1.05f, 0.90f, 1.05f);
    drawSphere(0.25f, 28, 18);
    glPopMatrix();
}


void drawShorts() {
    setMaterial(0.35f, 0.15f, 0.08f);
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
    glPushMatrix(); glTranslatef(0.0f, -0.88f, 0.25f); glScalef(0.9f, 0.30f, 0.06f); glutSolidCube(1.0f); glPopMatrix();
}

static void drawBoot(float sideX, float yBottom, float zOffset) {
    // rounded boot (sphere heel + capsule toe)
    // heel
    setMaterial(0.15f, 0.15f, 0.15f);
    glPushMatrix();
    glTranslatef(sideX, yBottom + 0.10f, zOffset - 0.02f);
    glScalef(0.42f, 0.22f, 0.46f);
    drawSphere(0.55f, 28, 20);
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
    setMaterial(1.0f, 0.85f, 0.75f);

    // upper leg (short)
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.23f, MS.upperLegH, 28);
    glPopMatrix();

    // knee
    glPushMatrix(); glTranslatef(side * MS.hipX, -0.98f - MS.upperLegH, 0.0f); drawSphere(0.17f); glPopMatrix();

    // lower leg
    glPushMatrix();
    glTranslatef(side * MS.hipX, -0.98f - MS.upperLegH, 0.0f);
    glRotatef(-90, 1, 0, 0);
    drawCappedCylinder(0.21f, MS.lowerLegH, 28);
    glPopMatrix();

    // boot (rounded)
    drawBoot(side * MS.hipX, -0.98f - MS.upperLegH - MS.lowerLegH - 0.12f, 0.10f);
}

void drawBraidedBelt() {
    setMaterial(0.90f, 0.70f, 0.15f); // yellow rope
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

// ---------- Character assembly ----------
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

// ---------- GLUT callbacks ----------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera
    double cx = camDist * cos(camPitch * M_PI / 180.0) * sin(camYaw * M_PI / 180.0);
    double cy = camDist * sin(camPitch * M_PI / 180.0);
    double cz = camDist * cos(camPitch * M_PI / 180.0) * cos(camYaw * M_PI / 180.0);
    gluLookAt(cx, cy, cz, 0, 0.5f, 0, 0, 1, 0);

    // light each frame (after camera)
    GLfloat lightPos[] = { 3.5f, 5.5f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Ground plane (lower it a bit)
    setMaterial(0.1f, 0.12f, 0.14f);
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
    glClearColor(0.05f, 0.06f, 0.08f, 1.0f);

    // light properties
    const GLfloat ambient[] = { 0.28f,0.28f,0.28f,1 };
    const GLfloat diffuse[] = { 0.95f,0.95f,0.95f,1 };
    const GLfloat specular[] = { 0.6f,0.6f,0.6f,1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
