#include "dragonHead.hpp"
#include "utils.hpp"
#include <cmath>
#include <GL/freeglut.h>

// Global dragon head state
DragonHeadState dragonHead;

void updateDragonHeadAnimation() {
    if (!dragonHead.isActive) return;

    // Simple animation - head appears and features grow in
    static float animTime = 0.0f;
    static bool needsReset = false;

    // Reset timer if needed
    if (needsReset) {
        animTime = 0.0f;
        needsReset = false;
    }

    animTime += 0.016f; // ~60 FPS

    if (animTime < 1.0f) {
        float phase = animTime / 1.0f;
        dragonHead.headY = phase * 1.2f;
        dragonHead.scale = phase * 0.8f;
        dragonHead.featureSize = phase * 1.2f;
        dragonHead.hornSize = phase * 1.2f;
        dragonHead.teethSize = phase * 1.2f;

        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 2.5f) {
        float phase2 = (animTime - 1.0f) / 1.5f;

        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;

        dragonHead.bodyProgress = phase2;
        dragonHead.spiralAngle = phase2 * 6.0f * (float)M_PI;
        dragonHead.bodyScale = phase2 * 1.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 4.0f) {
        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;
        dragonHead.bodyProgress = 1.0f;
        dragonHead.spiralAngle = 6.0f * (float)M_PI;
        dragonHead.bodyScale = 1.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 5.5f) {
        float phase3 = (animTime - 4.0f) / 1.5f;

        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;
        dragonHead.bodyProgress = 1.0f;
        dragonHead.spiralAngle = 6.0f * (float)M_PI;
        dragonHead.bodyScale = 1.0f;

        dragonHead.isBreathingFire = true;
        dragonHead.fireIntensity = phase3;
        dragonHead.fireParticleCount = phase3 * 50.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 6.0f) {
        float retractionPhase = (animTime - 5.5f) / 0.5f;

        dragonHead.isRetracting = true;
        dragonHead.retractionProgress = retractionPhase;
        dragonHead.headShrinkProgress = 0.0f;

        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;

        dragonHead.bodyProgress = 1.0f - retractionPhase;
        dragonHead.spiralAngle = 6.0f * (float)M_PI * (1.0f - retractionPhase);
        dragonHead.bodyScale = 1.0f - retractionPhase * 0.5f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
    }
    else if (animTime < 6.5f) {
        float shrinkPhase = (animTime - 6.0f) / 0.5f;

        dragonHead.isRetracting = true;
        dragonHead.retractionProgress = 1.0f;
        dragonHead.headShrinkProgress = shrinkPhase;

        dragonHead.headY = 1.2f;
        dragonHead.finalHeadScale = 0.8f * (1.0f - shrinkPhase);
        dragonHead.scale = dragonHead.finalHeadScale;
        dragonHead.featureSize = 1.0f * (1.0f - shrinkPhase);
        dragonHead.hornSize = 1.0f * (1.0f - shrinkPhase);
        dragonHead.teethSize = 1.0f * (1.0f - shrinkPhase);

        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
    }
    else {
        dragonHead.isActive = false;
        dragonHead.headY = 0.0f;
        dragonHead.scale = 0.0f;
        dragonHead.featureSize = 0.0f;
        dragonHead.hornSize = 0.0f;
        dragonHead.teethSize = 0.0f;
        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;
        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
}

void updateDragonHeadAnimationWithTime(float animTime) {
    if (!dragonHead.isActive) return;

    if (animTime < 1.0f) {
        float phase = animTime / 1.0f;
        dragonHead.headY = phase * 1.2f;
        dragonHead.scale = phase * 0.8f;
        dragonHead.featureSize = phase * 1.2f;
        dragonHead.hornSize = phase * 1.2f;
        dragonHead.teethSize = phase * 1.2f;

        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 2.5f) {
        float phase2 = (animTime - 1.0f) / 1.5f;

        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;

        dragonHead.bodyProgress = phase2;
        dragonHead.spiralAngle = phase2 * 6.0f * (float)M_PI;
        dragonHead.bodyScale = phase2 * 1.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 4.0f) {
        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;
        dragonHead.bodyProgress = 1.0f;
        dragonHead.spiralAngle = 6.0f * (float)M_PI;
        dragonHead.bodyScale = 1.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 5.5f) {
        float phase3 = (animTime - 4.0f) / 1.5f;

        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;
        dragonHead.bodyProgress = 1.0f;
        dragonHead.spiralAngle = 6.0f * (float)M_PI;
        dragonHead.bodyScale = 1.0f;

        dragonHead.isBreathingFire = true;
        dragonHead.fireIntensity = phase3;
        dragonHead.fireParticleCount = phase3 * 50.0f;

        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
    else if (animTime < 6.0f) {
        float retractionPhase = (animTime - 5.5f) / 0.5f;

        dragonHead.isRetracting = true;
        dragonHead.retractionProgress = retractionPhase;
        dragonHead.headShrinkProgress = 0.0f;

        dragonHead.headY = 1.2f;
        dragonHead.scale = 0.8f;
        dragonHead.featureSize = 1.0f;
        dragonHead.hornSize = 1.0f;
        dragonHead.teethSize = 1.0f;

        dragonHead.bodyProgress = 1.0f - retractionPhase;
        dragonHead.spiralAngle = 6.0f * (float)M_PI * (1.0f - retractionPhase);
        dragonHead.bodyScale = 1.0f - retractionPhase * 0.5f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
    }
    else if (animTime < 6.5f) {
        float shrinkPhase = (animTime - 6.0f) / 0.5f;

        dragonHead.isRetracting = true;
        dragonHead.retractionProgress = 1.0f;
        dragonHead.headShrinkProgress = shrinkPhase;

        dragonHead.headY = 1.2f;
        dragonHead.finalHeadScale = 0.8f * (1.0f - shrinkPhase * 0.95f);
        dragonHead.scale = dragonHead.finalHeadScale;
        dragonHead.featureSize = 1.0f * (1.0f - shrinkPhase);
        dragonHead.hornSize = 1.0f * (1.0f - shrinkPhase);
        dragonHead.teethSize = 1.0f * (1.0f - shrinkPhase);

        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;

        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
    }
    else {
        dragonHead.isActive = false;
        dragonHead.headY = 0.0f;
        dragonHead.scale = 0.0f;
        dragonHead.featureSize = 0.0f;
        dragonHead.hornSize = 0.0f;
        dragonHead.teethSize = 0.0f;
        dragonHead.bodyProgress = 0.0f;
        dragonHead.spiralAngle = 0.0f;
        dragonHead.bodyScale = 0.0f;
        dragonHead.isBreathingFire = false;
        dragonHead.fireIntensity = 0.0f;
        dragonHead.fireParticleCount = 0.0f;
        dragonHead.isRetracting = false;
        dragonHead.retractionProgress = 0.0f;
        dragonHead.headShrinkProgress = 0.0f;
        dragonHead.finalHeadScale = 0.0f;
    }
}

void triggerDragonHead() {
    dragonHead.isActive = true;
    dragonHead.headY = 0.0f;
    dragonHead.scale = 0.0f;
    dragonHead.featureSize = 0.1f;
    dragonHead.hornSize = 0.1f;
    dragonHead.teethSize = 0.1f;
    dragonHead.headTilt = 0.0f;

    dragonHead.bodyProgress = 0.0f;
    dragonHead.spiralAngle = 0.0f;
    dragonHead.bodyScale = 0.0f;

    dragonHead.isBreathingFire = false;
    dragonHead.fireIntensity = 0.0f;
    dragonHead.fireParticleCount = 0.0f;

    dragonHead.isRetracting = false;
    dragonHead.retractionProgress = 0.0f;
    dragonHead.headShrinkProgress = 0.0f;
    dragonHead.finalHeadScale = 0.0f;

    static bool needsReset = true;
    (void)needsReset;
}

// Function to draw the spiraling dragon body
void drawDragonBody() {
    if (!dragonHead.isActive || dragonHead.bodyProgress <= 0.0f) return;

    // Material for specular highlights
    const GLfloat bodyAmb[] = { 0.3f, 0.25f, 0.0f, 1.0f };
    const GLfloat bodyDiff[] = { 0.6f, 0.5f, 0.0f, 1.0f };
    const GLfloat bodySpec[] = { 0.2f, 0.2f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, bodyAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, bodyDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, bodySpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);

    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTex.dragon);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glPushMatrix();

    float globalScale = dragonHead.bodyScale * 1.0f;
    glScalef(globalScale, globalScale, globalScale);

    int   numSegments = 100;
    float totalHeight = 2.8f;
    float spiralRadius = 1.2f;

    for (int i = 0; i < numSegments; i++) {
        float segmentProgress = (float)i / (float)numSegments;
        if (segmentProgress > dragonHead.bodyProgress) break;

        float height = segmentProgress * totalHeight;
        float angle = segmentProgress * dragonHead.spiralAngle;

        float x = cosf(angle) * spiralRadius;
        float z = sinf(angle) * spiralRadius;
        float y = 1.2f - height;

        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(angle * 180.0f / (float)M_PI, 0, 1, 0);

        float segmentScale = 1.0f - segmentProgress * 0.4f;
        if (dragonHead.isRetracting) segmentScale *= (1.0f - dragonHead.retractionProgress * 0.7f);
        glScalef(segmentScale, segmentScale, segmentScale);

        drawSpherePrim(0.15f, 16, 12);

        glPopMatrix();
    }

    glPopMatrix();
    glPopAttrib();
}

// Static function to draw a single dragon eye
static void drawDragonEye(float x, float y, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.2f * scale, 0.2f * scale, 0.2f * scale);

    const GLfloat whiteAmb[] = { 0.50f, 0.50f, 0.50f, 1 };
    const GLfloat whiteDiff[] = { 1, 1, 1, 1 };
    const GLfloat whiteSpec[] = { 0.40f, 0.40f, 0.40f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, whiteAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, whiteDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32);

    drawSpherePrim(1.0f, 16, 12);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f);
    const GLfloat irisAmb[] = { 0.3f, 0.25f, 0.1f, 1 };
    const GLfloat irisDiff[] = { 1.0f, 0.8f, 0.0f, 1 };
    const GLfloat irisSpec[] = { 0.2f, 0.2f, 0.1f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, irisAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, irisDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, irisSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16);
    glScalef(0.6f, 0.6f, 0.6f);
    drawSpherePrim(1.0f, 16, 12);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.85f);
    const GLfloat blackAmb[] = { 0.05f, 0.05f, 0.05f, 1 };
    const GLfloat blackDiff[] = { 0.08f, 0.08f, 0.08f, 1 };
    const GLfloat blackSpec[] = { 0.05f, 0.05f, 0.05f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, blackAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blackDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blackSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8);
    glScalef(0.3f, 0.3f, 0.3f);
    drawSpherePrim(1.0f, 16, 12);
    glPopMatrix();

    glPopMatrix();
}

