#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawShorts() {
    // darker cloth derived from vest color
    matVest();  // use wrapper instead of raw setMaterial

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

    // back cloth
    glPushMatrix();
    glTranslatef(0.0f, -0.90f, -0.22f);
    glScalef(0.95f, 0.32f, 0.06f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // side flaps (left & right)
    glPushMatrix();
    glTranslatef(-MS.hipX - 0.08f, -0.90f, 0.0f);
    glScalef(0.06f, 0.30f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(MS.hipX + 0.08f, -0.90f, 0.0f);
    glScalef(0.06f, 0.30f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
}
