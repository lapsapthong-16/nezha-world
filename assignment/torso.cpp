#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// ---------- materials (match head white & matte black fur) ----------
static void matPandaWhiteMatchHead() {
    const GLfloat amb[] = { 0.35f,0.35f,0.35f,1.0f };  // same recipe as head white
    const GLfloat diff[] = { 1.00f,1.00f,1.00f,1.0f };
    const GLfloat spec[] = { 0.30f,0.30f,0.30f,1.0f };
    const GLfloat emi[] = { 0.03f,0.03f,0.03f,1.0f };  // tiny lift so it stays bright under rim
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
}
static void matFurBlackMatte() {
    const GLfloat amb[] = { 0.06f,0.06f,0.06f,1.0f };
    const GLfloat diff[] = { 0.10f,0.10f,0.10f,1.0f };
    const GLfloat spec[] = { 0.00f,0.00f,0.00f,1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

// ---------- gold trim helper ----------
static void drawGoldEdgeAtDeg(float deg) {
    const float a = deg2rad(deg);
    const float cs = cosf(a), sn = sinf(a);

    const float rBot = MS.torsoBotR * 0.95f;
    const float rTop = MS.torsoTopR * 0.92f;

    const float eps = 0.010f;               // lift off shell to avoid z-fight
    const float t = 0.050f;               // strip width
    const float y0 = -MS.torsoH * 0.5f + 0.015f;
    const float y1 = MS.torsoH * 0.5f - 0.015f;

    const float tx = -sn, tz = cs;          // tangent

    const float xBi = (rBot + eps) * cs + (t * -0.5f) * tx;
    const float zBi = (rBot + eps) * sn + (t * -0.5f) * tz;
    const float xBo = (rBot + eps) * cs + (t * 0.5f) * tx;
    const float zBo = (rBot + eps) * sn + (t * 0.5f) * tz;

    const float xTi = (rTop + eps) * cs + (t * -0.5f) * tx;
    const float zTi = (rTop + eps) * sn + (t * -0.5f) * tz;
    const float xTo = (rTop + eps) * cs + (t * 0.5f) * tx;
    const float zTo = (rTop + eps) * sn + (t * 0.5f) * tz;

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matRope();
    glBegin(GL_QUADS);
    glNormal3f(cs, 0.0f, sn);
    glVertex3f(xBi, y0, zBi);
    glVertex3f(xBo, y0, zBo);
    glVertex3f(xTo, y1, zTo);
    glVertex3f(xTi, y1, zTi);
    glEnd();

    if (wasCull) glEnable(GL_CULL_FACE);
}

// ---------- panda torso (white belly matches head) ----------
static void drawPandaTorsoCore() {
    const float rBot = MS.torsoBotR * 0.88f;
    const float rTop = MS.torsoTopR * 0.85f;
    const float h = MS.torsoH + 0.08f;

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);    // make cylinder Y-up
    glTranslatef(0, 0, -h * 0.5f);

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    const int   numSlices = 25;
    const float sliceHeight = h / numSlices;
    const int   numSegments = 32;
    const float angleStepDeg = 360.0f / numSegments;

    for (int i = 0; i < numSlices; ++i) {
        float t = (float)i / (float)(numSlices - 1);
        float tNext = (float)(i + 1) / (float)(numSlices - 1);

        float r0 = rBot + (rTop - rBot) * t;
        float r1 = rBot + (rTop - rBot) * tNext;

        for (int seg = 0; seg < numSegments; ++seg) {
            float a0 = deg2rad(seg * angleStepDeg);
            float a1 = deg2rad((seg + 1) * angleStepDeg);

            // curved white belly region in front, black shoulders/sides
            bool isBlack;
            if (t > 0.70f) {
                isBlack = true;  // shoulders/chest
            }
            else if (t > 0.35f) {
                // front-centered width shrinks toward top
                float frontDeg = fmodf(seg * angleStepDeg + 180.0f, 360.0f);
                if (frontDeg > 180.0f) frontDeg = 360.0f - frontDeg;
                float heightFactor = (0.70f - t) / 0.35f; // 1..0
                float bellyWidth = 65.0f + 25.0f * heightFactor;
                float curve = 4.0f * t * (1.0f - t);
                bellyWidth += 15.0f * curve;
                isBlack = !(frontDeg <= bellyWidth);
            }
            else {
                isBlack = false; // mostly white at bottom
            }

            if (isBlack) matFurBlackMatte(); else matPandaWhiteMatchHead();

            glPushMatrix();
            glTranslatef(0, 0, i * sliceHeight);
            glBegin(GL_QUADS);
            // bottom
            glNormal3f(cosf(a0), 0, sinf(a0));
            glVertex3f(r0 * cosf(a0), r0 * sinf(a0), 0);
            glNormal3f(cosf(a1), 0, sinf(a1));
            glVertex3f(r0 * cosf(a1), r0 * sinf(a1), 0);
            // top
            glNormal3f(cosf(a1), 0, sinf(a1));
            glVertex3f(r1 * cosf(a1), r1 * sinf(a1), sliceHeight);
            glNormal3f(cosf(a0), 0, sinf(a0));
            glVertex3f(r1 * cosf(a0), r1 * sinf(a0), sliceHeight);
            glEnd();
            glPopMatrix();
        }
    }

    // caps
    matPandaWhiteMatchHead(); gluDisk(q, 0.0f, rBot, 32, 1);   // bottom = white belly
    matFurBlackMatte();      glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 32, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
    if (wasCull) glEnable(GL_CULL_FACE);
    glPopMatrix();
}

// ---------- vest bits ----------
static void drawVestTopYoke() {
    matVest();
    const float yTop = MS.torsoH * 0.5f;
    const float rOut = MS.torsoTopR + 0.002f;
    const float rIn = 0.38f;
    glPushMatrix();
    glTranslatef(0.0f, yTop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, rIn, rOut, 48, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

static void drawSidePanels() {
    matVest();
    glPushMatrix();
    glTranslatef(-0.42f, 0.0f, 0.05f);
    glRotatef(14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.42f, 0.0f, 0.05f);
    glRotatef(-14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

static void drawWaistSeal() {
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.690f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float rTop = MS.torsoBotR * 0.915f;
    const float rBot = MS.torsoBotR * 0.920f;
    const float h = 0.16f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBot, rTop, h, 44, 1);
    gluDisk(q, 0.0f, rBot, 44, 1);
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 44, 1);
    glPopMatrix();
    gluDeleteQuadric(q);

    glPopMatrix();
    if (wasCull) glEnable(GL_CULL_FACE);
}

// ---------- public ----------
void drawTorso() {
    constexpr float VEST_GAP_DEG = 75.0f;
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float edgeR = 90.0f - VEST_GAP_DEG * 0.5f;
    const float yawDeg = -8.0f;

    // vest shell
    matVest();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawOpenCylinderY(
        MS.torsoBotR * 0.95f,
        MS.torsoTopR * 0.92f,
        MS.torsoH,
        edgeL,
        360.0f - VEST_GAP_DEG,
        64);
    glPopMatrix();

    // panda body under vest (white belly matches head)
    drawPandaTorsoCore();

    // finishings
    drawVestTopYoke();
    drawWaistSeal();
    drawSidePanels();

    // waist band
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.675f, 0.0f);
    glScalef(1.12f, 0.20f, 0.54f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Gold edges: draw LAST with depth testing ON + small polygon offset ---
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);                 // keep depth testing!
    glDepthMask(GL_TRUE);                    // write depth
    glEnable(GL_POLYGON_OFFSET_FILL);
    // Negative values pull the geometry slightly toward the camera to avoid z-fighting
    glPolygonOffset(-1.0f, -1.0f);

    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawGoldEdgeAtDeg(edgeL);
    drawGoldEdgeAtDeg(edgeR);
    glPopMatrix();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();
}

void drawHipWrap() {
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.74f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float h = 0.12f;
    const float r = MS.torsoBotR * 0.90f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, 44, 1);

    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}
