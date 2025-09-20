#include "head.hpp"
#include "utils.hpp"
#include "model.hpp"
#include "meditation.hpp"   // for meditation eye closing
#include <GL/freeglut.h>
#include <cmath>

// ===== Spinning state =====
float gHeadSpinAngle = 0.0f;

// ===== Materials =====
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
// Shiny white fur for the face (strong specular so highlight pops over texture)
static void matFaceFurSpec() {
    const GLfloat amb[] = { 0.45f, 0.45f, 0.45f, 1.0f };
    const GLfloat dif[] = { 1.00f, 1.00f, 1.00f, 1.0f };
    const GLfloat spc[] = { 0.90f, 0.90f, 0.90f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
}

// ===== Texture helpers (red silk + object-linear mapping where handy) =====
static inline void bindRibbonTex() {
    glBindTexture(GL_TEXTURE_2D, gTex.redSilk);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
static inline void enableObjLinearTex(float sScale, float tScale) {
    GLfloat sPlane[4] = { sScale, 0.0f, 0.0f, 0.0f };
    GLfloat tPlane[4] = { 0.0f, tScale, 0.0f, 0.0f };
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, sPlane);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, tPlane);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
}
static inline void disableObjLinearTex() {
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
}

// ===== Small geometry helpers =====
static inline float zSurf(float r) { return r + 0.010f; }

static void drawTaperCupY(float rBottom, float rTop, float h, int slices = 36) {
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q, rBottom, rTop, h, slices, 1);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM);
    gluDisk(q, 0.0f, rBottom, slices, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// Panda eye patches
static void drawPatchOval(float R, float x, float y, float z) {
    matBlack();
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.22f * R, 0.28f * R, 0.070f * R);
    countGlutSolidSphere(1.0f, 24, 18);
    glPopMatrix();
}

// Cute eye with meditation eye-close
static void drawCuteEye(float R, float x, float y, float zPatchCenter,
    float lookX = 0.0f, float lookY = 0.0f) {
    const float zEye = zPatchCenter + 0.075f * R;
    const float offX = lookX * 0.020f * R;
    const float offY = lookY * 0.020f * R;

    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    const bool eyesClosed = meditation.isActive && meditation.eyeClose > 0.5f;

    if (eyesClosed) {
        // closed eye as thin white line
        matPureWhite();
        glPushMatrix();
        glTranslatef(x + offX, y + 0.020f * R + offY, zEye + 0.015f * R);
        glScalef(0.120f * R, 0.008f * R, 0.020f * R);
        countGlutSolidCube(1.0f);
        glPopMatrix();
    }
    else {
        const float eyeScale = 1.0f - (meditation.isActive ? meditation.eyeClose * 0.5f : 0.0f);

        // sclera
        matPureWhite();
        glPushMatrix();
        glTranslatef(x - 0.010f * R + offX, y + 0.020f * R + offY, zEye);
        glScalef(0.165f * R * eyeScale, 0.165f * R * eyeScale, 0.060f * R);
        countGlutSolidSphere(1.0f, 28, 22);
        glPopMatrix();

        // iris
        matIrisPastel();
        glPushMatrix();
        glTranslatef(x + offX, y + 0.012f * R + offY, zEye + 0.012f * R);
        glScalef(0.125f * R * eyeScale, 0.125f * R * eyeScale, 0.046f * R);
        countGlutSolidSphere(1.0f, 28, 22);
        glPopMatrix();

        // pupil
        matBlack();
        glPushMatrix();
        glTranslatef(x + 0.004f * R + offX, y + 0.014f * R + offY, zEye + 0.020f * R);
        glScalef(0.070f * R * eyeScale, 0.070f * R * eyeScale, 0.028f * R);
        countGlutSolidSphere(1.0f, 24, 18);
        glPopMatrix();

        // highlights
        matPureWhite();
        glPushMatrix();
        glTranslatef(x - 0.030f * R + offX, y + 0.040f * R + offY, zEye + 0.026f * R);
        glScalef(0.024f * R * eyeScale, 0.024f * R * eyeScale, 0.016f * R);
        countGlutSolidSphere(1.0f, 18, 14);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(x + 0.020f * R + offX, y - 0.014f * R + offY, zEye + 0.022f * R);
        glScalef(0.010f * R * eyeScale, 0.010f * R * eyeScale, 0.008f * R);
        countGlutSolidSphere(1.0f, 14, 10);
        glPopMatrix();
    }

    glPopAttrib();
}

