// main.cpp  (Multi-Byte build; no UNICODE strings)
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>

#include "utils.hpp"
#include "model.hpp"
#include "torso.hpp"
#include "head.hpp"
#include "arms.hpp"
#include "shorts.hpp"
#include "legs.hpp"
#include "cannon.hpp"
#include "customization.hpp"
#include "nezha_bg.hpp"   // <-- Nezha background

// Animations & extras
#include "animation.hpp"
#include "prayAnimation.hpp"
#include "flower.hpp"
#include "meditation.hpp"

// ===============================
// Controls UI (overlay + menu)
// ===============================
namespace ControlsUI {
    static bool show = true;        // visible by default
    static int  W = 960, H = 720;   // updated from reshape

    static void drawBitmapText(float x, float y, const char* s) {
        glRasterPos2f(x, y);
        for (const char* p = s; *p; ++p)
            glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *p);
    }

    static void drawOverlay() {
        if (!show) return;

        static const char* L[] = {
            "================ Controls ================",
            "Mouse drag: orbit     Mouse wheel: zoom",
            "W/S: pitch   A/D: yaw   Z/X: zoom",
            "",
            "Weapons/Cannon:",
            "  3: toggle weapon on/off    1: sword    2: staff",
            "  - / + : weapon length      C: toggle cannon    V: fire cannon",
            "",
            "Outfit (shirt texture):",
            "  O: cycle (Red/Orange/Blue/Gold)",
            "  R/T/U/Y: set Red/Orange/Blue/Gold",
            "",
            "Head:",
            "  T: spin left    G: spin right",
            "",
            "Arms:",
            "  Left- J: lift   U: lower   5: elbow bend   4: elbow other side",
            "  Right- I: lift  N: lower   6: elbow up     Q: elbow down",
            "",
            "Legs:",
            "  Left-  B: lift    F: lower",
            "  Right- ;: lift    [: lower",
            "",
            "Animations:",
            "  7: Fire Dragon Coil   8: Kung Fu Kick + Flower  9: Meditation",
            "",
            "P: print primitive counts    Right-click: quick menu    Esc/Q: quit",
            "=========================================="
        };
        const int N = int(sizeof(L) / sizeof(L[0]));

        glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 2D overlay
        glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
        gluOrtho2D(0, W, 0, H);
        glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();

        const float pad = 12.0f;
        const float lineH = 18.0f;
        const float w = 645.0f;
        const float h = pad * 2 + 22.0f + N * lineH;
        const float x0 = 10.0f;
        const float y1 = H - 10.0f;
        const float y0 = y1 - h;

        glColor4f(0.06f, 0.08f, 0.12f, 0.82f);
        glBegin(GL_QUADS);
        glVertex2f(x0, y0);
        glVertex2f(x0 + w, y0);
        glVertex2f(x0 + w, y1);
        glVertex2f(x0, y1);
        glEnd();

        glColor3f(1, 1, 1);
        drawBitmapText(x0 + pad, y1 - pad - 4, "Controls");
        float y = y1 - pad - 24.0f;
        for (int i = 0; i < N; ++i, y -= lineH)
            drawBitmapText(x0 + pad, y, L[i]);

        glMatrixMode(GL_MODELVIEW);  glPopMatrix();
        glMatrixMode(GL_PROJECTION); glPopMatrix();
        glPopAttrib();
    }

    static void onSpecial(int key) {
        if (key == GLUT_KEY_F1) { show = !show; glutPostRedisplay(); }
    }
    static void onReshape(int w, int h) { W = w; H = (h == 0 ? 1 : h); }

    // right-click menu
    static void onMenu(int id) {
        switch (id) {
        case 1: toggleCannon(); break;
        case 2: fireCannon(); break;
        case 3: handleCustomizationKey('3'); break; // toggle weapons on/off
        case 4: handleCustomizationKey('1'); break; // sword
        case 5: handleCustomizationKey('2'); break; // staff
        case 6: handleCustomizationKey('-'); break; // length -
        case 7: handleCustomizationKey('='); break; // length + (Shift+'=' is '+')
        case 8: handleCustomizationKey('O'); break; // cycle shirt color
        case 9: PolygonCounter::printToConsole(); break;
        }
        glutPostRedisplay();
    }
    static void initMenu() {
        int m = glutCreateMenu(onMenu);
        glutAddMenuEntry("Toggle cannon (C)", 1);
        glutAddMenuEntry("Fire cannon (V)", 2);
        glutAddMenuEntry("Toggle weapon (3)", 3);
        glutAddMenuEntry("Sword (1)", 4);
        glutAddMenuEntry("Staff (2)", 5);
        glutAddMenuEntry("Length - (-)", 6);
        glutAddMenuEntry("Length + (+)", 7);
        glutAddMenuEntry("Cycle shirt (O)", 8);
        glutAddMenuEntry("Print counts (P)", 9);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    }
}
static void ControlsUI_DrawOverlay() { ControlsUI::drawOverlay(); }
static void ControlsUI_OnSpecial(int k, int, int) { ControlsUI::onSpecial(k); }
static void ControlsUI_OnReshape(int w, int h) { ControlsUI::onReshape(w, h); }
static void ControlsUI_Init() { ControlsUI::initMenu(); }

