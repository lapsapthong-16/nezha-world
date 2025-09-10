#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// Panda fur materials
static void matPandaWhiteMatchHead() {
    const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    const GLfloat diff[] = { 1.00f, 1.00f, 1.00f, 1.0f };
    const GLfloat spec[] = { 0.30f, 0.30f, 0.30f, 1.0f };
    const GLfloat emi[] = { 0.03f, 0.03f, 0.03f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
}

static void matFurBlackMatte() {
    const GLfloat amb[] = { 0.06f, 0.06f, 0.06f, 1.0f };
    const GLfloat diff[] = { 0.10f, 0.10f, 0.10f, 1.0f };
    const GLfloat spec[] = { 0.00f, 0.00f, 0.00f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
}

// Gold trim for vest edges
static void drawGoldEdgeAtDeg(float deg) {
    const float a = deg2rad(deg);
    const float cs = cosf(a), sn = sinf(a);
    const float rBot = MS.torsoBotR * 0.95f;
    const float rTop = MS.torsoTopR * 0.92f;
    const float eps = 0.010f;
    const float t = 0.050f;
    const float y0 = -MS.torsoH * 0.5f + 0.015f;
    const float y1 = MS.torsoH * 0.5f - 0.015f;
    const float tx = -sn, tz = cs;

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
    PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUADS_PRIM);
    glBegin(GL_QUADS);
    glNormal3f(cs, 0.0f, sn);
    glVertex3f(xBi, y0, zBi);
    glVertex3f(xBo, y0, zBo);
    glVertex3f(xTo, y1, zTo);
    glVertex3f(xTi, y1, zTi);
    glEnd();

    if (wasCull) glEnable(GL_CULL_FACE);
}

// Panda body with white belly and black shoulders
static void drawPandaTorsoCore() {
    const float rBot = MS.torsoBotR * 0.88f;
    const float rTop = MS.torsoTopR * 0.85f;
    const float h = MS.torsoH + 0.08f;

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, -h * 0.5f);

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    const int numSlices = 25;
    const float sliceHeight = h / numSlices;
    const int numSegments = 32;
    const float angleStepDeg = 360.0f / numSegments;

    // Build torso with color variation
    for (int i = 0; i < numSlices; ++i) {
        float t = (float)i / (float)(numSlices - 1);
        float tNext = (float)(i + 1) / (float)(numSlices - 1);
        float r0 = rBot + (rTop - rBot) * t;
        float r1 = rBot + (rTop - rBot) * tNext;

        for (int seg = 0; seg < numSegments; ++seg) {
            float a0 = deg2rad(seg * angleStepDeg);
            float a1 = deg2rad((seg + 1) * angleStepDeg);

            // Determine color based on position
            bool isBlack;
            if (t > 0.70f) {
                isBlack = true;
            } else if (t > 0.35f) {
                float frontDeg = fmodf(seg * angleStepDeg + 180.0f, 360.0f);
                if (frontDeg > 180.0f) frontDeg = 360.0f - frontDeg;
                float heightFactor = (0.70f - t) / 0.35f;
                float bellyWidth = 65.0f + 25.0f * heightFactor;
                float curve = 4.0f * t * (1.0f - t);
                bellyWidth += 15.0f * curve;
                isBlack = !(frontDeg <= bellyWidth);
            } else {
                isBlack = false;
            }

            if (isBlack) matFurBlackMatte(); else matPandaWhiteMatchHead();

            glPushMatrix();
            glTranslatef(0, 0, i * sliceHeight);
            PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUADS_PRIM);
            glBegin(GL_QUADS);
            glNormal3f(cosf(a0), 0, sinf(a0));
            glVertex3f(r0 * cosf(a0), r0 * sinf(a0), 0);
            glNormal3f(cosf(a1), 0, sinf(a1));
            glVertex3f(r0 * cosf(a1), r0 * sinf(a1), 0);
            glNormal3f(cosf(a1), 0, sinf(a1));
            glVertex3f(r1 * cosf(a1), r1 * sinf(a1), sliceHeight);
            glNormal3f(cosf(a0), 0, sinf(a0));
            glVertex3f(r1 * cosf(a0), r1 * sinf(a0), sliceHeight);
            glEnd();
            glPopMatrix();
        }
    }

    // End caps
    matPandaWhiteMatchHead();
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rBot, 32, 1);
    matFurBlackMatte();
    glPushMatrix();
    glTranslatef(0, 0, h);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rTop, 32, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
    if (wasCull) glEnable(GL_CULL_FACE);
    glPopMatrix();
}

// Helper functions for decorative elements
static void drawRingArcY(float y, float rIn, float rOut, float startDeg, float sweepDeg, int segs) {
    const float step = sweepDeg / float(segs);

    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);

    PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUAD_STRIP_PRIM);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float deg = startDeg + step * float(i);
        float a = deg2rad(deg);
        float c = cosf(a), s = sinf(a);

        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(rOut * c, rOut * s, 0.0f);
        glVertex3f(rIn * c, rIn * s, 0.0f);
    }
    glEnd();

    glPopMatrix();
}

