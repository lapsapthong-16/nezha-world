// weapon.cpp
#include "weapon.hpp"
#include "utils.hpp"          // <-- for gTex.blade
#include <GL/freeglut.h>

// ---------- tiny helpers ----------
static inline void setMat(const RGB& c) {
    const GLfloat amb[4] = { 0.20f * c.r, 0.20f * c.g, 0.20f * c.b, 1.0f };
    const GLfloat dif[4] = { c.r, c.g, c.b, 1.0f };
    const GLfloat spc[4] = { 0.30f, 0.30f, 0.30f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);
}
static inline void box(float sx, float sy, float sz) {
    glPushMatrix(); glScalef(sx, sy, sz); glutSolidCube(1.0f); glPopMatrix();
}

// ---- MATRIX-SAFE primitives (do not modify the caller's matrix) ----
static inline void cyl(float r, float h, int n = 24) {
    glPushMatrix();
    GLUquadric* q = gluNewQuadric(); gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, n, 1);
    // base cap @ z = 0
    glPushMatrix(); glRotatef(180, 1, 0, 0); gluDisk(q, 0, r, n, 1); glPopMatrix();
    // top cap  @ z = h
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0, r, n, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();
}
static inline void frustum(float r0, float r1, float h, int n = 24) {
    glPushMatrix();
    GLUquadric* q = gluNewQuadric(); gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r0, r1, h, n, 1);
    // base cap @ z = 0
    glPushMatrix(); glRotatef(180, 1, 0, 0); gluDisk(q, 0, r0, n, 1); glPopMatrix();
    // top cap  @ z = h
    glPushMatrix(); glTranslatef(0, 0, h); gluDisk(q, 0, r1, n, 1); glPopMatrix();
    gluDeleteQuadric(q);
    glPopMatrix();
}
static inline void torus(float tubeR, float ringR, int ns = 18, int rings = 28) {
    glutSolidTorus(tubeR, ringR, ns, rings);
}

// --- small helper to emit a triangle with a proper normal ---
static inline void triN(float ax, float ay, float az,
    float bx, float by, float bz,
    float cx, float cy, float cz) {
    const float ux = bx - ax, uy = by - ay, uz = bz - az;
    const float vx = cx - ax, vy = cy - ay, vz = cz - az;
    const float nx = uy * vz - uz * vy;
    const float ny = uz * vx - ux * vz;
    const float nz = ux * vy - uy * vx;
    glNormal3f(nx, ny, nz);
    glVertex3f(ax, ay, az); glVertex3f(bx, by, bz); glVertex3f(cx, cy, cz);
}

// ---------- color swatches ----------
static const RGB GOLD = { 0.95f, 0.80f, 0.25f };
static const RGB STEEL = { 0.82f, 0.86f, 0.95f };   // slightly bluish steel
static const RGB REDRIB = { 0.88f, 0.10f, 0.10f };

constexpr float EPS = 0.0035f;   // tiny overlap to hide seams

// ---------- Textured blade helpers ----------
static void beginBladeTexture() {
    if (gTex.blade == 0) return;
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTex.blade);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glColor3f(1, 1, 1); // no tint
    // a little spec so it still looks metallic under lights
    const GLfloat spec[] = { 0.40f, 0.40f, 0.40f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 48.0f);
}
static void endBladeTexture() {
    if (gTex.blade == 0) return;
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();
}