void drawDragonHead() {
    if (!dragonHead.isActive || dragonHead.scale <= 0.0f) return;

    // Head material (specular highlights)
    const GLfloat headAmb[] = { 0.3f, 0.25f, 0.0f, 1.0f };
    const GLfloat headDiff[] = { 0.6f, 0.5f, 0.0f, 1.0f };
    const GLfloat headSpec[] = { 0.2f, 0.2f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, headAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, headDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, headSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);

    glPushMatrix();
    glTranslatef(0.0f, dragonHead.headY, 1.5f);

    float globalScale = dragonHead.scale * 0.6f;
    glScalef(globalScale, globalScale, globalScale);

    // Enable dragon skin texture for head parts
    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gTex.dragon);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // 1) Long snout
    glPushMatrix();
    glTranslatef(0.0f, -0.1f, 0.3f);
    glScalef(0.7f, 0.6f, 2.0f);
    drawSpherePrim(0.6f, 32, 24);
    glPopMatrix();

    // 2) Upper skull
    glPushMatrix();
    glTranslatef(0.0f, 0.15f, 0.35f);
    glScalef(0.8f, 0.7f, 1.0f);
    drawSpherePrim(0.5f, 32, 24);
    glPopMatrix();

    // 3) Jaw / neck
    glPushMatrix();
    glTranslatef(0.0f, -0.25f, -0.2f);
    glScalef(0.5f, 0.4f, 1.2f);
    drawSpherePrim(0.4f, 32, 24);
    glPopMatrix();

    glPopAttrib(); // disable texture for the rest

    // Teeth
    const GLfloat teethAmb[] = { 0.2f, 0.2f, 0.2f, 1 };
    const GLfloat teethDiff[] = { 1.0f, 1.0f, 1.0f, 1 };
    const GLfloat teethSpec[] = { 0.3f, 0.3f, 0.3f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, teethAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teethDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, teethSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 8);

    glPushMatrix();
    glTranslatef(-0.15f, -0.25f, 1.45f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.08f * dragonHead.teethSize, 0.25f * dragonHead.teethSize, 12, 4);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15f, -0.25f, 1.45f);
    glRotatef(-90, 1, 0, 0);
    glutSolidCone(0.08f * dragonHead.teethSize, 0.25f * dragonHead.teethSize, 12, 4);
    glPopMatrix();

    // Horns
    const GLfloat hornAmb[] = { 0.2f, 0.2f, 0.2f, 1 };
    const GLfloat hornDiff[] = { 0.8f, 0.8f, 0.8f, 1 };
    const GLfloat hornSpec[] = { 0.3f, 0.3f, 0.3f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, hornAmb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, hornDiff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, hornSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 12);

    glPushMatrix();
    glTranslatef(-0.3f, 0.4f, 0.2f);
    glRotatef(-70, 1, 0, 0);
    glutSolidCone(0.1f * dragonHead.hornSize, 0.5f * dragonHead.hornSize, 16, 8);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3f, 0.4f, 0.2f);
    glRotatef(-70, 1, 0, 0);
    glutSolidCone(0.1f * dragonHead.hornSize, 0.5f * dragonHead.hornSize, 16, 8);
    glPopMatrix();

    // Eyes
    drawDragonEye(-0.35f, 0.15f, 0.6f, dragonHead.featureSize);
    drawDragonEye(0.35f, 0.15f, 0.6f, dragonHead.featureSize);

    glPopMatrix(); // end head transform
}