// ===============================
// Camera & input
// ===============================
double camDist = 8.0, camYaw = 25.0, camPitch = 15.0;
bool mouseDown = false;
int lastMouseX = 0, lastMouseY = 0;

#define SHOW_HEAD 1

// ===============================
// Character draw
// ===============================
static void drawCharacter() {
    PolygonCounter::reset();
    PrimitiveCounter::reset();

    glPushMatrix();

    // Vertical placement + global idle motion
    glTranslatef(0.0f, 0.55f + animState.fireWheelHeight + kungFuKick.flyHeight, 0.0f);

    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glTranslatef(0.0f, animState.craneFlyHeight, 0.0f);
    }

    glTranslatef(0.0f, animState.idleBob, 0.0f);
    glRotatef(animState.idleSway, 0, 0, 1);

    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glTranslatef(animState.cranePelvisShift, 0.0f, 0.0f);
        glRotatef(animState.cranePelvisYaw, 0, 1, 0);
        glRotatef(animState.cranePelvisRoll, 0, 0, 1);
    }

    // GROUNDED PARTS
    drawHipWrap();

    // Cannons (grounded)
    // RIGHT
    glPushMatrix();
    glTranslatef(0.65f, 1.05f, 0.0f);
    glRotatef(25.0f, 0, 0, 1);
    glRotatef(cannonState.canonRot, 1, 0, 0);
    glScalef(0.05f, 0.05f, 0.05f);
    drawLaserCannon();
    glPopMatrix();

    // LEFT
    glPushMatrix();
    glTranslatef(-0.65f, 1.05f, 0.0f);
    glRotatef(-25.0f, 0, 0, 1);
    glRotatef(cannonState.canonRot, 1, 0, 0);
    glScalef(0.05f, 0.05f, 0.05f);
    drawLaserCannon();
    glPopMatrix();

    // RIGHT beam
    glPushMatrix();
    glTranslatef(0.65f, 1.05f, 0.0f);
    glRotatef(25.0f, 0, 0, 1);
    glRotatef(cannonState.canonRot, 1, 0, 0);
    glScalef(0.05f, 0.05f, 0.05f);
    glTranslatef(0, 0, 9.5f);
    drawLaserBeam();
    glPopMatrix();

    // LEFT beam
    glPushMatrix();
    glTranslatef(-0.65f, 1.05f, 0.0f);
    glRotatef(-25.0f, 0, 0, 1);
    glRotatef(cannonState.canonRot, 1, 0, 0);
    glScalef(0.05f, 0.05f, 0.05f);
    glTranslatef(0, 0, 9.5f);
    drawLaserBeam();
    glPopMatrix();

    // MEDITATION PARTS (pose changes only)
    glPushMatrix();

    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glRotatef(animState.craneSpineExtension, 1, 0, 0);
        glRotatef(animState.craneSpineSideBend, 0, 0, 1);
        glRotatef(animState.craneChestYaw, 0, 1, 0);
    }

    // Torso (with kung fu kick side-bend)
    glPushMatrix();
    if (kungFuKick.isActive) {
        glRotatef(kungFuKick.torsoYawDeg, 0, 1, 0);
        glRotatef(kungFuKick.torsoSideBendDeg, 0, 0, 1);
    }
    drawTorso();
    drawShorts();
    glPopMatrix();

