#include "nezha_bg.hpp"
#include <cmath>
#include <cstdlib>

NezhaBGState gNezhaBG;

struct Cloud { float x, y, scale, speed, phase; };
static const int kCloudCount = 12;
static Cloud gClouds[kCloudCount];
static int gSeed = 1337;

static float frand() {
    gSeed = (1103515245 * gSeed + 12345) & 0x7fffffff;
    return (gSeed / 2147483647.0f);
}

void initNezhaBackground(int seed) {
    gSeed = seed;
    for (int i = 0; i < kCloudCount; ++i) {
        gClouds[i].x = frand();
        gClouds[i].y = 0.55f + frand() * 0.35f;
        gClouds[i].scale = 0.06f + frand() * 0.08f;
        gClouds[i].speed = 0.005f + frand() * 0.02f;
        gClouds[i].phase = frand() * 100.0f;
    }
}

// ------------- 2D helpers -------------
static void begin2D() {
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}
static void end2D() {
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
    glMatrixMode(GL_MODELVIEW); // leave MODELVIEW active (important)
}

static void quadGradientVert(float x, float y, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glVertex2f(x, y);
}

static void drawDisc(float cx, float cy, float r, float aCenter, float aEdge) {
    const int seg = 48;
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 1.0f, aCenter);
    glVertex2f(cx, cy);
    glColor4f(1.0f, 1.0f, 1.0f, aEdge);
    for (int i = 0; i <= seg; ++i) {
        float t = (float)i / (float)seg * 6.2831853f;
        glVertex2f(cx + std::cos(t) * r, cy + std::sin(t) * r);
    }
    glEnd();
}

static void drawRibbonArc(float cx, float cy, float r0, float r1, float a0, float a1,
    float amp, float freq, float alpha, float rr, float gg, float bb) {
    const int seg = 96;
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= seg; ++i) {
        float u = (float)i / (float)seg;
        float a = a0 + (a1 - a0) * u;
        float wobble = std::sin(a * freq) * amp;
        float rInner = r0 + wobble;
        float rOuter = r1 + wobble;
        float cs = std::cos(a), sn = std::sin(a);
        glColor4f(rr, gg, bb, alpha * (0.6f + 0.4f * (1.0f - u)));
        glVertex2f(cx + cs * rOuter, cy + sn * rOuter);
        glColor4f(rr, gg, bb, alpha * (0.4f + 0.6f * u));
        glVertex2f(cx + cs * rInner, cy + sn * rInner);
    }
    glEnd();
}

