#include "cannon.hpp"
#include <cmath>
#include <cstdio>

// Global cannon state instance
CannonState cannonState;

// Material setup functions
static void setCannonMaterial(float r, float g, float b) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.3f, g * 0.3f, b * 0.3f, 1.0f };
    const GLfloat spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
}

static void matCannonMetal() { setCannonMaterial(0.75f, 0.75f, 0.75f); }
static void matCannonAccent() { setCannonMaterial(1.0f, 0.0f, 0.0f); }
static void matCannonOrange() { setCannonMaterial(1.0f, 0.5f, 0.0f); }

// Cannon muzzle design components
void drawCannonMuzzleDesign() {
    matCannonAccent();
    
    // Red design elements at 4 positions around the muzzle
    const float positions[4][2] = {{0, 2.9f}, {-2.9f, 0}, {0, -2.9f}, {2.9f, 0}};
    const float rotations[4] = {0, 90, 180, -90};
    
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(positions[i][0], positions[i][1], 7.0f);
        glRotatef(rotations[i], 0, 0, 1);
        glRotatef(-3.576f, 1, 0, 0);
        
        glPushMatrix();
        glTranslatef(0, -0.5f, -1.5f);
        glRotatef(180, 0, 1, 0);
        drawTriangularPrism(0.5f, 1.0f, 0.5f);
        glPopMatrix();
        
        drawCuboidCannon(0.5f, 0.5f, 1.0f);
        
        glPushMatrix();
        glTranslatef(0, -0.5f, 1.0f);
        glRotatef(90, 1, 0, 0);
        drawTriangularPrism(0.5f, 1.0f, 1.0f);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0, -1.0f, 0.25f);
        drawCuboidCannon(0.5f, 0.5f, 0.75f);
        glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0, -1.0f, -0.5f);
        glRotatef(180, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        drawTriangularPrism(0.5f, 0.5f, 0.5f);
        glPopMatrix();
        
        glPopMatrix();
    }
}

// Cannon shell elements
void drawCannonShell() {
    matCannonAccent();
    
    glPushMatrix();
    glRotatef(45, 0, 1, 0);
    glRotatef(-45, 0, 0, 1);
    glTranslatef(0, 3.0f, 1.5f);
    drawCuboidCannon(0.25f, 2.0f, 0.25f);
    glPopMatrix();
    
    glPushMatrix();
    glRotatef(-45, 0, 1, 0);
    glRotatef(45, 0, 0, 1);
    glTranslatef(0, 3.0f, 1.5f);
    drawCuboidCannon(0.25f, 2.0f, 0.25f);
    glPopMatrix();
}

// Cannon design elements
void drawCannonDesignElements() {
    matCannonAccent();
    
    glPushMatrix();
    glTranslatef(0, -0.5f, -0.75f);
    glTranslatef(0, 2.9f, 7.0f);
    glRotatef(-3.576f, 1, 0, 0);
    
    glPushMatrix();
    glTranslatef(0, -0.5f, -3.5f);
    drawTriangularPrism(0.5f, 1.0f, 0.5f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 0, -5.0f);
    drawCuboidCannon(0.5f, 0.5f, 1.0f);
    glPopMatrix();
    
    glPopMatrix();
}

// Cannon stand/mount
void drawCannonStand() {
    matCannonAccent();
    
    glPushMatrix();
    glTranslatef(-0.5f, -2.5f, 2.0f);
    glRotatef(90, 0, 1, 0);
    drawCylinderCannon(1.0f, 1.0f, 1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, -6.0f, -1.5f);
    glRotatef(-45, 1, 0, 0);
    drawCuboidBasedZero(0.5f, 0.5f, 5.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.5f, -6.0f, -1.5f);
    glRotatef(90, 0, 1, 0);
    drawCylinderCannon(1.0f, 1.0f, 1.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, -9.5f, 2.0f);
    glRotatef(-135, 1, 0, 0);
    drawCuboidBasedZero(0.5f, 0.5f, 5.0f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.5f, -9.5f, 2.0f);
    glRotatef(90, 0, 1, 0);
    drawCylinderCannon(1.0f, 1.0f, 1.0f);
    glPopMatrix();
}

// Main cannon barrel
void drawCannonBarrel() {
    matCannonMetal();
    
    glPushMatrix();
    glScalef(1.25f, 1.25f, 1.25f);
    
    // Main barrel
    drawCylinderCannon(2.0f, 2.5f, 8.0f);
    
    // Inner barrel detail
    glPushMatrix();
    drawCylinderCannon(2.0f, 2.5f, 7.5f);
    glPopMatrix();
    
    // Muzzle design
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    drawCannonMuzzleDesign();
    glPopMatrix();
    
    // Tip sphere
    glPushMatrix();
    glTranslatef(0, 0, 7.5f);
    matCannonOrange();
    drawSphereWithoutGLU(2.2f, 2.2f, 2.0f, 1.0f);
    glPopMatrix();
    
    drawCannonDesignElements();
    drawCannonShell();
    
    glPopMatrix();
}

