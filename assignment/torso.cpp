// torso.cpp
#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>
#include <cmath>

// ====== NEW: tiny helpers so torso can use the selected shirt texture ======
// gShirtTex is defined in customization.cpp. We just reference it here.
extern GLuint gShirtTex;   // 0 means "no specific pick" -> fall back to a default

// Bind currently selected shirt texture (state is pushed/popped safely)
static inline void beginShirtMaterial() {
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1, 1, 1);

    // If user hasn't picked via H/G yet, fall back to a sensible default.
    // You already load these in main.cpp: gTex.redSilk, gTex.goldBelt, etc.
    GLuint tex = gShirtTex ? gShirtTex : gTex.goldBelt;
    glBindTexture(GL_TEXTURE_2D, tex);

    // Nice tiling behavior
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

static inline void endShirtMaterial() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();
}
// ==========================================================================


// -----------------------------------------------------------------------------
// Panda fur materials (unchanged)
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Small textured helpers for vest/shirt pieces
// -----------------------------------------------------------------------------

// Cylinder around Y with simple UVs:  U along angle, V along height.
static void drawOpenCylinderY_Tex(float rBot, float rTop, float h,
    float startDeg, float sweepDeg,
    int slices, float uRepeat = 2.0f,
    float vRepeat = 1.0f) {
    const float y0 = -h * 0.5f, y1 = h * 0.5f;
    countGLQuadStrip(slices);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float t = (float)i / (float)slices;
        float deg = startDeg + sweepDeg * t;
        float a = deg2rad(deg);
        float c = cosf(a), s = sinf(a);
        float u = uRepeat * t;

        glNormal3f(c, 0.0f, s);
        glTexCoord2f(u, 0.0f * vRepeat); glVertex3f(rBot * c, y0, rBot * s);
        glTexCoord2f(u, 1.0f * vRepeat); glVertex3f(rTop * c, y1, rTop * s);
    }
    glEnd();
}

// Thin ring strip (in XZ plane at height y) with UVs: U along arc, V across thickness.
static void drawRingArcY_Tex(float y, float rIn, float rOut,
    float startDeg, float sweepDeg,
    int segs, float uRepeat = 1.0f) {
    const float step = sweepDeg / float(segs);

    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);

    countGLQuadStrip(segs);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float t = (float)i / (float)segs;
        float deg = startDeg + step * float(i);
        float a = deg2rad(deg);
        float c = cosf(a), s = sinf(a);
        float u = uRepeat * t;

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(u, 0.0f); glVertex3f(rOut * c, rOut * s, 0.0f); // outer edge
        glTexCoord2f(u, 1.0f); glVertex3f(rIn * c, rIn * s, 0.0f); // inner edge
    }
    glEnd();
    glPopMatrix();
}

// Unit cube with UVs per face (GLUT cube has no texcoords).
static void drawTexturedUnitCube(float uRepeat = 1.0f, float vRepeat = 1.0f) {
    const float s = 0.5f;
    countGLQuads(6);
    glBegin(GL_QUADS);
    // -Y
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0);        glVertex3f(-s, -s, -s);
    glTexCoord2f(0, vRepeat);  glVertex3f(-s, -s, s);
    glTexCoord2f(uRepeat, vRepeat);  glVertex3f(s, -s, s);
    glTexCoord2f(uRepeat, 0);        glVertex3f(s, -s, -s);
    // -X
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0);        glVertex3f(-s, s, -s);
    glTexCoord2f(0, vRepeat);  glVertex3f(-s, s, s);
    glTexCoord2f(uRepeat, vRepeat);  glVertex3f(-s, -s, s);
    glTexCoord2f(uRepeat, 0);        glVertex3f(-s, -s, -s);
    // +Z
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);        glVertex3f(-s, -s, s);
    glTexCoord2f(0, vRepeat);  glVertex3f(-s, s, s);
    glTexCoord2f(uRepeat, vRepeat);  glVertex3f(s, s, s);
    glTexCoord2f(uRepeat, 0);        glVertex3f(s, -s, s);
    // +X
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0);        glVertex3f(s, -s, s);
    glTexCoord2f(0, vRepeat);  glVertex3f(s, s, s);
    glTexCoord2f(uRepeat, vRepeat);  glVertex3f(s, s, -s);
    glTexCoord2f(uRepeat, 0);        glVertex3f(s, -s, -s);
    // -Z
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0);        glVertex3f(s, -s, -s);
    glTexCoord2f(0, vRepeat);  glVertex3f(-s, -s, -s);
    glTexCoord2f(uRepeat, vRepeat);  glVertex3f(-s, s, -s);
    glTexCoord2f(uRepeat, 0);        glVertex3f(s, s, -s);
    // +Y
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0);        glVertex3f(s, s, s);
    glTexCoord2f(0, vRepeat);  glVertex3f(-s, s, s);
    glTexCoord2f(uRepeat, vRepeat);  glVertex3f(-s, s, -s);
    glTexCoord2f(uRepeat, 0);        glVertex3f(s, s, -s);
    glEnd();
}

// -----------------------------------------------------------------------------
// Decorative gold edge (unchanged draw, still uses rope material)
// -----------------------------------------------------------------------------
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
    countGLQuads(1);
    glBegin(GL_QUADS);
    glNormal3f(cs, 0.0f, sn);
    glVertex3f(xBi, y0, zBi);
    glVertex3f(xBo, y0, zBo);
    glVertex3f(xTo, y1, zTo);
    glVertex3f(xTi, y1, zTi);
    glEnd();

    if (wasCull) glEnable(GL_CULL_FACE);
}

