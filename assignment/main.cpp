#include <GL/freeglut.h>
#include "utils.hpp"
#include "model.hpp"
#include "torso.hpp"
#include "head.hpp"
#include "arms.hpp"
#include "shorts.hpp"
#include "legs.hpp"
#include "belt.hpp"

// camera
double camDist = 8.0, camYaw = 25.0, camPitch = 15.0;
#define SHOW_HEAD 1

static void drawCharacter() {
    // Move entire character up to prevent feet from going under platform
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);  // Lift character up by 0.4 units
    
    glPushMatrix();
    drawTorso();
    drawHipWrap();
#if SHOW_HEAD
    glPushMatrix(); glTranslatef(0.0f, MS.headLift, 0.0f); drawHeadUnit(); glPopMatrix();
#endif
    //drawBraidedBelt();
    glPopMatrix();

    //glPushMatrix(); drawArmChain(true);  glPopMatrix();
    //glPushMatrix(); drawArmChain(false); glPopMatrix();

    drawShorts();
    glPushMatrix(); drawLeg(true);  glPopMatrix();
    glPushMatrix(); drawLeg(false); glPopMatrix();
    
    glPopMatrix();  // End character lift
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    const double cx = camDist * std::cos(deg2rad(camPitch)) * std::sin(deg2rad(camYaw));
    const double cy = camDist * std::sin(deg2rad(camPitch));
    const double cz = camDist * std::cos(deg2rad(camPitch)) * std::cos(deg2rad(camYaw));
    gluLookAt(cx, cy, cz, 0, 0.5f, 0, 0, 1, 0);

    // Main light
    GLfloat lightPos[] = { 3.5f, 5.5f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    // Additional lights for even illumination
    GLfloat lightPos2[] = { -3.5f, 3.0f, -3.0f, 1.0f };  // Opposite side
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    
    GLfloat lightPos3[] = { 0.0f, -2.0f, 4.0f, 1.0f };   // From below/front
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos3);

    matGround();
    glPushMatrix(); glTranslatef(0, -1.50f, 0); glScalef(8, 0.05f, 8); glutSolidCube(1.0f); glPopMatrix();

    drawCharacter();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(50.0, (float)w / h, 0.1, 100.0);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 27: case 'q': exit(0); break;
    case 'w': camPitch += 3.0; break;
    case 's': camPitch -= 3.0; break;
    case 'a': camYaw -= 3.0; break;
    case 'd': camYaw += 3.0; break;
    case 'z': camDist -= 0.3; if (camDist < 3) camDist = 3; break;
    case 'x': camDist += 0.3; break;
    default: break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(960, 720);
    glutCreateWindow("BMCS2173 Character (modular)");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glClearColor(Palette::CLEAR[0], Palette::CLEAR[1], Palette::CLEAR[2], Palette::CLEAR[3]);

    // SOLUTION 1: Increase ambient lighting significantly
    const GLfloat ambient[] = { 0.85f, 0.85f, 0.85f, 1.0f };  // Much brighter ambient
    const GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    const GLfloat specular[] = { 0.60f, 0.60f, 0.60f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // SOLUTION 2: Add additional lights for even better coverage
    // Enable more lights
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    
    // Light from opposite side
    const GLfloat ambient2[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat diffuse2[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);
    
    // Light from below/side
    const GLfloat ambient3[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    const GLfloat diffuse3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse3);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