// Laser beam visual effect
void drawLaserBeam() {
    // Always draw something if we're shooting to debug
    if (cannonState.shootOn || cannonState.powerBall > 0.0f || cannonState.attack > 0.0f) {
        matCannonAccent();
        
        glPushMatrix();
        
        // Power ball at origin (always visible when shooting)
        if (cannonState.powerBall > 0.0f) {
            glPushMatrix();
            float ballSize = cannonState.powerBall * 0.02f + 0.5f; // Make it more visible
            glScalef(ballSize, ballSize, ballSize);
            drawSphereWithoutGLU(1.0f, 1.0f, 1.0f, 1.0f);
            glPopMatrix();
        }
        
        // Main laser beam (cylinder) - show when attack is active
        if (cannonState.attack > 0.0f && cannonState.attackRadius > 0.0f) {
            glPushMatrix();
            float beamLength = cannonState.attack * 0.01f + 1.0f; // Make beam more visible
            glScalef(cannonState.attackRadius * 0.5f, cannonState.attackRadius * 0.5f, beamLength);
            drawCylinderCannon(1.0f, 1.0f, 1.0f);
            glPopMatrix();
        }
        
        // Debug: Always show a small red sphere when shooting is active
        if (cannonState.shootOn) {
            glPushMatrix();
            glScalef(2.0f, 2.0f, 2.0f);
            drawSphereWithoutGLU(1.0f, 1.0f, 1.0f, 1.0f);
            glPopMatrix();
        }
        
        glPopMatrix();
    }
}

// Main laser cannon drawing function
void drawLaserCannon() {
    glPushMatrix();
    
    // Main barrel assembly
    glPushMatrix();
    drawCannonBarrel();
    glPopMatrix();
    
    // Stand/mount
    drawCannonStand();
    
    glPopMatrix();
}

void updateCannonAnimation() {
    if (!cannonState.weaponOn) {   
        if (cannonState.canonRot <= 45.0f) {
            cannonState.canonRotSpeed = 5.0f;   
            cannonState.startFire = false;
        }
        else {
            cannonState.canonRotSpeed = 0.0f;
            cannonState.startFire = true;
        }
    }
    else {  
        if (cannonState.canonRot > 0.0f) {
            cannonState.canonRotSpeed = -5.0f;  
            cannonState.startFire = false;
        }
        else {
            cannonState.canonRotSpeed = 0.0f;
            cannonState.startFire = true;
        }
    }

    cannonState.canonRot += cannonState.canonRotSpeed;
    if (cannonState.canonRot < 0.0f) cannonState.canonRot = 0.0f;
    if (cannonState.canonRot > 45.0f) cannonState.canonRot = 45.0f;
}

void updateShootingAnimation() {
    if (cannonState.shootOn) {
        if (cannonState.powerBall <= 30.0f) {
            cannonState.powerBallSize = 5.0f;
            cannonState.endShoot = false;
        } else {
            cannonState.powerBallSize = 0.0f;
            if (cannonState.attack <= 1000.0f) {
                cannonState.attackLength = 100.0f;
                if (cannonState.attackRadius <= 2.0f) {
                    cannonState.attackRadius += 0.5f;
                }
                cannonState.endShoot = false;
            } else {
                cannonState.attackLength = 0.0f;
                cannonState.endShoot = true;
                cannonState.shootOn = false;
            }
        }
    } else {
        if (cannonState.powerBall > 0.0f) {
            cannonState.powerBallSize = -5.0f;
        } else {
            cannonState.powerBallSize = 0.0f;
        }
        if (cannonState.attack > 0.0f) {
            cannonState.attackLength = -50.0f;
        } else {
            cannonState.attackLength = 0.0f;
        }
        if (cannonState.attackRadius > 0.0f) {
            cannonState.attackRadius -= 0.5f;
        } else {
            cannonState.attackRadius = 0.0f;
        }
    }
    
    // Update values
    cannonState.attack += cannonState.attackLength;
    cannonState.powerBall += cannonState.powerBallSize;
    
    // Clamp values
    if (cannonState.attack < 0.0f) cannonState.attack = 0.0f;
    if (cannonState.powerBall < 0.0f) cannonState.powerBall = 0.0f;
    if (cannonState.attackRadius < 0.0f) cannonState.attackRadius = 0.0f;
}

// Control functions
void toggleCannon() {
    cannonState.weaponOn = !cannonState.weaponOn;
    printf("Cannon toggled: %s\n", cannonState.weaponOn ? "ON" : "OFF");
}

void fireCannon() {
    printf("Fire button pressed! weaponOn: %d, startFire: %d, canonRot: %.1f\n", 
           cannonState.weaponOn, cannonState.startFire, cannonState.canonRot);
    
    if (cannonState.startFire && cannonState.weaponOn) {
        cannonState.shootOn = true;
        printf("Cannon fired! shootOn: %d, powerBall: %.1f, attack: %.1f\n", 
               cannonState.shootOn, cannonState.powerBall, cannonState.attack);
    } else {
        // Allow firing even if cannon isn't fully ready for testing
        cannonState.shootOn = true;
        printf("Force firing for debug - shootOn: %d\n", cannonState.shootOn);
    }
}