void drawFireParticles() {
    if (!dragonHead.isActive || !dragonHead.isBreathingFire || dragonHead.fireParticleCount <= 0.0f) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glPushMatrix();
    glTranslatef(0.0f, dragonHead.headY, 1.5f);
    float globalScale = dragonHead.scale * 0.6f;
    glScalef(globalScale, globalScale, globalScale);
    glTranslatef(0.0f, -0.1f, 1.8f); // mouth

    int numParticles = (int)dragonHead.fireParticleCount;

    for (int i = 0; i < numParticles; i++) {
        float randomX = ((float)rand() / RAND_MAX - 0.5f) * 0.3f;
        float randomY = ((float)rand() / RAND_MAX - 0.5f) * 0.2f;
        float randomZ = ((float)rand() / RAND_MAX) * 2.0f * dragonHead.fireIntensity;
        float particleSize = 0.05f + ((float)rand() / RAND_MAX) * 0.1f;

        glPushMatrix();
        glTranslatef(randomX, randomY, randomZ);
        glScalef(particleSize, particleSize, particleSize);

        const GLfloat fireAmb[] = { 0.8f, 0.3f, 0.0f, 0.8f };
        const GLfloat fireDiff[] = { 1.0f, 0.4f, 0.0f, 0.8f };
        const GLfloat fireSpec[] = { 1.0f, 0.6f, 0.0f, 0.8f };
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, fireAmb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, fireDiff);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, fireSpec);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 32.0f);

        drawSpherePrim(1.0f, 10, 8);
        glPopMatrix();
    }

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