#if SHOW_HEAD
    glPushMatrix();
    glTranslatef(0.0f, MS.headLift + 0.1f, 0.0f);

    if (animState.currentAnim == ANIM_CRANE_POSE) {
        glRotatef(animState.craneHeadYaw, 0, 1, 0);
        glRotatef(animState.craneHeadPitch, 1, 0, 0);
    }
    else {
        if (kungFuKick.isActive && (kungFuKick.torsoYawDeg > 0.0f || kungFuKick.headYawDeg > 0.0f)) {
            glRotatef(kungFuKick.headYawDeg, 0, 1, 0);
        }
        else {
            glRotatef(-5.0f + animState.headNod, 1, 0, 0);
        }
    }

    if (meditation.isActive) {
        glRotatef(meditation.headTilt, 0, 0, 1);
    }

    drawHeadUnit();
    glPopMatrix();
#endif

    glPopMatrix(); // end meditation parts matrix

    // Arms
    glPushMatrix();
    if (!(animState.currentAnim == ANIM_CRANE_POSE) && meditation.isActive) {
        glRotatef(meditation.armPose, 1, 0, 0);
    }
    else if (!(animState.currentAnim == ANIM_CRANE_POSE)) {
        glRotatef(animState.leftArmSwing, 1, 0, 0);
    }
    drawArmChain(true);
    glPopMatrix();

    glPushMatrix();
    if (!(animState.currentAnim == ANIM_CRANE_POSE) && meditation.isActive) {
        glRotatef(meditation.armPose, 1, 0, 0);
    }
    else if (!(animState.currentAnim == ANIM_CRANE_POSE)) {
        glRotatef(animState.rightArmSwing, 1, 0, 0);
    }
    drawArmChain(false);
    glPopMatrix();

    // Legs
    glPushMatrix();
    if (!(animState.currentAnim == ANIM_CRANE_POSE)) {
        glTranslatef(0, animState.leftLegLift, 0);
    }
    drawLeg(true);
    glPopMatrix();

    glPushMatrix();
    if (animState.currentAnim != ANIM_CRANE_POSE && !rightLegLiftAnim.isActive) {
        glTranslatef(0, animState.rightLegLift, 0);
    }
    drawLeg(false);
    glPopMatrix();

    glPopMatrix(); // character root

    // VFX outside character transform
    drawFireWheels();
    drawFireDragon();
}