// Centered at origin, length along +Z/-Z (total `len`), with UVs:
// U runs along Z (blade length), V spans the cross-section.
static void drawBladeBoxTex(float w, float t, float len, float uRepeat = 6.0f) {
    const float hx = 0.5f * w, hy = 0.5f * t, hz = 0.5f * len;
    const float u0 = 0.0f, u1 = uRepeat;
    glBegin(GL_QUADS);
    // +Z (front)
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-hx, -hy, hz);
    glTexCoord2f(1, 0); glVertex3f(hx, -hy, hz);
    glTexCoord2f(1, 1); glVertex3f(hx, hy, hz);
    glTexCoord2f(0, 1); glVertex3f(-hx, hy, hz);
    // -Z (back)
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(hx, -hy, -hz);
    glTexCoord2f(1, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(1, 1); glVertex3f(-hx, hy, -hz);
    glTexCoord2f(0, 1); glVertex3f(hx, hy, -hz);

    // +X (right) — U along length, V across thickness
    glNormal3f(1, 0, 0);
    glTexCoord2f(u0, 0); glVertex3f(hx, -hy, -hz);
    glTexCoord2f(u1, 0); glVertex3f(hx, -hy, hz);
    glTexCoord2f(u1, 1); glVertex3f(hx, hy, hz);
    glTexCoord2f(u0, 1); glVertex3f(hx, hy, -hz);
    // -X (left)
    glNormal3f(-1, 0, 0);
    glTexCoord2f(u0, 0); glVertex3f(-hx, -hy, hz);
    glTexCoord2f(u1, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(u1, 1); glVertex3f(-hx, hy, -hz);
    glTexCoord2f(u0, 1); glVertex3f(-hx, hy, hz);

    // +Y (top) — U along length, V across width
    glNormal3f(0, 1, 0);
    glTexCoord2f(u0, 0); glVertex3f(-hx, hy, -hz);
    glTexCoord2f(u1, 0); glVertex3f(hx, hy, -hz);
    glTexCoord2f(u1, 1); glVertex3f(hx, hy, hz);
    glTexCoord2f(u0, 1); glVertex3f(-hx, hy, hz);
    // -Y (bottom)
    glNormal3f(0, -1, 0);
    glTexCoord2f(u0, 0); glVertex3f(hx, -hy, -hz);
    glTexCoord2f(u1, 0); glVertex3f(-hx, -hy, -hz);
    glTexCoord2f(u1, 1); glVertex3f(-hx, -hy, hz);
    glTexCoord2f(u0, 1); glVertex3f(hx, -hy, hz);
    glEnd();
}

// Simple textured tip (four triangles). U runs along tip length (0..uTip),
// V spans width/thickness (0..1). Base sits on z=0, tip points +Z.
static void drawBladeTipTex(float halfW, float halfT, float tipLen, float uTip = 1.2f) {
    const float x = halfW, y = halfT, z = 0.0f;
    const float tx = 0.0f, ty = 0.0f, tz = tipLen;

    glBegin(GL_TRIANGLES);
    // +X face
    glNormal3f(0, 0, 0);  // will be set by triN-style math below (approx via two calls)
    // We’ll just set UVs proportionally by Z and cross-section
    glNormal3f(0, 0, 0);  // OpenGL will normalize current normal from preceding faces
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, -y, z);
    glTexCoord2f(uTip, 0.5f); glVertex3f(tx, ty, tz);

    // -X face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, -y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, y, z);
    glTexCoord2f(uTip, 0.5f); glVertex3f(tx, ty, tz);

    // +Y face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-x, y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z);
    glTexCoord2f(uTip, 0.5f); glVertex3f(tx, ty, tz);

    // -Y face
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, -y, z);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-x, -y, z);
    glTexCoord2f(uTip, 0.5f); glVertex3f(tx, ty, tz);
    glEnd();
}

