#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawShorts() {
    // ---------- quick refs ----------
    const float hipX = MS.hipX;      // thigh center X
    const float hipY = -0.98f;       // where thighs attach
    const float topY = hipY + 0.34f; // shorts top (waist)
    const float hemY = hipY - 0.02f; // hem sits a touch above cuffs
    const float depth = 0.18f;        // front/back thickness of trunk
    const float width = 0.95f;        // overall trunk width

    // ---------- waistband (very short cylinder; no caps = no disks) ----------
    {
        matVest();
        glPushMatrix();
        glTranslatef(0.0f, topY, 0.0f);
        glRotatef(-90, 1, 0, 0);
        GLUquadric* qb = gluNewQuadric();
        gluQuadricNormals(qb, GLU_SMOOTH);
        const float waistR = MS.torsoBotR * 0.96f;
        gluCylinder(qb, waistR, waistR, 0.06f, 44, 1); // no caps
        gluDeleteQuadric(qb);
        glPopMatrix();
    }

    // ---------- trunk (stops above cuffs so it reads as shorts) ----------
    {
        matVest();
        const float trunkH = topY - hemY;
        const float midY = (topY + hemY) * 0.5f;

        // front
        glPushMatrix();
        glTranslatef(0.0f, midY, depth * 0.98f);
        glScalef(width * 0.78f, trunkH, 0.06f);
        glutSolidCube(1.0f);
        glPopMatrix();
        // back
        glPushMatrix();
        glTranslatef(0.0f, midY, -depth * 0.98f);
        glScalef(width * 0.78f, trunkH, 0.06f);
        glutSolidCube(1.0f);
        glPopMatrix();
        // sides (slight taper)
        glPushMatrix();
        glTranslatef(-width * 0.45f, midY, 0.0f);
        glRotatef(8.0f, 0, 1, 0);
        glScalef(0.08f, trunkH, depth * 1.65f);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(width * 0.45f, midY, 0.0f);
        glRotatef(-8.0f, 0, 1, 0);
        glScalef(0.08f, trunkH, depth * 1.65f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // ---------- cuffs (no caps -> no flat colored disks) ----------
    {
        const float cuffY = hemY - 0.02f;  // a hair below hem
        const float cuffH = 0.18f;
        const float cuffR0 = 0.29f;         // slightly > upper-leg
        const float cuffR1 = 0.30f;         // tiny outward taper

        // left cuff
        glPushMatrix();
        glTranslatef(-hipX, cuffY, 0.0f);
        glRotatef(-90, 1, 0, 0);
        GLUquadric* q1 = gluNewQuadric();
        gluQuadricNormals(q1, GLU_SMOOTH);
        matVest();
        gluCylinder(q1, cuffR0, cuffR1, cuffH, 32, 1); // no caps
        gluDeleteQuadric(q1);
        glPopMatrix();

        // right cuff
        glPushMatrix();
        glTranslatef(hipX, cuffY, 0.0f);
        glRotatef(-90, 1, 0, 0);
        GLUquadric* q2 = gluNewQuadric();
        gluQuadricNormals(q2, GLU_SMOOTH);
        matVest();
        gluCylinder(q2, cuffR0, cuffR1, cuffH, 32, 1); // no caps
        gluDeleteQuadric(q2);
        glPopMatrix();

        // small gold rings at cuff tops (purely decorative)
        matRope();
        glPushMatrix();
        glTranslatef(-hipX, cuffY + cuffH - 0.005f, 0.0f);
        glRotatef(90, 1, 0, 0);
        glutSolidTorus(0.012f, cuffR1 + 0.005f, 10, 36);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(hipX, cuffY + cuffH - 0.005f, 0.0f);
        glRotatef(90, 1, 0, 0);
        glutSolidTorus(0.012f, cuffR1 + 0.005f, 10, 36);
        glPopMatrix();
    }

    // ---------- (REMOVED) hem lip ----------
    // The thin hem cylinder was the “red circle”. It’s intentionally removed.
}
