// shorts.cpp
#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include "customization.hpp"   // getCurrentShirtTexture()
#include <GL/freeglut.h>
#include <cmath>

// ---------- small helpers ----------
static void drawUnitCubeTex(float repS = 1.0f, float repT = 1.0f)
{
    countGLQuads(6);
    glBegin(GL_QUADS);
    // -Z
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(repS, 0); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(repS, repT); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(0, repT); glVertex3f(-0.5f, 0.5f, -0.5f);
    // +Z
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(repS, 0); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(repS, repT); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0, repT); glVertex3f(-0.5f, 0.5f, 0.5f);
    // -X
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(repS, 0); glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(repS, repT); glVertex3f(-0.5f, 0.5f, 0.5f);
    glTexCoord2f(0, repT); glVertex3f(-0.5f, 0.5f, -0.5f);
    // +X
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(repS, 0); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(repS, repT); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0, repT); glVertex3f(0.5f, 0.5f, -0.5f);
    // -Y
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(repS, 0); glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(repS, repT); glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(0, repT); glVertex3f(-0.5f, -0.5f, 0.5f);
    // +Y
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-0.5f, 0.5f, -0.5f);
    glTexCoord2f(repS, 0); glVertex3f(0.5f, 0.5f, -0.5f);
    glTexCoord2f(repS, repT); glVertex3f(0.5f, 0.5f, 0.5f);
    glTexCoord2f(0, repT); glVertex3f(-0.5f, 0.5f, 0.5f);
    glEnd();
}

// Neutral cloth material so the silk texture isn’t tinted by previous materials
static inline void matSilkNeutral()
{
    const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    const GLfloat dif[] = { 1.00f, 1.00f, 1.00f, 1.0f };
    const GLfloat spec[] = { 0.06f, 0.06f, 0.06f, 1.0f };
    const GLfloat emi[] = { 0.00f, 0.00f, 0.00f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emi);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8.0f);
}

void drawShorts() {
    PolygonCounter::setCurrentPart(BodyPart::SHORTS);

    const float hipX = MS.hipX;
    const float hipY = -0.98f;
    const float topY = hipY + 0.34f;
    const float hemY = hipY - 0.02f;
    const float depth = 0.18f, width = 0.95f;

    const float trunkH = topY - hemY;
    const float midY = (topY + hemY) * 0.5f;

    // *** CRITICAL: bind the SAME silk as the shirt ***
    GLuint silkTex = getCurrentShirtTexture();
    if (!silkTex) {
        // graceful fallback to red if something failed to load
        silkTex = gTex.red_silk_shirt;
    }

    // ================== WAIST BAND (gold_belt.bmp) ==================
    {
        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, gTex.goldBelt);  // <- your gold belt texture
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        glPushMatrix();
        glTranslatef(0.0f, topY, 0.0f);
        glRotatef(-90, 1, 0, 0);

        GLUquadric* qb = gluNewQuadric();
        gluQuadricNormals(qb, GLU_SMOOTH);
        gluQuadricTexture(qb, GL_TRUE); // give the cylinder UVs
        const float waistR = MS.torsoBotR * 0.95f;
        PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
        gluCylinder(qb, waistR, waistR, 0.05f, 64, 1);

        gluDeleteQuadric(qb);
        glPopMatrix();

        glBindTexture(GL_TEXTURE_2D, 0);
        glPopAttrib();
    }

    // ================== Gold rope trim (unchanged) ==================
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, topY + 0.024f, 0.0f);
    glRotatef(90, 1, 0, 0);
    countGlutSolidTorus(0.014f, MS.torsoBotR * 0.95f + 0.004f, 12, 44);
    glPopMatrix();

    // ================== Main shorts fabric (SILK) ==================
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    matSilkNeutral();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, silkTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(1, 1, 1); // do not tint

    // Front
    glPushMatrix();
    glTranslatef(0.0f, midY, depth * 0.98f);
    glScalef(width * 0.82f, trunkH, 0.06f);
    drawUnitCubeTex(1.6f, 1.6f);
    glPopMatrix();

    // Back
    glPushMatrix();
    glTranslatef(0.0f, midY, -depth * 0.98f);
    glScalef(width * 0.82f, trunkH, 0.06f);
    drawUnitCubeTex(1.6f, 1.6f);
    glPopMatrix();

    // Sides
    glPushMatrix();
    glTranslatef(-width * 0.46f, midY, 0.0f);
    glRotatef(8, 0, 1, 0);
    glScalef(0.08f, trunkH, depth * 1.70f);
    drawUnitCubeTex(0.8f, 1.6f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(width * 0.46f, midY, 0.0f);
    glRotatef(-8, 0, 1, 0);
    glScalef(0.08f, trunkH, depth * 1.70f);
    drawUnitCubeTex(0.8f, 1.6f);
    glPopMatrix();

    // Leg cuffs (silk cylinders)
    const float cuffY = hemY - 0.02f;
    const float cuffH = 0.30f;
    const float cuffRTop = 0.31f;
    const float cuffRBot = 0.36f;

    glPushMatrix();
    glTranslatef(-hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q1 = gluNewQuadric();
    gluQuadricNormals(q1, GLU_SMOOTH);
    gluQuadricTexture(q1, GL_TRUE);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q1, cuffRTop, cuffRBot, cuffH, 40, 1);
    gluDeleteQuadric(q1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluQuadricNormals(q2, GLU_SMOOTH);
    gluQuadricTexture(q2, GL_TRUE);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q2, cuffRTop, cuffRBot, cuffH, 40, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();

    // Front bridge (silk)
    glPushMatrix();
    glTranslatef(0.0f, hemY - 0.04f, 0.0f);
    glScalef(0.22f, 0.08f, 0.10f);
    drawUnitCubeTex(0.7f, 0.4f);
    glPopMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();

    // ================== Cuff trim rings (rope) ==================
    matRope();
    glPushMatrix();
    glTranslatef(-hipX, cuffY + cuffH - 0.005f, 0.0f);
    glRotatef(90, 1, 0, 0);
    countGlutSolidTorus(0.012f, cuffRTop + 0.01f, 10, 36);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(hipX, cuffY + cuffH - 0.005f, 0.0f);
    glRotatef(90, 1, 0, 0);
    countGlutSolidTorus(0.012f, cuffRTop + 0.01f, 10, 36);
    glPopMatrix();
}