// ===============================
// Display / reshape / input
// ===============================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- Background timing and draw (2D overlay) ---
    static int prevMs = 0;
    int curMs = glutGet(GLUT_ELAPSED_TIME);
    float dt = (prevMs == 0) ? 0.016f : (curMs - prevMs) * 0.001f;
    if (dt > 0.05f) dt = 0.05f;
    prevMs = curMs;

    updateNezhaBackground(dt);
    drawNezhaBackground();
    drawNezhaBackdropMountains();

    // IMPORTANT: restore MODELVIEW before 3D camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Animation updates
    updateCannonAnimation();
    updateShootingAnimation();
    updateAnimations();
    updateRightLegLiftAnimation();
    updateRightStraightLegLift();
    updateKungFuKickAnimation();
    updateFlowerBloomAnimation();
    updateMeditationAnimation();

    // Camera
    const double cx = camDist * std::cos(deg2rad(camPitch)) * std::sin(deg2rad(camYaw));
    const double cy = camDist * std::sin(deg2rad(camPitch));
    const double cz = camDist * std::cos(deg2rad(camPitch)) * std::cos(deg2rad(camYaw));
    gluLookAt(cx, cy, cz, 0, 0.5f, 0, 0, 1, 0);

    // Lights (includes extra spot + rim lights)
    GLfloat lightPos[] = { 3.5f, 5.5f, 3.0f, 1.0f };
    GLfloat lightPos2[] = { -3.5f, 3.0f, -3.0f, 1.0f };
    GLfloat lightPos3[] = { 0.0f, -2.0f, 4.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos2);
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos3);

    // Spot (LIGHT3) follows action a bit
    GLfloat spotPos[] = { 0.0f, 4.0f, 2.0f, 1.0f };
    GLfloat rimPos[] = { -2.0f, 1.0f, -2.0f, 1.0f };
    if (kungFuKick.isActive) {
        GLfloat kickLight[] = { 0.0f, 3.0f, 0.0f, 1.0f };
        GLfloat kickDiffuse[] = { 1.0f, 0.8f, 0.6f, 1.0f };
        glLightfv(GL_LIGHT3, GL_POSITION, kickLight);
        glLightfv(GL_LIGHT3, GL_DIFFUSE, kickDiffuse);
        glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 45.0f);
    }
    else {
        glLightfv(GL_LIGHT3, GL_POSITION, spotPos);
        GLfloat spotDiffuse[] = { 1.0f, 0.9f, 0.8f, 1.0f };
        glLightfv(GL_LIGHT3, GL_DIFFUSE, spotDiffuse);
        glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30.0f);
    }
    glLightfv(GL_LIGHT4, GL_POSITION, rimPos);

    // Ground
    matGround();
    glPushMatrix();
    glTranslatef(0, -1.50f, 0);
    glScalef(8, 0.05f, 8);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Character
    drawCharacter();

    // Flower/lotus/particles at feet
    drawFlowerBloomAt(0.0f, -1.50f + 0.02f, 0.0f);
    drawLotusPlatform(0.0f, -1.50f, 0.0f);
    drawMeditationParticles(0.0f, -1.50f, 0.0f);

    // In-game help
    ControlsUI_DrawOverlay();

    // Print polygon/primitive counts once
    static int frameCount = 0;
    if (++frameCount == 1) {
        PolygonCounter::printToConsole();
        PrimitiveCounter::printToConsole();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (float)w / h, 0.1, 100.0);

    ControlsUI_OnReshape(w, h);
}

void keyboard(unsigned char key, int, int) {
    // Most gameplay keys are centralized here:
    if (handleCustomizationKey(key)) return;

    switch (key) {
    case 27: case 'q': case 'Q': exit(0); break;
    case 'w': camPitch += 3.0; break;
    case 's': camPitch -= 3.0; break;
    case 'a': camYaw -= 3.0; break;
    case 'd': camYaw += 3.0; break;
    case 'z': camDist -= 0.3; if (camDist < 3.0)  camDist = 3.0;  break;
    case 'x': camDist += 0.3; if (camDist > 20.0) camDist = 20.0; break;

        // Redundant cannon shortcuts (menu also calls them)
    case 'c': case 'C': toggleCannon(); break;
    case 'v': case 'V': fireCannon();   break;

        // Animations
    case '7': triggerFireDragonCoilAnimation(); break;
    case '8': triggerKungFuKick(); triggerFlowerBloom(); break;
    case '9': triggerMeditation(); break;

        // Polygon count
    case 'p': case 'P': PolygonCounter::printToConsole(); PrimitiveCounter::printToConsole(); break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) { mouseDown = true; lastMouseX = x; lastMouseY = y; }
        else { mouseDown = false; }
    }
}

void mouseMotion(int x, int y) {
    if (!mouseDown) return;
    int dx = x - lastMouseX;
    int dy = y - lastMouseY;
    camYaw += dx * 0.5;
    camPitch -= dy * 0.5;
    if (camPitch > 89.0)  camPitch = 89.0;
    if (camPitch < -89.0) camPitch = -89.0;
    lastMouseX = x; lastMouseY = y;
    glutPostRedisplay();
}

