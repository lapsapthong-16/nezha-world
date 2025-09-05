#include <GL/freeglut.h>
#include "utils.hpp"
#include "model.hpp"
#include "torso.hpp"
#include "head.hpp"
#include "arms.hpp"
#include "shorts.hpp"
#include "legs.hpp"
#include "cannon.hpp"

// Camera settings
double camDist = 8.0, camYaw = 25.0, camPitch = 15.0;

// Mouse control variables
bool mouseDown = false;
int lastMouseX = 0, lastMouseY = 0;

#define SHOW_HEAD 1

static void drawCharacter() {
    glPushMatrix();
    glTranslatef(0.0f, 0.55f, 0.0f);
    
    glPushMatrix();
    drawTorso();
    drawHipWrap();
#if SHOW_HEAD
    glPushMatrix();
    glTranslatef(0.0f, MS.headLift + 0.1f, 0.0f);
    glRotatef(-5.0f, 1, 0, 0);
    drawHeadUnit();
    glPopMatrix();
#endif
    glPopMatrix();

    glPushMatrix(); drawArmChain(true);  glPopMatrix();
    glPushMatrix(); drawArmChain(false); glPopMatrix();

    drawShorts();
    glPushMatrix(); drawLeg(true);  glPopMatrix();
    glPushMatrix(); drawLeg(false); glPopMatrix();
    
    // Draw RIGHT laser cannon on the right shoulder
    glPushMatrix();
    glTranslatef(0.65f, 1.05f, 0.0f);  // Position OUTSIDE and ON TOP of right shoulder
    glRotatef(25.0f, 0, 0, 1);         // Angle it outward
    glRotatef(cannonState.canonRot, 1, 0, 0);  // Elevation control (starts at 0)
    glScalef(0.05f, 0.05f, 0.05f);     // Slightly smaller scale
    glTranslatef(0, 0.0f, 0.0f);       // Center positioning
    drawLaserCannon();
    glPopMatrix();
    
    // Draw LEFT laser cannon on the left shoulder
    glPushMatrix();
    glTranslatef(-0.65f, 1.05f, 0.0f); // Position OUTSIDE and ON TOP of left shoulder
    glRotatef(-25.0f, 0, 0, 1);        // Angle it outward (opposite direction)
    glRotatef(cannonState.canonRot, 1, 0, 0);  // Same elevation control
    glScalef(0.05f, 0.05f, 0.05f);     // Same scale
    glTranslatef(0, 0.0f, 0.0f);       // Same positioning
    drawLaserCannon();
    glPopMatrix();
    
    // === RIGHT beam (match RIGHT cannon’s transform) ===
    glPushMatrix();
    glTranslatef(0.65f, 1.05f, 0.0f);   // SAME as cannon
    glRotatef(25.0f, 0, 0, 1);          // SAME roll
    glRotatef(cannonState.canonRot, 1, 0, 0); // SAME pitch
    glScalef(0.05f, 0.05f, 0.05f);      // SAME scale

    // Move the beam origin to the muzzle in MODEL units.
    // Your barrel tip is ~7.5 after a 1.25x scale in drawCannonBarrel().
    // 7.5 * 1.25 = 9.375, add a little to sit just in front of the tip:
    glTranslatef(0, 0, 9.5f);

    drawLaserBeam();
    glPopMatrix();


    // === LEFT beam (match LEFT cannon’s transform) ===
    glPushMatrix();
    glTranslatef(-0.65f, 1.05f, 0.0f);  // SAME as cannon
    glRotatef(-25.0f, 0, 0, 1);         // SAME roll (opposite sign)
    glRotatef(cannonState.canonRot, 1, 0, 0); // SAME pitch
    glScalef(0.05f, 0.05f, 0.05f);      // SAME scale

    glTranslatef(0, 0, 9.5f);           // to the muzzle
    drawLaserBeam();
    glPopMatrix();
    
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();

    // Update cannon animations
    updateCannonAnimation();
    updateShootingAnimation();

    // Calculate camera position
    const double cx = camDist * std::cos(deg2rad(camPitch)) * std::sin(deg2rad(camYaw));
    const double cy = camDist * std::sin(deg2rad(camPitch));
    const double cz = camDist * std::cos(deg2rad(camPitch)) * std::cos(deg2rad(camYaw));
    gluLookAt(cx, cy, cz, 0, 0.5f, 0, 0, 1, 0);

    // Set up lighting
    GLfloat lightPos[] = { 3.5f, 5.5f, 3.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    GLfloat lightPos2[] = { -3.5f, 3.0f, -3.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    
    GLfloat lightPos3[] = { 0.0f, -2.0f, 4.0f, 1.0f };
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos3);

    // Draw ground plane
    matGround();
    glPushMatrix(); 
    glTranslatef(0, -1.50f, 0); 
    glScalef(8, 0.05f, 8); 
    glutSolidCube(1.0f); 
    glPopMatrix();

    drawCharacter();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity();
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
    
    // Cannon controls
    case 'c': case 'C': toggleCannon(); break;  // Toggle cannon on/off
    case 'v': case 'V': fireCannon(); break;    // Fire laser
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDown = true;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            mouseDown = false;
        }
    }
}

void mouseMotion(int x, int y) {
    if (mouseDown) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;
        
        camYaw += deltaX * 0.5;
        camPitch -= deltaY * 0.5;
        
        if (camPitch > 89.0) camPitch = 89.0;
        if (camPitch < -89.0) camPitch = -89.0;
        
        lastMouseX = x;
        lastMouseY = y;
        
        glutPostRedisplay();
    }
}

void mouseWheel(int wheel, int direction, int x, int y) {
    if (direction > 0) {
        camDist -= 0.5;
        if (camDist < 3.0) camDist = 3.0;
    } else {
        camDist += 0.5;
        if (camDist > 20.0) camDist = 20.0;
    }
    glutPostRedisplay();
}

void idle() {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(960, 720);
    glutCreateWindow("BMCS2173 Character (modular)");

    // OpenGL setup
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glClearColor(Palette::CLEAR[0], Palette::CLEAR[1], Palette::CLEAR[2], Palette::CLEAR[3]);

    // Configure lighting
    const GLfloat ambient[] = { 0.85f, 0.85f, 0.85f, 1.0f };
    const GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    const GLfloat specular[] = { 0.60f, 0.60f, 0.60f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    
    const GLfloat ambient2[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat diffuse2[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);
    
    const GLfloat ambient3[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    const GLfloat diffuse3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse3);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutMouseWheelFunc(mouseWheel);
    glutIdleFunc(idle);
    
    glutMainLoop();
    return 0;
}
