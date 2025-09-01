#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawShorts() {
    // ----- refs (waist stays put) -----
    const float hipX = MS.hipX;
    const float hipY = -0.98f;          // thigh attach
    const float topY = hipY + 0.34f;    // waist (unchanged)
    const float hemY = hipY - 0.02f;    // trunk hem (unchanged)
    const float depth = 0.18f, width = 0.95f;

    // ===== waistband (short band) =====
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, topY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* qb = gluNewQuadric();
    gluQuadricNormals(qb, GLU_SMOOTH);
    const float waistR = MS.torsoBotR * 0.95f;
    gluCylinder(qb, waistR, waistR, 0.05f, 44, 1);   // no caps -> no disks
    gluDeleteQuadric(qb);
    glPopMatrix();

    // gold rope a touch higher
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, topY + 0.024f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.014f, waistR + 0.004f, 12, 44);
    glPopMatrix();

    // ===== trunk (a tiny bit wider so shorts read looser) =====
    matVest();
    const float trunkH = topY - hemY;
    const float midY = (topY + hemY) * 0.5f;

    // front / back
    glPushMatrix(); glTranslatef(0.0f, midY, depth * 0.98f);  glScalef(width * 0.82f, trunkH, 0.06f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.0f, midY, -depth * 0.98f);  glScalef(width * 0.82f, trunkH, 0.06f); glutSolidCube(1.0f); glPopMatrix();

    // sides with slight taper
    glPushMatrix(); glTranslatef(-width * 0.46f, midY, 0.0f); glRotatef(8, 0, 1, 0);  glScalef(0.08f, trunkH, depth * 1.70f); glutSolidCube(1.0f); glPopMatrix();
    glPushMatrix(); glTranslatef(width * 0.46f, midY, 0.0f); glRotatef(-8, 0, 1, 0); glScalef(0.08f, trunkH, depth * 1.70f); glutSolidCube(1.0f); glPopMatrix();

    // ===== cuffs (LONGER + FLARED) =====
    // keep the top of each cuff right under hemY; extend DOWN and flare bottom
    const float cuffY = hemY - 0.02f;   // attach just under trunk hem
    const float cuffH = 0.30f;          // longer legs
    const float cuffRTop = 0.31f;          // radius at cuff top
    const float cuffRBot = 0.36f;          // larger -> visible flare at bottom

    // left cuff
    glPushMatrix();
    glTranslatef(-hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q1 = gluNewQuadric(); gluQuadricNormals(q1, GLU_SMOOTH);
    gluCylinder(q1, cuffRTop, cuffRBot, cuffH, 40, 1);   // no caps (no disks)
    gluDeleteQuadric(q1);
    glPopMatrix();

    // right cuff
    glPushMatrix();
    glTranslatef(hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric(); gluQuadricNormals(q2, GLU_SMOOTH);
    gluCylinder(q2, cuffRTop, cuffRBot, cuffH, 40, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();

    // subtle gold rings at cuff tops (unchanged)
    matRope();
    glPushMatrix(); glTranslatef(-hipX, cuffY + cuffH - 0.005f, 0.0f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.012f, cuffRTop + 0.01f, 10, 36); glPopMatrix();
    glPushMatrix(); glTranslatef(hipX, cuffY + cuffH - 0.005f, 0.0f); glRotatef(90, 1, 0, 0); glutSolidTorus(0.012f, cuffRTop + 0.01f, 10, 36); glPopMatrix();

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, hemY - 0.04f, 0.0f);
    glScalef(0.22f, 0.08f, 0.10f);   // narrow front “bridge”
    glutSolidCube(1.0f);
    glPopMatrix();

}