void mouseWheel(int, int direction, int, int) {
    if (direction > 0) { camDist -= 0.5; if (camDist < 3.0) camDist = 3.0; }
    else { camDist += 0.5; if (camDist > 20.0) camDist = 20.0; }
    glutPostRedisplay();
}

void idle() { glutPostRedisplay(); }

// ===============================
// Main
// ===============================
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

    // Additional lights (key, fill, back, spot, rim)
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_LIGHT3);  // spot
    glEnable(GL_LIGHT4);  // rim

    // LIGHT0 key
    const GLfloat ambient[] = { 0.85f, 0.85f, 0.85f, 1.0f };
    const GLfloat diffuse[] = { 0.95f, 0.95f, 0.95f, 1.0f };
    const GLfloat specular[] = { 0.60f, 0.60f, 0.60f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // LIGHT1 fill
    const GLfloat ambient2[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat diffuse2[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient2);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse2);

    // LIGHT2 back
    const GLfloat ambient3[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    const GLfloat diffuse3[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient3);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse3);

    // LIGHT3 spot
    const GLfloat spotDiffuse[] = { 1.0f, 0.9f, 0.8f, 1.0f };
    const GLfloat spotSpecular[] = { 1.0f, 0.9f, 0.8f, 1.0f };
    const GLfloat spotDirection[] = { 0.0f, -1.0f, 0.0f };
    glLightfv(GL_LIGHT3, GL_DIFFUSE, spotDiffuse);
    glLightfv(GL_LIGHT3, GL_SPECULAR, spotSpecular);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 30.0f);
    glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 2.0f);
    glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spotDirection);

    // LIGHT4 rim
    const GLfloat rimDiffuse[] = { 0.3f, 0.3f, 0.5f, 1.0f };
    const GLfloat rimSpecular[] = { 0.1f, 0.1f, 0.2f, 1.0f };
    glLightfv(GL_LIGHT4, GL_DIFFUSE, rimDiffuse);
    glLightfv(GL_LIGHT4, GL_SPECULAR, rimSpecular);

    // Texturing init (Multi-Byte loader)
    startGDIplus();
    atexit(stopGDIplus);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Fur, belt, ribbons, weapon, bamboo, bronze
    gTex.pandaWhite = loadTexture2D("textures/panda_white_fur.bmp");
    gTex.pandaBlack = loadTexture2D("textures/panda_black_fur.bmp");
    gTex.goldBelt = loadTexture2D("textures/gold_belt.bmp");
    gTex.redSilk = loadTexture2D("textures/red_silk_ribbon.bmp");
    gTex.blade = loadTexture2D("textures/sword_blade.bmp");
    gTex.bamboo = loadTexture2D("textures/bamboo_boom.bmp");
    gTex.bronze = loadTexture2D("textures/bronze_bullet.bmp");

    // Shirt textures
    gTex.redShirt = loadTexture2D("textures/red_silk_shirt.bmp");
    gTex.orangeShirt = loadTexture2D("textures/orange_silk_shirt.bmp");
    gTex.blueShirt = loadTexture2D("textures/blue_silk_shirt.bmp");
    gTex.goldShirt = loadTexture2D("textures/gold_silk_shirt.bmp");

    // Optional extras
    gTex.dragon = loadTexture2D("textures/dragon_skin.bmp");
    gTex.lotus = loadTexture2D("textures/lotus_petal.bmp");
    gTex.cloud = loadTexture2D("textures/cloud_texture.bmp");

    // Pick a starting shirt (also sets sword/outfit color)
    setShirtStyle(SHIRT_RED);

    // Nezha background init
    initNezhaBackground(1337);

    // Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(ControlsUI_OnSpecial);   // F1 toggle for help
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutMouseWheelFunc(mouseWheel);
    glutIdleFunc(idle);

    // Right-click menu
    ControlsUI_Init();

    glutMainLoop();
    return 0;
}
