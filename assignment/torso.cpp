#include "torso.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

// ---------------- helpers ----------------
static void drawGoldEdgeAtDeg(float deg)
{
    // follows the tapered vest rim exactly
    const float a = deg2rad(deg);
    const float cs = cosf(a), sn = sinf(a);

    // match the vest taper you use for drawOpenCylinderY(...)
    const float rBot = MS.torsoBotR * 0.95f;
    const float rTop = MS.torsoTopR * 0.92f;

    // push slightly outward so it never z-fights the vest shell
    const float eps = 0.010f;

    // visual thickness across the opening
    const float t = 0.050f;

    // keep a little margin from top/bottom of the vest
    const float y0 = -MS.torsoH * 0.5f + 0.015f;
    const float y1 = MS.torsoH * 0.5f - 0.015f;

    // tangent in XZ (perpendicular to rim direction)
    const float tx = -sn, tz = cs;

    // bottom inner/outer
    const float xBi = (rBot + eps) * cs + (t * -0.5f) * tx;
    const float zBi = (rBot + eps) * sn + (t * -0.5f) * tz;
    const float xBo = (rBot + eps) * cs + (t * 0.5f) * tx;
    const float zBo = (rBot + eps) * sn + (t * 0.5f) * tz;

    // top inner/outer (uses rTop so the strip matches the taper)
    const float xTi = (rTop + eps) * cs + (t * -0.5f) * tx;
    const float zTi = (rTop + eps) * sn + (t * -0.5f) * tz;
    const float xTo = (rTop + eps) * cs + (t * 0.5f) * tx;
    const float zTo = (rTop + eps) * sn + (t * 0.5f) * tz;

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);   // avoid disappearing when viewed edge-on

    matRope();
    glBegin(GL_QUADS);
    glNormal3f(cs, 0.0f, sn);           // outward-ish normal
    glVertex3f(xBi, y0, zBi);
    glVertex3f(xBo, y0, zBo);
    glVertex3f(xTo, y1, zTo);
    glVertex3f(xTi, y1, zTi);
    glEnd();

    if (wasCull) glEnable(GL_CULL_FACE);
}