// ------------- elements -------------
static void drawSkyGradient() {
    float topR = 0.08f, topG = 0.07f, topB = 0.16f;
    float midR = 0.35f, midG = 0.10f, midB = 0.32f;
    float botR = 0.95f, botG = 0.42f, botB = 0.12f;

    glBegin(GL_QUADS);
    quadGradientVert(0, 1, topR, topG, topB, 1.0f);
    quadGradientVert(1, 1, topR, topG, topB, 1.0f);
    quadGradientVert(1, 0.55f, midR, midG, midB, 1.0f);
    quadGradientVert(0, 0.55f, midR, midG, midB, 1.0f);

    quadGradientVert(0, 0.55f, midR, midG, midB, 1.0f);
    quadGradientVert(1, 0.55f, midR, midG, midB, 1.0f);
    quadGradientVert(1, 0, botR, botG, botB, 1.0f);
    quadGradientVert(0, 0, botR, botG, botB, 1.0f);
    glEnd();
}
static void drawSunGlow() {
    float cx = 0.72f, cy = 0.78f;
    drawDisc(cx, cy, 0.18f, 0.85f, 0.0f);
    drawDisc(cx, cy, 0.32f, 0.25f, 0.0f);
}
static void drawClouds() {
    for (int i = 0; i < kCloudCount; ++i) {
        float x = gClouds[i].x + gNezhaBG.time * gClouds[i].speed;
        x = std::fmod(x, 1.3f) - 0.15f;
        float y = gClouds[i].y + 0.01f * std::sin(gNezhaBG.time * 0.3f + gClouds[i].phase);
        float s = gClouds[i].scale;
        drawDisc(x, y, s * 0.9f, 0.12f, 0.0f);
        drawDisc(x - s * 0.5f, y, s * 0.7f, 0.12f, 0.0f);
        drawDisc(x + s * 0.55f, y, s * 0.75f, 0.12f, 0.0f);
        drawDisc(x - s * 0.15f, y + s * 0.35f, s * 0.65f, 0.12f, 0.0f);
        drawDisc(x + s * 0.25f, y + s * 0.30f, s * 0.6f, 0.12f, 0.0f);
    }
}
static void drawLotusHaloAndRibbons() {
    const float cx = 0.5f, cy = 0.58f;
    const int petals = 18;
    for (int i = 0; i < petals; ++i) {
        float a = (6.2831853f * i) / petals;
        float r = 0.10f + 0.02f * std::sin(gNezhaBG.time * 0.8f + i);
        glPushMatrix();
        glTranslatef(cx, cy, 0);
        glRotatef(a * 57.29578f, 0, 0, 1);
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 0.9f, 0.6f, 0.14f);
        glVertex2f(0, 0);
        glColor4f(1.0f, 0.8f, 0.3f, 0.0f);
        for (int k = 0; k <= 18; ++k) {
            float t = (float)k / 18.0f * 3.14159f;
            float ex = std::cos(t) * r;
            float ey = std::sin(t) * r * 0.55f;
            glVertex2f(ex, ey);
        }
        glEnd();
        glPopMatrix();
    }
    float a0 = 190.0f * 3.14159f / 180.0f;
    float a1 = 350.0f * 3.14159f / 180.0f;
    drawRibbonArc(cx, cy, 0.22f, 0.27f, a0, a1, 0.01f, 6.0f, 0.55f, 0.95f, 0.15f, 0.10f);
    drawRibbonArc(cx, cy, 0.28f, 0.33f, a0 + 0.12f, a1 + 0.12f, 0.012f, 7.5f, 0.35f, 0.95f, 0.15f, 0.10f);
}
static void drawEmbers() {
    const int count = 60;
    glEnable(GL_POINT_SMOOTH);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < count; ++i) {
        float rx = frand();
        float ry = frand() * 0.5f + 0.05f;
        float t = gNezhaBG.time * 0.6f + i;
        float x = std::fmod(rx + 0.02f * std::sin(t + i), 1.0f);
        float y = ry + 0.05f * std::sin(t * 0.7f + i);
        glColor4f(1.0f, 0.6f, 0.2f, 0.65f * (0.5f + 0.5f * std::sin(t + i)));
        glVertex2f(x, y);
    }
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}
static void drawMountainsLayer(float y0, float h, float r, float g, float b, float alpha, float speed) {
    glColor4f(r, g, b, alpha);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 200; ++i) {
        float u = (float)i / 200.0f;
        float w = gNezhaBG.time * speed;
        float noise = std::sin((u + w) * 6.0f) * 0.03f
            + std::sin((u * 2.0f + w * 1.3f) * 7.0f) * 0.02f;
        float y = y0 + noise * h;
        glVertex2f(u, 0.0f);
        glVertex2f(u, y);
    }
    glEnd();
}

// ------------- API -------------
void updateNezhaBackground(float dt) { if (gNezhaBG.enabled) gNezhaBG.time += dt; }

void drawNezhaBackground() {
    if (!gNezhaBG.enabled) return;
    begin2D();
    drawSkyGradient();
    drawSunGlow();
    drawClouds();
    drawLotusHaloAndRibbons();
    drawEmbers();
    end2D();
}

void drawNezhaBackdropMountains() {
    if (!gNezhaBG.enabled) return;
    begin2D();
    drawMountainsLayer(0.25f, 0.08f, 0.10f, 0.08f, 0.16f, 0.35f, 0.02f);
    drawMountainsLayer(0.20f, 0.09f, 0.14f, 0.09f, 0.18f, 0.45f, 0.04f);
    drawMountainsLayer(0.16f, 0.11f, 0.18f, 0.10f, 0.20f, 0.60f, 0.06f);
    end2D();
}