// Nose + mouth
static void drawNoseMouth(float R, float zSurface) {
    const float zN = zSurface + 0.098f * R;
    const float zM = zSurface + 0.092f * R;

    // nose
    matBlack();
    glPushMatrix();
    glTranslatef(0.0f, -0.116f * R, zN);
    glScalef(0.060f * R, 0.040f * R, 0.032f * R);
    countGlutSolidSphere(1.0f, 12, 10);
    glPopMatrix();

    // mouth
    glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    matBrown();
    glPushMatrix();
    glTranslatef(0.0f, -0.182f * R, zM);
    glScalef(0.088f * R, 0.022f * R, 0.014f * R);
    countGlutSolidCube(1.0f);
    glPopMatrix();
    glPopAttrib();
}

// Red silk ribbon arc that hugs the head (textured)
static void drawHeadRibbonArc(float R, float elevDeg, float startDeg, float sweepDeg, float width) {
    const int segs = 64;
    const float epsR = 0.004f * R;
    const float elev = deg2rad(elevDeg);
    const float dElev = (width / R);
    const float elevOut = elev + dElev * 0.5f;
    const float elevIn = elev - dElev * 0.5f;

    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.5f, -1.5f);

    matRibbon();
    glEnable(GL_TEXTURE_2D);
    bindRibbonTex();
    const float repS = 4.0f;

    countGLQuadStrip(segs);
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float t = (float)i / (float)segs;
        float phi = deg2rad(startDeg + sweepDeg * t);
        float c = cosf(phi), s = sinf(phi);

        float co = cosf(elevOut), so = sinf(elevOut);
        float xO = (R + epsR) * co * c;
        float yO = (R + epsR) * so;
        float zO = (R + epsR) * co * s;

        float ci = cosf(elevIn), si = sinf(elevIn);
        float xI = (R + epsR) * ci * c;
        float yI = (R + epsR) * si;
        float zI = (R + epsR) * ci * s;

        glNormal3f(co * c, so, co * s);
        glTexCoord2f(repS * t, 1.0f); glVertex3f(xO, yO, zO);
        glNormal3f(ci * c, si, ci * s);
        glTexCoord2f(repS * t, 0.0f); glVertex3f(xI, yI, zI);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_POLYGON_OFFSET_FILL);
    glPopAttrib();
    glPopMatrix();
}

// Ears + local ribbons/bow (ears use black fur texture)
static void drawEarPairHigh(float R) {
    const float earR = 0.34f * R;
    const float earX = 0.92f * R;
    const float earY = 0.80f * R;
    const float earZ = -0.05f * R;

    const float sx = 1.00f, sy = 0.86f, sz = 0.94f;

    const float bandMinor = 0.055f * earR;
    const float bandMajor = 1.00f * earR;
    const float bias = 0.003f * R;

    for (int side = -1; side <= 1; side += 2) {
        glPushMatrix();
        glTranslatef(side * earX, earY, earZ);

        // ear black fur (textured)
        matBlack();
        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_TEXTURE_BIT);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glEnable(GL_TEXTURE_2D);
        glColor3f(1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, gTex.pandaBlack);
        glPushMatrix();
        glScalef(sx, sy, sz);
        drawSpherePrim(earR, 18, 12);
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopAttrib();

        // textured red silk rings + small bow
        matRibbon();
        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT);
        glEnable(GL_TEXTURE_2D);
        bindRibbonTex();
        enableObjLinearTex(4.0f / earR, 4.0f / earR);

        glPushMatrix();
        glScalef(sx, sy, sz);
        glRotatef(90, 0, 1, 0);
        glRotatef(14, 1, 0, 0);
        countGlutSolidTorus(bandMinor, bandMajor + bias, 16, 48);
        glPopMatrix();

        glPushMatrix();
        glScalef(sx, sy, sz);
        glRotatef(90, 0, 1, 0);
        glRotatef(-14, 1, 0, 0);
        countGlutSolidTorus(bandMinor, bandMajor + bias, 16, 48);
        glPopMatrix();

        // tiny bow block at top-front
        glPushMatrix();
        glScalef(sx, sy, sz);
        glTranslatef(0.00f, 0.88f * earR, 0.36f * earR);
        glScalef(0.22f * earR, 0.32f * earR, 0.10f * earR);
        countGlutSolidCube(1.0f);
        glPopMatrix();

        disableObjLinearTex();
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopAttrib();

        glPopMatrix();
    }
}

