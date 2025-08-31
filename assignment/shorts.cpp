#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawShorts() {
    // Use vest material for traditional reddish-brown pants
    matVest();

    // Main pants body - more fitted and traditional style
    // Front panel - wider and more fitted
    glPushMatrix();
    glTranslatef(0.0f, -0.85f, 0.18f);
    glScalef(0.75f, 0.45f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Back panel - similar to front
    glPushMatrix();
    glTranslatef(0.0f, -0.85f, -0.18f);
    glScalef(0.75f, 0.45f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Left side panel - connects front and back
    glPushMatrix();
    glTranslatef(-0.35f, -0.85f, 0.0f);
    glScalef(0.08f, 0.45f, 0.36f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Right side panel
    glPushMatrix();
    glTranslatef(0.35f, -0.85f, 0.0f);
    glScalef(0.08f, 0.45f, 0.36f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Left leg sleeve - fitted around upper leg
    glPushMatrix();
    glTranslatef(-MS.hipX, -0.85f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, 0.28f, 0.30f, 0.25f, 28, 1);
    gluDeleteQuadric(q);
    glPopMatrix();

    // Right leg sleeve
    glPushMatrix();
    glTranslatef(MS.hipX, -0.85f, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric();
    gluQuadricNormals(q2, GLU_SMOOTH);
    gluCylinder(q2, 0.28f, 0.30f, 0.25f, 28, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();

    // Add decorative golden trim/rope detail at waist
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, -0.65f, 0.0f);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(0.03f, 0.42f, 12, 32);
    glPopMatrix();
}