static void drawStudRingWithGapY(float y, float radius, int total, float studR, float gapCenterDeg, float gapWidthDeg) {
    if (total <= 0 || gapWidthDeg <= 0.0f) return;

    const float EPS = 2.0f;

    auto norm360 = [](float d) -> float {
        while (d < 0.0f) d += 360.0f;
        while (d >= 360.0f) d -= 360.0f;
        return d;
    };

    float startGap = norm360(gapCenterDeg - gapWidthDeg * 0.5f + EPS);
    float endGap = norm360(gapCenterDeg + gapWidthDeg * 0.5f - EPS);

    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);

    for (int i = 0; i < total; ++i) {
        float deg = (i + 0.5f) * (360.0f / (float)total);
        float d = norm360(deg);

        bool inGap = (startGap <= endGap) ? (d >= startGap && d <= endGap) : (d >= startGap || d <= endGap);
        if (inGap) continue;

        float a = deg2rad(deg);
        float c = cosf(a), s = sinf(a);

        glPushMatrix();
        glTranslatef(radius * c, 0.0f, radius * s);
        drawSpherePrim(studR, 10, 8);
        glPopMatrix();
    }

    glPopMatrix();
}

// Vest top with black belly section
static void drawVestTopYoke(float yawDeg) {
    const float yTop = MS.torsoH * 0.5f;
    const float rOut = MS.torsoTopR + 0.002f;
    const float rIn = 0.38f;
    const float centerDeg = 270.0f;
    const float bellyDeg = 140.0f;
    const float startB = centerDeg - bellyDeg * 0.5f;
    const float endB = centerDeg + bellyDeg * 0.5f;
    const float EPS_ANG = 0.8f;
    const float epsR = 0.0020f;
    const float rOutB = rOut + epsR;
    const float rInB = rIn - epsR;

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);

    // Red vest sections
    matVest();
    float startRed1 = endB + EPS_ANG;
    float sweepRed1 = fmaxf(0.0f, (360.0f - startRed1) - (startB - EPS_ANG));
    if (sweepRed1 > 0.0f) drawRingArcY(yTop, rIn, rOut, startRed1, sweepRed1, 96);

    float startRed2 = 0.0f;
    float sweepRed2 = fmaxf(0.0f, (startB - EPS_ANG) - startRed2);
    if (sweepRed2 > 0.0f) drawRingArcY(yTop, rIn, rOut, startRed2, sweepRed2, 96);

    // Black belly section
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-2.5f, -2.5f);

    matFurBlackMatte();
    drawRingArcY(yTop + 0.0008f, rInB, rOutB, startB, bellyDeg, 72);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}

static void drawSidePanels() {
    matVest();
    glPushMatrix();
    glTranslatef(-0.42f, 0.0f, 0.05f);
    glRotatef(14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    countGlutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.42f, 0.0f, 0.05f);
    glRotatef(-14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    countGlutSolidCube(1.0f);
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
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q, rBot, rTop, h, 44, 1);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rBot, 44, 1);
    glPushMatrix();
    glTranslatef(0, 0, h);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rTop, 44, 1);
    glPopMatrix();
    gluDeleteQuadric(q);

    glPopMatrix();
    if (wasCull) glEnable(GL_CULL_FACE);
}

// Main torso drawing function
void drawTorso() {
    // Set polygon counter to count torso polygons
    PolygonCounter::setCurrentPart(BodyPart::TORSO);
    
    constexpr float VEST_GAP_DEG = 75.0f;
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float edgeR = 90.0f - VEST_GAP_DEG * 0.5f;
    const float yawDeg = -8.0f;

    // Main vest shell
    matVest();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawOpenCylinderY(MS.torsoBotR * 0.95f, MS.torsoTopR * 0.92f, MS.torsoH, edgeL, 360.0f - VEST_GAP_DEG, 64);
    glPopMatrix();

    // Panda body underneath
    drawPandaTorsoCore();

    // Vest components
    drawVestTopYoke(yawDeg);
    drawWaistSeal();
    drawSidePanels();

    // Waist band
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.675f, 0.0f);
    glScalef(1.12f, 0.20f, 0.54f);
    countGlutSolidCube(1.0f);
    glPopMatrix();

    // Decorative elements
    const float yCollar = MS.torsoH * 0.5f - 0.012f;
    const float rCollar = MS.torsoTopR * 0.90f;
    matRope();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawStudRingWithGapY(yCollar, rCollar, 96, 0.016f, 90.0f, 150.0f);
    glPopMatrix();

    // Vest edge stitching
    Vec3 Ltop(-MS.torsoTopR * 0.78f, MS.torsoH * 0.46f, 0.06f);
    Vec3 Lbot(-MS.torsoBotR * 0.82f, -MS.torsoH * 0.46f, 0.06f);
    Vec3 Rtop(MS.torsoTopR * 0.78f, MS.torsoH * 0.46f, 0.06f);
    Vec3 Rbot(MS.torsoBotR * 0.82f, -MS.torsoH * 0.46f, 0.06f);

    matRope();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawStitchStrip(Ltop, Lbot, 60, 0.010f);
    drawStitchStrip(Rtop, Rbot, 60, 0.010f);
    glPopMatrix();

    // Gold edge trim
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);

    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawGoldEdgeAtDeg(edgeL);
    drawGoldEdgeAtDeg(edgeR);
    glPopMatrix();

    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();

    // Additional decorative rings
    const float yWaist = -0.675f;
    const float rWaist = MS.torsoBotR * 0.96f;
    matRope();
    drawStudRing(yWaist, rWaist, 110, 0.018f);

    const float yHem = -MS.torsoH * 0.5f + 0.010f;
    const float rHem = MS.torsoBotR * 0.95f;
    matRope();
    drawStudRing(yHem, rHem, 120, 0.014f);
}

void drawHipWrap() {
    // Set polygon counter to count torso polygons
    PolygonCounter::setCurrentPart(BodyPart::TORSO);
    
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.74f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float h = 0.12f;
    const float r = MS.torsoBotR * 0.90f;

    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, 44, 1);

    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}