// ---------- Nezha Jian (weapon 1) — sword ----------
static void drawNezhaSword(const RGB& gripCol, float lenScale)
{
    const float kEPS = EPS;

    // +Z faces the blade tip
    const float handleR = 0.055f;
    const float handleH = 0.36f;
    const float guardZ = handleH;

    // ring guard
    const float ringR = 0.14f;
    const float ringTube = 0.018f;

    // blade
    const float bladeLen = 2.00f * lenScale;
    const float bladeW = 0.110f;
    const float bladeT = 0.025f;

    // handle stack
    const float ferruleLen = 0.032f;
    const float blueTaperOut = 1.020f;
    const float blueTaperIn = 0.970f;
    const float rampLenF = 0.022f;
    const float blueLen = handleH - rampLenF;
    const float sleeveR = handleR + 0.020f;
    const float sleeveLen = 0.180f;
    const float sleeveStartZ = guardZ - rampLenF;
    const float sleeveEndZ = sleeveStartZ + sleeveLen;

    // guard hub placement
    const float hubDepth = 0.070f;
    const float hubCenterZ = sleeveStartZ + 0.045f + 0.5f * hubDepth;

    // collar and root plug
    const float collarLen = 0.110f;
    const float collarW = 1.25f * bladeW;
    const float collarT = 2.20f * bladeT;
    const float rootPlugLen = 0.020f;

    // blade overlap to avoid gaps
    const float bladeBackOverlap = 0.024f;
    const float bladeZ0 = sleeveEndZ + collarLen - bladeBackOverlap;
    const float bladeLenTotal = bladeLen + bladeBackOverlap;

    glPushMatrix();

    // pommel + ribbons
    glPushMatrix();
    glTranslatef(0, 0, -0.065f);
    setMat(GOLD); glutSolidSphere(0.07f, 22, 16);
    glPushMatrix(); glRotatef(90, 1, 0, 0); torus(0.010f, 0.090f, 14, 24); glPopMatrix();
    setMat(REDRIB);
    glPushMatrix(); glRotatef(-25, 1, 0, 0); glRotatef(15, 0, 1, 0);
    glTranslatef(-0.03f, -0.11f, -0.01f); box(0.015f, 0.18f, 0.025f); glPopMatrix();
    glPushMatrix(); glRotatef(-15, 1, 0, 0); glRotatef(-10, 0, 1, 0);
    glTranslatef(0.03f, -0.13f, -0.02f);  box(0.015f, 0.20f, 0.025f); glPopMatrix();
    glPopMatrix();

    // rear ferrule
    setMat(GOLD);
    glPushMatrix();
    glTranslatef(0, 0, -0.5f * ferruleLen);
    cyl(handleR + 0.016f, ferruleLen + 2 * kEPS, 28);
    glPopMatrix();

    // blue grip (slight taper)
    glPushMatrix();
    setMat(gripCol);
    frustum(handleR * blueTaperOut, handleR * blueTaperIn, blueLen, 28);
    glPopMatrix();

    // front ramp + sleeve
    setMat(GOLD);
    glPushMatrix(); glTranslatef(0, 0, blueLen);
    frustum(handleR * blueTaperIn, sleeveR, rampLenF + kEPS, 28);
    glPopMatrix();

    glPushMatrix(); glTranslatef(0, 0, sleeveStartZ);
    cyl(sleeveR, sleeveLen + kEPS, 28);
    glPopMatrix();

    // ring guard
    glPushMatrix();
    glTranslatef(0, 0, hubCenterZ);
    torus(ringTube, ringR, 20, 28);
    for (int i = 0; i < 4; ++i) {
        glPushMatrix();
        glRotatef(90.0f * i, 0, 0, 1);
        glTranslatef(ringR, 0, 0);
        box(0.090f, 0.022f, hubDepth + 2 * kEPS);
        glPopMatrix();
    }
    glPopMatrix();

    // collar + steel root plug
    glPushMatrix();
    glTranslatef(0, 0, sleeveEndZ + 0.5f * collarLen);
    setMat(GOLD);
    box(collarW, collarT, collarLen + 2 * kEPS);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, sleeveEndZ + 0.5f * rootPlugLen);
    setMat(STEEL);
    box(1.02f * bladeW, 1.10f * bladeT, rootPlugLen + kEPS);
    glPopMatrix();

    // ====== TEXTURED BLADE (body + tip) ======
    if (gTex.blade != 0) {
        beginBladeTexture();

        // Rectangular blade body (centered at its segment)
        glPushMatrix();
        glTranslatef(0, 0, bladeZ0 + 0.5f * bladeLenTotal);
        drawBladeBoxTex(bladeW, bladeT, bladeLenTotal, /*uRepeat*/ 6.0f);
        glPopMatrix();

        // Optional: keep steel ridge for form
        setMat(STEEL);
        glPushMatrix();
        glTranslatef(0, 0, bladeZ0 + 0.5f * bladeLenTotal);
        glRotatef(45, 0, 0, 1);
        glScalef(0.026f, 0.026f, bladeLenTotal);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Textured tip
        glPushMatrix();
        glTranslatef(0, 0, bladeZ0 + bladeLenTotal);
        drawBladeTipTex(0.5f * bladeW, 0.5f * bladeT, 0.22f, /*uTip*/ 1.5f);
        glPopMatrix();

        endBladeTexture();
    }
    else {
        // Fallback (no texture loaded): steel blade as before
        setMat(STEEL);

        glPushMatrix();
        glTranslatef(0, 0, bladeZ0 + 0.5f * bladeLenTotal);
        glScalef(bladeW, bladeT, bladeLenTotal);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, bladeZ0 + 0.5f * bladeLenTotal);
        glRotatef(45, 0, 0, 1);
        glScalef(0.026f, 0.026f, bladeLenTotal);
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(0, 0, bladeZ0 + bladeLenTotal);
        // simple untextured tip
        glBegin(GL_TRIANGLES);
        const float x = 0.5f * bladeW, y = 0.5f * bladeT, z = 0.0f;
        const float tx = 0.0f, ty = 0.0f, tz = 0.22f;
        triN(x, y, z, x, -y, z, tx, ty, tz);
        triN(-x, -y, z, -x, y, z, tx, ty, tz);
        triN(-x, y, z, x, y, z, tx, ty, tz);
        triN(x, -y, z, -x, -y, z, tx, ty, tz);
        glEnd();
        glPopMatrix();
    }

    glPopMatrix();
}