// -----------------------------------------------------------------------------
// Panda body core (unchanged)
// -----------------------------------------------------------------------------
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

            bool isBlack;
            if (t > 0.70f) {
                isBlack = true;
            }
            else if (t > 0.35f) {
                float frontDeg = fmodf(seg * angleStepDeg + 180.0f, 360.0f);
                if (frontDeg > 180.0f) frontDeg = 360.0f - frontDeg;
                float heightFactor = (0.70f - t) / 0.35f;
                float bellyWidth = 65.0f + 25.0f * heightFactor;
                float curve = 4.0f * t * (1.0f - t);
                bellyWidth += 15.0f * curve;
                isBlack = !(frontDeg <= bellyWidth);
            }
            else {
                isBlack = false;
            }

            if (isBlack) matFurBlackMatte(); else matPandaWhiteMatchHead();

            glPushMatrix();
            glTranslatef(0, 0, i * sliceHeight);
            countGLQuads(1);
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

// -----------------------------------------------------------------------------
// Decorative helpers (unchanged except we kept for compile completeness)
// -----------------------------------------------------------------------------
static void drawRingArcY(float y, float rIn, float rOut, float startDeg, float sweepDeg, int segs) {
    const float step = sweepDeg / float(segs);

    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);

    countGLQuadStrip(segs);
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

// -----------------------------------------------------------------------------
// Vest top yoke: now textures the red vest sections; keeps black belly matte
// -----------------------------------------------------------------------------
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

    // --- Texture on the red vest sections ---
    beginShirtMaterial();
    float startRed1 = endB + EPS_ANG;
    float sweepRed1 = fmaxf(0.0f, (360.0f - startRed1) - (startB - EPS_ANG));
    if (sweepRed1 > 0.0f) drawRingArcY_Tex(yTop, rIn, rOut, startRed1, sweepRed1, 96, /*uRepeat*/2.0f);

    float startRed2 = 0.0f;
    float sweepRed2 = fmaxf(0.0f, (startB - EPS_ANG) - startRed2);
    if (sweepRed2 > 0.0f) drawRingArcY_Tex(yTop, rIn, rOut, startRed2, sweepRed2, 96, /*uRepeat*/2.0f);
    endShirtMaterial();

    // --- Black belly section stays matte ---
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

// -----------------------------------------------------------------------------
// Textured vest side panels (replaces glut cube with UV cube)
// -----------------------------------------------------------------------------
static void drawSidePanels() {
    beginShirtMaterial();

    glPushMatrix();
    glTranslatef(-0.42f, 0.0f, 0.05f);
    glRotatef(14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    drawTexturedUnitCube(/*u*/1.2f, /*v*/1.2f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.42f, 0.0f, 0.05f);
    glRotatef(-14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    drawTexturedUnitCube(/*u*/1.2f, /*v*/1.2f);
    glPopMatrix();

    endShirtMaterial();
}

// -----------------------------------------------------------------------------
// Textured waist seal (GLU cylinder with texture coords enabled)
// -----------------------------------------------------------------------------
static void drawWaistSeal() {
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    beginShirtMaterial();
    glPushMatrix();
    glTranslatef(0.0f, -0.690f, 0.0f);
    glRotatef(-90, 1, 0, 0);

    const float rTop = MS.torsoBotR * 0.915f;
    const float rBot = MS.torsoBotR * 0.920f;
    const float h = 0.16f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);                 // <-- give us UVs
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q, rBot, rTop, h, 44, 1);

    // (caps textured too; harmless if you don't see them)
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rBot, 44, 1);
    glPushMatrix();
    glTranslatef(0, 0, h);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rTop, 44, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
    glPopMatrix();
    endShirtMaterial();

    if (wasCull) glEnable(GL_CULL_FACE);
}

// -----------------------------------------------------------------------------
// Main torso drawing (vest pieces now textured)
// -----------------------------------------------------------------------------
void drawTorso() {
    PolygonCounter::setCurrentPart(BodyPart::TORSO);

    constexpr float VEST_GAP_DEG = 75.0f;
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float edgeR = 90.0f - VEST_GAP_DEG * 0.5f;
    const float yawDeg = -8.0f;

    // --- Main vest shell (textured open cylinder) ---
    beginShirtMaterial();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    // slight tiling around the body so the pattern repeats nicely
    drawOpenCylinderY_Tex(MS.torsoBotR * 0.95f, MS.torsoTopR * 0.92f, MS.torsoH,
        edgeL, 360.0f - VEST_GAP_DEG, 64,
        /*uRepeat*/3.0f, /*vRepeat*/1.0f);
    glPopMatrix();
    endShirtMaterial();

    // Panda body underneath (no texture)
    drawPandaTorsoCore();

    // Vest components
    drawVestTopYoke(yawDeg); // textured red sections inside
    drawWaistSeal();         // textured belt-like seal
    drawSidePanels();        // textured cubes

    // Waist band (textured cube)
    beginShirtMaterial();
    glPushMatrix();
    glTranslatef(0.0f, -0.675f, 0.0f);
    glScalef(1.12f, 0.20f, 0.54f);
    drawTexturedUnitCube(/*u*/2.0f, /*v*/0.5f);
    glPopMatrix();
    endShirtMaterial();

    // Decorative elements that should stay non-textured
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

// -----------------------------------------------------------------------------
// Hip wrap (kept as-is; not part of shirt texture)
// -----------------------------------------------------------------------------
void drawHipWrap() {
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