// NATURAL Panda-style torso with curved transition like real pandas
static void drawPandaTorsoCore() {
    // More fitted to body - closer to skin
    const float rBot = MS.torsoBotR * 0.88f;  
    const float rTop = MS.torsoTopR * 0.85f;  
    const float h = MS.torsoH + 0.08f;  // Make torso slightly taller so white belly reaches vest top
    
    // GLU cylinders are along +Z; rotate to stand up on Y and center on origin
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, -h * 0.5f);

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    
    // Natural curved panda pattern with angular segments for realistic look
    const int numSlices = 25;  // More slices for smoother transition
    const float sliceHeight = h / numSlices;
    
    for (int i = 0; i < numSlices; i++) {
        float t = (float)i / (float)(numSlices - 1);  // 0.0 to 1.0 from bottom to top
        float nextT = (float)(i + 1) / (float)(numSlices - 1);
        
        // Calculate radii at this height
        float currentR = rBot + (rTop - rBot) * t;
        float nextR = rBot + (rTop - rBot) * nextT;
        
        // Draw this slice with multiple angular segments for natural panda pattern
        const int numSegments = 32;
        const float angleStep = 360.0f / numSegments;
        
        for (int seg = 0; seg < numSegments; seg++) {
            float angle = seg * angleStep;
            float nextAngle = (seg + 1) * angleStep;
            
            // Calculate radii at this height
            float currentR = rBot + (rTop - rBot) * t;
            float nextR = rBot + (rTop - rBot) * nextT;
            
            // Natural panda pattern: curved white belly, black on sides and top
            bool isBlack = false;
            
            // Top part (shoulders/chest) - mostly black above 70%
            if (t > 0.7f) {
                isBlack = true;
            }
            // Middle transition zone - create curved natural boundaries
            else if (t > 0.35f && t <= 0.7f) {
                // Create a natural curved white belly area in the front-center
                float frontAngle = fmod(angle + 180.0f, 360.0f); // Normalize so front = 0
                if (frontAngle > 180.0f) frontAngle = 360.0f - frontAngle;
                
                // White belly extends from front center, with natural curve
                float heightFactor = (0.7f - t) / 0.35f; // 1.0 at bottom of transition, 0.0 at top
                float bellyWidth = 65.0f + 25.0f * heightFactor; // Wider at bottom, narrower at top
                
                // Add natural curve - belly is wider in middle heights
                float curveFactor = 4.0f * t * (1.0f - t); // Bell curve peaks at t=0.5
                bellyWidth += 15.0f * curveFactor;
                
                if (frontAngle <= bellyWidth) {
                    isBlack = false; // White belly
                } else {
                    isBlack = true;  // Black sides
                }
            }
            // Bottom part - mostly white belly
            else {
                isBlack = false;
            }
            
            // Set material - using the pure white like the head
            if (isBlack) {
                matBlackFur();
            } else {
                matPandaWhite();  // Pure white like the head
            }
            
            // Draw segment
            glPushMatrix();
            glTranslatef(0, 0, i * sliceHeight);
            if (i < numSlices - 1) {
                float a1 = deg2rad(angle);
                float a2 = deg2rad(nextAngle);
                
                // Draw a quad segment
                glBegin(GL_QUADS);
                // Bottom edge
                glNormal3f(cosf(a1), 0, sinf(a1));
                glVertex3f(currentR * cosf(a1), currentR * sinf(a1), 0);
                glNormal3f(cosf(a2), 0, sinf(a2));
                glVertex3f(currentR * cosf(a2), currentR * sinf(a2), 0);
                // Top edge
                glNormal3f(cosf(a2), 0, sinf(a2));
                glVertex3f(nextR * cosf(a2), nextR * sinf(a2), sliceHeight);
                glNormal3f(cosf(a1), 0, sinf(a1));
                glVertex3f(nextR * cosf(a1), nextR * sinf(a1), sliceHeight);
                glEnd();
            }
            glPopMatrix();
        }
    }
    
    // Bottom cap - white belly
    matPandaWhite();
    gluDisk(q, 0.0f, rBot, 32, 1);
    
    // Top cap - black shoulders
    matBlackFur();
    glPushMatrix();
    glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 32, 1);
    glPopMatrix();

    gluDeleteQuadric(q);
    if (wasCull) glEnable(GL_CULL_FACE);
    glPopMatrix();
}

// Nezha-style vest collar/yoke - more traditional Chinese style
static void drawVestTopYoke() {
    matVest();
    const float yTop = MS.torsoH * 0.5f;
    const float rOuter = MS.torsoTopR + 0.002f;
    const float rInner = 0.38f; // Slightly smaller neck opening

    glPushMatrix();
    glTranslatef(0.0f, yTop, 0.0f);
    glRotatef(-90, 1, 0, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, rInner, rOuter, 48, 1);
    gluDeleteQuadric(q);
    glPopMatrix();
}

