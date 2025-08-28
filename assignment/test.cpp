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

// ---------- Helpers (ONE set only) ----------
inline float deg2rad(float deg) { return deg * (float)M_PI / 180.0f; }

inline void setMaterial(float r, float g, float b, float shininess = 16.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
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

inline void drawCylinder(float r, float h, int slices = 24) {
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, slices, 1);     // body
    gluDisk(q, 0.0, r, slices, 1);          // bottom cap
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0.0, r, slices, 1); glPopMatrix(); // top cap
    gluDeleteQuadric(q);
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

// --- proportion knobs---
struct ModelScale {
    float torsoH = 1.30f;  // was 1.4
    float torsoTopR = 0.62f;  // taper
    float torsoBotR = 0.78f;

    float headLift = 1.05f;  // how high head unit sits above torso origin
    float headR = 0.72f;

    float shoulderX = 1.08f;  // shoulders slightly wider
    float shoulderY = 0.58f;

    float upperArmR = 0.24f;  // chunkier, shorter arms
    float upperArmH = 0.60f;
    float lowerArmR = 0.22f;
    float lowerArmH = 0.48f;
    float jointR = 0.17f;

    float shortSleeveDrop = 0.85f; // where shorts begin
    float shortSleeveLen = 0.42f;

    float hipX = 0.36f;
    float upperLegH = 0.50f;
    float lowerLegH = 0.48f;
} MS;

// ---------- Sizes for arms ----------
struct Sizes {
    float shoulderX = 1.02f;  // how far from center
    float shoulderY = 0.55f;  // height on torso
    float upperR = 0.22f;  // upper arm radius
    float upperH = 0.70f;  // upper arm length
    float lowerR = 0.20f;  // forearm radius
    float lowerH = 0.55f;  // forearm length
    float jointR = 0.16f;  // elbow/wrist spheres
} SZ;

inline void drawArmSegment(float r, float h) {
    glPushMatrix(); glRotatef(-90, 1, 0, 0); drawCylinder(r, h, 32); glPopMatrix();
}

inline void drawHandFist() {
    setMaterial(1.0f, 0.85f, 0.75f);
    glPushMatrix(); glScalef(1.0f, 0.85f, 1.0f); drawSphere(0.23f, 28, 18); glPopMatrix();
}