// ===== Main head assembly =====
void drawHeadUnit() {
    PolygonCounter::setCurrentPart(BodyPart::HEAD);
    PrimitiveCounter::setCurrentPart(BodyPart::HEAD);

    glPushMatrix();

    // friend feature: spin
    glRotatef(gHeadSpinAngle, 0, 1, 0);

    const float R = 0.66f;
    const float z = zSurf(R);

    // lower head/neck transition
    matWhite();
    glPushMatrix();
    glTranslatef(0.0f, -0.25f * R, 0.0f);
    drawTaperCupY(0.78f * R, 0.95f * R, 0.55f * R, 40);
    glPopMatrix();

    // main head dome — base textured pass plus reflection overlay
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.16f * R, 0.0f);
        glScalef(1.05f, 0.95f, 1.05f);

        // base textured with shiny material so specular pops
        matFaceFurSpec();
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glColor3f(1, 1, 1);
        glBindTexture(GL_TEXTURE_2D, gTex.pandaWhite);
        drawSpherePrim(R, 32, 24);
        glBindTexture(GL_TEXTURE_2D, 0);

        // environment reflection overlay using sphere-map
        GLuint envTex = gTex.cloud ? gTex.cloud : (gTex.goldBelt ? gTex.goldBelt : 0);
        if (envTex) {
            glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, envTex);
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
            glEnable(GL_TEXTURE_GEN_S);
            glEnable(GL_TEXTURE_GEN_T);

            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glColor4f(1, 1, 1, 0.22f); // reflection strength

            drawSpherePrim(R, 32, 24);

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glBindTexture(GL_TEXTURE_2D, 0);
            glPopAttrib();
        }

        glPopMatrix();
    }

    // ears + local ribbons/bows (textured)
    drawEarPairHigh(R);

    // head-hugging ribbon arcs (textured)
    {
        const float elevDeg = 47.0f;
        const float bandWidth = 0.025f * R;
        drawHeadRibbonArc(R, elevDeg, 330.0f, 60.0f, bandWidth);
        drawHeadRibbonArc(R, elevDeg, 30.0f, 60.0f, bandWidth);
        drawHeadRibbonArc(R, elevDeg, 150.0f, 60.0f, bandWidth);
        drawHeadRibbonArc(R, elevDeg, 210.0f, 60.0f, bandWidth);
    }

    // face patches + eyes (with meditation eye-close)
    const float zFace = zSurf(R);
    const float ex = 0.205f * R;
    const float ey = 0.065f * R;
    drawPatchOval(R, -ex, ey, zFace);
    drawPatchOval(R, ex, ey, zFace);
    drawCuteEye(R, -ex, ey, zFace, 0.7f, 0.15f);
    drawCuteEye(R, ex, ey, zFace, -0.7f, 0.15f);

    // nose + mouth
    drawNoseMouth(R, z);

    // scarf ring
    matScarf();
    glPushMatrix();
    glTranslatef(0.0f, -0.36f * R, 0.0f);
    glRotatef(90, 1, 0, 0);
    countGlutSolidTorus(0.030f, 0.34f, 16, 36);
    glPopMatrix();

    // tiny neck stump
    matWhite();
    glPushMatrix();
    glTranslatef(0.0f, -0.40f * R, 0.0f);
    glScalef(0.55f * R, 0.30f * R, 0.55f * R);
    countGlutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

// ===== Spin controls =====
void spinHeadLeft() { gHeadSpinAngle -= 15.0f; if (gHeadSpinAngle < -180.0f) gHeadSpinAngle += 360.0f; glutPostRedisplay(); }
void spinHeadRight() { gHeadSpinAngle += 15.0f; if (gHeadSpinAngle > 180.0f) gHeadSpinAngle -= 360.0f; glutPostRedisplay(); }
void resetHeadSpin() { gHeadSpinAngle = 0.0f; glutPostRedisplay(); }
