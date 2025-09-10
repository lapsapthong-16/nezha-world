#include "shorts.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawShorts() {
    // Set polygon counter to count shorts polygons
    PolygonCounter::setCurrentPart(BodyPart::SHORTS);
    
    const float hipX = MS.hipX;
    const float hipY = -0.98f;
    const float topY = hipY + 0.34f;
    const float hemY = hipY - 0.02f;
    const float depth = 0.18f, width = 0.95f;

    // Waistband
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, topY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* qb = gluNewQuadric();
    gluQuadricNormals(qb, GLU_SMOOTH);
    const float waistR = MS.torsoBotR * 0.95f;
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(qb, waistR, waistR, 0.05f, 44, 1);
    gluDeleteQuadric(qb);
    glPopMatrix();

    // Gold rope trim
    matRope();
    glPushMatrix();
    glTranslatef(0.0f, topY + 0.024f, 0.0f);
    glRotatef(90, 1, 0, 0);
    countGlutSolidTorus(0.014f, waistR + 0.004f, 12, 44);
    glPopMatrix();

    // Main shorts body
    matVest();
    const float trunkH = topY - hemY;
    const float midY = (topY + hemY) * 0.5f;

    // Front and back panels
    glPushMatrix(); 
    glTranslatef(0.0f, midY, depth * 0.98f);  
    glScalef(width * 0.82f, trunkH, 0.06f); 
    countGlutSolidCube(1.0f); 
    glPopMatrix();
    
    glPushMatrix(); 
    glTranslatef(0.0f, midY, -depth * 0.98f);  
    glScalef(width * 0.82f, trunkH, 0.06f); 
    countGlutSolidCube(1.0f); 
    glPopMatrix();

    // Side panels
    glPushMatrix(); 
    glTranslatef(-width * 0.46f, midY, 0.0f); 
    glRotatef(8, 0, 1, 0);  
    glScalef(0.08f, trunkH, depth * 1.70f); 
    countGlutSolidCube(1.0f); 
    glPopMatrix();
    
    glPushMatrix(); 
    glTranslatef(width * 0.46f, midY, 0.0f); 
    glRotatef(-8, 0, 1, 0); 
    glScalef(0.08f, trunkH, depth * 1.70f); 
    countGlutSolidCube(1.0f); 
    glPopMatrix();

    // Leg cuffs
    const float cuffY = hemY - 0.02f;
    const float cuffH = 0.30f;
    const float cuffRTop = 0.31f;
    const float cuffRBot = 0.36f;

    // Left cuff
    glPushMatrix();
    glTranslatef(-hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q1 = gluNewQuadric(); 
    gluQuadricNormals(q1, GLU_SMOOTH);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q1, cuffRTop, cuffRBot, cuffH, 40, 1);
    gluDeleteQuadric(q1);
    glPopMatrix();

    // Right cuff
    glPushMatrix();
    glTranslatef(hipX, cuffY, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q2 = gluNewQuadric(); 
    gluQuadricNormals(q2, GLU_SMOOTH);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    gluCylinder(q2, cuffRTop, cuffRBot, cuffH, 40, 1);
    gluDeleteQuadric(q2);
    glPopMatrix();

    // Cuff trim rings
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

    // Front bridge piece
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, hemY - 0.04f, 0.0f);
    glScalef(0.22f, 0.08f, 0.10f);
    countGlutSolidCube(1.0f);
    glPopMatrix();
}