// ---------- Body parts ----------
void drawTorso() {
    // vest (tapered)
    setMaterial(0.55f, 0.20f, 0.12f);
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, MS.torsoBotR, MS.torsoTopR, MS.torsoH, 40, 1);
    gluDisk(q, 0.0, MS.torsoBotR, 40, 1);
    gluDeleteQuadric(q);
    glPopMatrix();

    // shirt panel (pulled forward so it's visible)
    setMaterial(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glTranslatef(0.0f, 0.1f, 0.3f);
    glScalef(1.0f, 1.0f, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

	drawVestEdges();

    // vest skirt flap
    setMaterial(0.55f, 0.20f, 0.12f);
    glPushMatrix();
    glTranslatef(0.0f, -0.75f, 0.0f);          // slightly higher
    glScalef(1.25f, 0.28f, 0.62f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawHeadUnit() {
    // neck
    setMaterial(1.0f, 0.85f, 0.75f);
    glPushMatrix(); glTranslatef(0.0f, 0.05f, 0.0f); glRotatef(-90, 1, 0, 0); drawCylinder(0.2f, 0.3f, 24); glPopMatrix();

    // scarf ring
    setMaterial(0.8f, 0.1f, 0.1f);
    glPushMatrix(); glTranslatef(0.0f, 0.25f, 0.0f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.08, 0.5, 20, 40); glPopMatrix();

    // head
    setMaterial(1.0f, 0.87f, 0.77f);
    glPushMatrix(); glTranslatef(0.0f, 0.75f, 0.0f); drawSphere(MS.headR, 36, 24); glPopMatrix();

    // hair cap
    setMaterial(0.08f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef(0.0f, 0.75f, 0.0f); glScalef(1.05f, 0.85f, 1.05f); drawSphere(MS.headR + 0.02f, 36, 24); glPopMatrix();

    // hair buns
    setMaterial(0.08f, 0.08f, 0.08f);
    glPushMatrix(); glTranslatef(-0.6f, 1.1f, 0.0f); drawSphere(0.22f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.6f, 1.1f, 0.0f); drawSphere(0.22f); glPopMatrix();

    // ears 
    setMaterial(1.0f, 0.87f, 0.77f);
    glPushMatrix(); glTranslatef(-0.78f, 0.62f, 0.00f); drawSphere(0.20f, 24, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0.78f, 0.62f, 0.00f); drawSphere(0.20f, 24, 16); glPopMatrix();
}

void drawArmChain(bool left) {
    float side = left ? -1.f : 1.f;

    glTranslatef(side * MS.shoulderX, MS.shoulderY, 0.0f);

    setMaterial(1.0f, 0.85f, 0.75f);
    drawSphere(0.19f);                         // shoulder cap

    // upper arm
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    glPushMatrix(); glRotatef(-90, 1, 0, 0); drawCylinder(MS.upperArmR, MS.upperArmH, 32); glPopMatrix();

    // elbow
    glTranslatef(0.0f, -MS.upperArmH * 0.5f, 0.0f);
    drawSphere(MS.jointR);

    // forearm
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    glPushMatrix(); glRotatef(-90, 1, 0, 0); drawCylinder(MS.lowerArmR, MS.lowerArmH, 32); glPopMatrix();

    // wrist + hand
    glTranslatef(0.0f, -MS.lowerArmH * 0.5f, 0.0f);
    drawSphere(MS.jointR * 0.9f);
    setMaterial(1.0f, 0.85f, 0.75f);
    glPushMatrix(); glScalef(1.0f, 0.85f, 1.0f); drawSphere(0.24f, 28, 18); glPopMatrix();
}

void drawShorts() {
    setMaterial(0.35f, 0.15f, 0.08f);
    // left sleeve
    glPushMatrix();
    glTranslatef(-MS.hipX, -MS.shortSleeveDrop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluCylinder(q, 0.36f, 0.42f, MS.shortSleeveLen, 28, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
    // right sleeve
    glPushMatrix();
    glTranslatef(MS.hipX, -MS.shortSleeveDrop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluCylinder(q2, 0.36f, 0.42f, MS.shortSleeveLen, 28, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();
    // front cloth
    glPushMatrix(); glTranslatef(0.0f, -0.88f, 0.25f); glScalef(0.9f, 0.30f, 0.06f); glutSolidCube(1.0f); glPopMatrix();
}

void drawLeg(bool left) {
    float side = left ? -1.f : 1.f;
    setMaterial(1.0f, 0.85f, 0.75f);
    // upper
    glPushMatrix(); glTranslatef(side * MS.hipX, -1.10f, 0.0f); glRotatef(-90, 1, 0, 0); drawCylinder(0.22f, MS.upperLegH, 28); glPopMatrix();
    // lower
    glPushMatrix(); glTranslatef(side * MS.hipX, -1.10f - MS.upperLegH, 0.0f); glRotatef(-90, 1, 0, 0); drawCylinder(0.20f, MS.lowerLegH, 28); glPopMatrix();
    // shoe
    setMaterial(0.15f, 0.15f, 0.15f);
    glPushMatrix(); glTranslatef(side * MS.hipX, -1.10f - MS.upperLegH - MS.lowerLegH - 0.15f, 0.12f); glScalef(0.55f, 0.25f, 0.9f); glutSolidCube(1.0f); glPopMatrix();
}

void drawBraidedBelt() {
    setMaterial(0.90f, 0.70f, 0.15f); // yellow rope
    const int  N = 18;
    const float R = 0.62f;     // belt radius around torso
    const float y = -0.25f;    // belt height (y)
    for (int i = 0;i < N;i++) {
        float t = (float)i / N * 2.0f * (float)M_PI;
        float x = R * cosf(t);
        float z = R * sinf(t);
        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(-t * 180.0f / (float)M_PI, 0, 1, 0);    // twist slightly
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

    glPushMatrix(); drawArmChain(true); glPopMatrix();
    glPushMatrix(); drawArmChain(false); glPopMatrix();

    drawShorts();
    glPushMatrix(); drawLeg(true); glPopMatrix();
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

    // ground
    setMaterial(0.1f, 0.12f, 0.14f);
    glPushMatrix(); glTranslatef(0, -1.2f, 0); glScalef(8, 0.05f, 8); glutSolidCube(1.0f); glPopMatrix();

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
