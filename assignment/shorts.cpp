#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawShorts() {
    // ----- references (unchanged waist position) -----
    const float hipX = MS.hipX;
    const float hipY = -0.98f;           // thigh attach
    const float topY = hipY + 0.34f;     // WAIST stays here
    const float hemY = hipY - 0.02f;     // trunk hem (unchanged)
    const float depth = 0.18f, width = 0.95f;

    // ----- waistband (same place, but the gold rope is HIGHER) -----
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, topY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* qb = gluNewQuadric();
    gluQuadricNormals(qb, GLU_SMOOTH);
    const float waistR = MS.torsoBotR * 0.95f;
    gluCylinder(qb, waistR, waistR, 0.05f, 44, 1);   // short band, no caps
    gluDeleteQuadric(qb);
    glPopMatrix();

    // gold rope slightly HIGHER than before
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, topY + 0.02f, 0.0f);          // << raised
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.014f, waistR + 0.004f, 12, 44);
    glPopMatrix();

    // ----- trunk (same height; does NOT move down) -----
    matVest();
    const float trunkH = topY - hemY;
    const float midY = (topY + hemY) * 0.5f;

    // front / back
    glPushMatrix(); glTranslatef(0.0f, midY, depth * 0.98f);  glScalef(width * 0.78f, trunkH, 0.06f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, midY, -depth * 0.98f);  glScalef(width * 0.78f, trunkH, 0.06f); glutSolidCube(1.0f); glPopMatrix();
    // sides with slight taper
    glPushMatrix(); glTranslatef(-width * 0.45f, midY, 0.0f); glRotatef(8, 0, 1, 0);  glScalef(0.08f, trunkH, depth * 1.65f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(width * 0.45f, midY, 0.0f); glRotatef(-8, 0, 1, 0); glScalef(0.08f, trunkH, depth * 1.65f); glutSolidCube(1.0f); glPopMatrix();

    // ----- cuffs: LONGER by extending downward (waist unchanged) -----
    const float cuffY = hemY - 0.02f;    // same attach height below hem
    const float cuffH = 0.28f;           // << increased from ~0.18f
    const float cuffR0 = 0.29f, cuffR1 = 0.30f;

    // left cuff
    glPushMatrix();
    glTranslatef(-hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q1 = gluNewQuadric(); gluQuadricNormals(q1, GLU_SMOOTH);
    gluCylinder(q1, cuffR0, cuffR1, cuffH, 32, 1);   // no caps
    gluDeleteQuadric(q1);
    glPopMatrix();

    // right cuff
    glPushMatrix();
    glTranslatef(hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric(); gluQuadricNormals(q2, GLU_SMOOTH);
    gluCylinder(q2, cuffR0, cuffR1, cuffH, 32, 1);   // no caps
    gluDeleteQuadric(q2);
    glPopMatrix();

    // small gold rings at cuff tops (unchanged)
    matRope();
    glPushMatrix(); glTranslatef(-hipX, cuffY + cuffH - 0.005f, 0.0f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.012f, cuffR1 + 0.005f, 10, 36); glPopMatrix();
    glPushMatrix(); glTranslatef(hipX, cuffY + cuffH - 0.005f, 0.0f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.012f, cuffR1 + 0.005f, 10, 36); glPopMatrix();
}