// ---------- Huo Jian Qiang (weapon 2: staff/spear) ----------
static void drawHuoJianQiang(const RGB& /*shaft*/, float len)
{
    const float shaftR = 0.040f;
    const float halfLen = 0.95f * len;
    const float gripBand = 0.030f;

    const RGB GOLD = { 0.95f, 0.80f, 0.25f };
    const RGB STEEL = { 0.82f, 0.86f, 0.95f };

    glPushMatrix();

    // center grip collar
    setMat(GOLD);
    cyl(shaftR + 0.010f, gripBand, 28);

    // shaft forward/back (matrix-safe)
    setMat(STEEL);
    glPushMatrix(); glTranslatef(0, 0, gripBand); cyl(shaftR, halfLen - gripBand, 28); glPopMatrix();
    glPushMatrix(); glRotatef(180, 1, 0, 0);        cyl(shaftR, halfLen - gripBand, 28); glPopMatrix();

    // front head
    glPushMatrix();
    glTranslatef(0, 0, halfLen);

    setMat(GOLD); cyl(shaftR + 0.012f, 0.020f, 28);
    const float beadR = shaftR * 0.45f;
    glTranslatef(0, 0, 0.020f); setMat(STEEL); frustum(shaftR + 0.012f, beadR, 0.015f, 28);
    setMat(GOLD); glTranslatef(0, 0, 0.015f); glutSolidSphere(beadR, 20, 14);

    glTranslatef(0, 0, beadR * 0.85f);
    setMat(GOLD); box(0.18f, 0.035f, 0.06f);

    setMat(STEEL);
    glTranslatef(0, 0, 0.040f);
    glPushMatrix(); glRotatef(45, 0, 0, 1);
    // reuse untextured triangular tip for spear
    const float hx = 0.10f, hy = 0.06f;
    glBegin(GL_TRIANGLES);
    triN(hx, hy, 0, hx, -hy, 0, 0, 0, 0.70f);
    triN(-hx, -hy, 0, -hx, hy, 0, 0, 0, 0.70f);
    triN(-hx, hy, 0, hx, hy, 0, 0, 0, 0.70f);
    triN(hx, -hy, 0, -hx, -hy, 0, 0, 0, 0.70f);
    glEnd();
    glPopMatrix();

    glPopMatrix();

    // rear pommel
    glPushMatrix();
    glTranslatef(0, 0, -halfLen);

    setMat(GOLD); cyl(shaftR + 0.010f, 0.045f, 28);
    glTranslatef(0, 0, 0.045f);

    for (int i = 0; i < 3; ++i) {
        float br = 0.060f - 0.008f * i;
        float h = 0.100f;
        glutSolidCone(br, h, 22, 1);
        glTranslatef(0, 0, h * 0.65f);
        glRotatef(180, 1, 0, 0);
        glutSolidCone(br * 0.75f, h * 0.55f, 22, 1);
        glRotatef(180, 1, 0, 0);
        glTranslatef(0, 0, h * 0.45f);
    }
    glutSolidSphere(0.030f, 16, 12);

    glPopMatrix();

    glPopMatrix();
}

// ---------- attachment to right hand ----------
void drawWeaponInRightHand() {
    if (!gWeaponOn) return;

    glPushMatrix();
    if (gWeaponType == WEAPON_SWORD) {
        drawNezhaSword(gWeaponColor, gWeaponLenScale);   // weapon 1 (sword)
    }
    else {
        drawHuoJianQiang(gWeaponColor, gWeaponLenScale); // weapon 2 (staff/spear)
    }
    glPopMatrix();
}
