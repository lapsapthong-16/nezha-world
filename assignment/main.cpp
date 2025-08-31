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
#define SHOW_HEAD 0

static void drawCharacter() {
    glPushMatrix();
    drawTorso();
    drawHipWrap();
#if SHOW_HEAD
    glPushMatrix(); glTranslatef(0.0f, MS.headLift, 0.0f); drawHeadUnit(); glPopMatrix();
#endif
    drawBraidedBelt();
    glPopMatrix();

    glPushMatrix(); drawArmChain(true);  glPopMatrix();
    glPushMatrix(); drawArmChain(false); glPopMatrix();

    drawShorts();
    glPushMatrix(); drawLeg(true);  glPopMatrix();
    glPushMatrix(); drawLeg(false); glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();

    const double cx = camDist * std::cos(deg2rad(camPitch)) * std::sin(deg2rad(camYaw));
    const double cy = camDist * std::sin(deg2rad(camPitch));
    const double cz = camDist * std::cos(deg2rad(camPitch)) * std::cos(deg2rad(camYaw));
    gluLookAt(cx, cy, cz, 0, 0.5f, 0, 0, 1, 0);

    GLfloat lightPos[] = { 3.5f, 5.5f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

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

    const GLfloat ambient[] = { 0.28f, 0.28f, 0.28f, 1.0f };
    const GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    const GLfloat specular[] = { 0.60f, 0.60f, 0.60f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