// Redesigned front panels for Nezha - more traditional Chinese vest style
static void drawFrontPanels() {
    const float panelY = 0.1f;   // Slightly higher
    const float panelZ = 0.32f;  // More forward
    const float flare = 8.0f;    // Less flare for cleaner look

    // LEFT panel - more fitted
    matVest();
    glPushMatrix();
    glTranslatef(-0.25f, panelY, panelZ);
    glRotatef(flare, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.85f, 0.04f); // Slightly narrower and thinner
    glutSolidCube(1.0f);
    glPopMatrix();

    // RIGHT panel
    glPushMatrix();
    glTranslatef(0.25f, panelY, panelZ);
    glRotatef(-flare, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.85f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Add center chest detail - traditional Chinese vest often has center decoration
    matRope(); // Gold accent
    glPushMatrix();
    glTranslatef(0.0f, 0.2f, 0.34f);
    glScalef(0.06f, 0.25f, 0.02f);
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Add traditional Chinese-style side panels
static void drawSidePanels() {
    matVest();
    
    // Left side panel
    glPushMatrix();
    glTranslatef(-0.55f, 0.0f, 0.0f);
    glRotatef(15, 0, 1, 0); // Slight curve following body
    glScalef(0.15f, MS.torsoH * 0.75f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
    
    // Right side panel
    glPushMatrix();
    glTranslatef(0.55f, 0.0f, 0.0f);
    glRotatef(-15, 0, 1, 0);
    glScalef(0.15f, MS.torsoH * 0.75f, 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();
}
// --- fills the underside of the vest so you can't see the white interior ---
static void drawWaistSeal()
{
    // A very short capped cylinder that sits just inside the vest hem
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();                                  // same color as the vest
    glPushMatrix();
    glTranslatef(0.0f, -0.690f, 0.0f);          // just under the vest�s bottom
    glRotatef(-90, 1, 0, 0);

    const float rTop = MS.torsoBotR * 0.915f;   // slightly inside the shell
    const float rBot = MS.torsoBotR * 0.920f;   // tiny taper looks nicer
    const float h = 0.16f;                   // short, enough to block views

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, rBot, rTop, h, 44, 1);       // wall
    gluDisk(q, 0.0f, rBot, 44, 1);              // bottom cap
    glPushMatrix(); glTranslatef(0, 0, h);
    gluDisk(q, 0.0f, rTop, 44, 1);              // top cap
    glPopMatrix();
    gluDeleteQuadric(q);

    glPopMatrix();
    if (wasCull) glEnable(GL_CULL_FACE);
}
// ---------------- public ----------------
void drawTorso() {
    // Opening width and a tiny yaw so the shell isn't perfectly straight-on
    constexpr float VEST_GAP_DEG = 75.0f;
    const float edgeL = 90.0f + VEST_GAP_DEG * 0.5f;
    const float edgeR = 90.0f - VEST_GAP_DEG * 0.5f;
    const float yawDeg = -8.0f;                 // keep small; used for shell & gold

    // --- Vest outer shell (tapered open cylinder) ---
    matVest();
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawOpenCylinderY(
        MS.torsoBotR * 0.95f,                   // bottom radius
        MS.torsoTopR * 0.92f,                   // top radius (smaller -> taper)
        MS.torsoH,                              // height
        edgeL,                                  // start (left rim)
        360.0f - VEST_GAP_DEG,                  // sweep around to right rim
        64);
    glPopMatrix();

    // --- CHANGED: Use panda-style torso core instead of regular one ---
    drawPandaTorsoCore();  // This replaces drawTorsoCore(/*asSkin=*/true);

    // --- Close the top (yoke) ---
    drawVestTopYoke();

    drawWaistSeal();

    // --- Side panels: thinner/closer so they don't protrude at the front ---
    matVest();
    // left
    glPushMatrix();
    glTranslatef(-0.42f, 0.0f, 0.05f);
    glRotatef(14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // right
    glPushMatrix();
    glTranslatef(0.42f, 0.0f, 0.05f);
    glRotatef(-14.0f, 0, 1, 0);
    glScalef(0.12f, MS.torsoH * 0.74f, 0.16f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Front panels: a touch narrower so the chest reads clearly ---
    // left
    glPushMatrix();
    glTranslatef(-0.20f, 0.10f, 0.37f);
    glRotatef(4.0f, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.84f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // right
    glPushMatrix();
    glTranslatef(0.20f, 0.10f, 0.37f);
    glRotatef(-4.0f, 0, 1, 0);
    glScalef(0.28f, MS.torsoH * 0.84f, 0.04f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Waist band (slightly thinner/raised to avoid intersections) ---
    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.675f, 0.0f);
    glScalef(1.12f, 0.20f, 0.54f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // --- Gold edges: draw LAST, with depth test off, inside same yaw as shell ---
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);                  // ensures both trims are visible
    glPushMatrix();
    glRotatef(yawDeg, 0, 1, 0);
    drawGoldEdgeAtDeg(edgeL);                  // left rim
    drawGoldEdgeAtDeg(edgeR);                  // right rim
    glPopMatrix();
    glPopAttrib();
}

void drawHipWrap() {

    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matVest();
    glPushMatrix();
    glTranslatef(0.0f, -0.74f, 0.0f); 
    glRotatef(-90, 1, 0, 0);

    const float h = 0.12f;              // very short
    const float r = MS.torsoBotR * 0.90f;

    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);

    gluCylinder(q, r, r, h, 44, 1);     // wall only � no top/bottom disks

    gluDeleteQuadric(q);
    glPopMatrix();

    if (wasCull) glEnable(GL_CULL_FACE);
}


