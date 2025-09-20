// Build guard: keep this TU Multi-Byte (ANSI)
#ifdef UNICODE
#  undef UNICODE
#endif
#ifdef _UNICODE
#  undef _UNICODE
#endif

#ifndef NOMINMAX
#  define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <GL/freeglut.h>

#include "customization.hpp"
#include "utils.hpp"
#include "cannon.hpp"
#include "head.hpp"
#include "arms.hpp"
#include "legs.hpp"

// ===== state =====
bool  gWeaponOn = true;
int   gWeaponType = WEAPON_SWORD;
float gWeaponLenScale = 1.0f;
RGB   gWeaponColor = { 0.20f, 0.20f, 0.80f };
RGB   gOutfitColor = { 0.20f, 0.60f, 0.90f };

// shirt (texture + tint) used by torso/shorts/etc.
GLuint     gShirtTex = 0;  // set at runtime
float      gShirtTint[3] = { 1.0f, 1.0f, 1.0f };
ShirtStyle gShirtStyle = SHIRT_RED;

// ---------- tiny helpers ----------
static inline void setMaterialRGB(const RGB& c) {
    const GLfloat amb[4] = { 0.2f * (GLfloat)c.r, 0.2f * (GLfloat)c.g, 0.2f * (GLfloat)c.b, 1.0f };
    const GLfloat dif[4] = { (GLfloat)c.r, (GLfloat)c.g, (GLfloat)c.b, 1.0f };
    const GLfloat spc[4] = { 0.30f, 0.30f, 0.30f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spc);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 16.0f);
}
void applyOutfitMaterial() { setMaterialRGB(gOutfitColor); }

// ===== Shirt material on/off =====
void beginShirtMaterial() {
    if (gShirtTex == 0) return;

    glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gShirtTex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glColor3f(gShirtTint[0], gShirtTint[1], gShirtTint[2]);

    const GLfloat amb[] = { 0.35f * gShirtTint[0], 0.35f * gShirtTint[1], 0.35f * gShirtTint[2], 1.0f };
    const GLfloat dif[] = { gShirtTint[0], gShirtTint[1], gShirtTint[2], 1.0f };
    const GLfloat spec[] = { 0.25f, 0.25f, 0.25f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
}
void endShirtMaterial() {
    if (gShirtTex == 0) return;
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopAttrib();
}

// Map style to assets and matching sword color
static inline void styleToAssets(ShirtStyle s, GLuint& tex, float tint[3], RGB& swordColor) {
    tint[0] = tint[1] = tint[2] = 1.0f; // textures already colored
    switch (s) {
    case SHIRT_RED:    tex = gTex.redShirt;    swordColor = { 0.90f, 0.20f, 0.20f }; break;
    case SHIRT_ORANGE: tex = gTex.orangeShirt; swordColor = { 1.00f, 0.50f, 0.10f }; break;
    case SHIRT_BLUE:   tex = gTex.blueShirt;   swordColor = { 0.20f, 0.50f, 0.90f }; break;
    case SHIRT_GOLD:
    default:           tex = gTex.goldShirt;   swordColor = { 0.95f, 0.80f, 0.25f }; break;
    }
}

// Public setter (also updates sword and outfit color to match)
void setShirtStyle(ShirtStyle s) {
    gShirtStyle = s;
    RGB swordCol;
    styleToAssets(s, gShirtTex, gShirtTint, swordCol);
    gWeaponColor = swordCol;   // weapon 1 color
    gOutfitColor = swordCol;   // lets other materials match palette
}

// Accessor used by shorts (and others) to bind the same texture as the shirt
GLuint getCurrentShirtTexture() {
    return gShirtTex;
}

// ===== Key handler =====
//
// 3  -> toggle weapon on/off (also toggles cannon)
// 1/2 -> weapon type
// -/+ -> weapon length
// O    -> cycle shirt (Red/Orange/Blue/Gold)
// R/T/U/Y -> set shirt Red/Orange/Blue/Gold  (NOTE: uppercase T reserved below)
// T/G (uppercase) -> head spin left/right
// Arms: J/U/I/N lifts; 5/4/6/Q elbows
// Legs: B/F left up/down;  ;/[ right up/down
bool handleCustomizationKey(unsigned char key) {
    bool handled = true;
    switch (key) {
        // toggle weapon and cannon visibility
    case '3': gWeaponOn = !gWeaponOn; toggleCannon(); break;

        // weapon types
    case '1': gWeaponType = WEAPON_SWORD; break;
    case '2': gWeaponType = WEAPON_STAFF; break;

        // length (Shift+'=' is '+')
    case '-': case '_': gWeaponLenScale = std::max(0.6f, gWeaponLenScale - 0.1f); break;
    case '+': case '=': gWeaponLenScale = std::min(2.0f, gWeaponLenScale + 0.1f); break;

        // Head spin (UPPERCASE only to avoid shirt 't')
    case 'l': spinHeadLeft();  break;
    case 'g': spinHeadRight(); break;

        // Shirt palette controls
    case 'o': case 'O': {
        ShirtStyle next = static_cast<ShirtStyle>((static_cast<int>(gShirtStyle) + 1) % 4);
        setShirtStyle(next);
        break;
    }
    case 'r': case 'R': setShirtStyle(SHIRT_RED);    break;
    case 't':            setShirtStyle(SHIRT_ORANGE); break; // lowercase 't' = Orange
    case 'u': case 'U': setShirtStyle(SHIRT_BLUE);   break;
    case 'y': case 'Y': setShirtStyle(SHIRT_GOLD);   break;

        // Optional: cycle weapon color independently
    case 'k': case 'K': {
        static const RGB kPal[] = {
            {0.90f,0.20f,0.20f}, {0.20f,0.70f,0.20f},
            {0.20f,0.50f,0.90f}, {0.95f,0.80f,0.20f}, {0.85f,0.50f,0.90f}
        };
        static int idx = 0;
        idx = (idx + 1) % 5;
        gWeaponColor = kPal[idx];
        break;
    }

            // Arms lift/lower
    case 'j': case 'J': liftLeftArm();  break;
    case 'm': case 'M': lowerLeftArm(); break;
    case 'i': case 'I': liftRightArm(); break;
    case 'n': case 'N': lowerRightArm(); break;

        // Elbows
    case '5': case '%': bendLeftElbow();        break;
    case '4': case '$': straightenLeftElbow();  break;
    case '6': case '^': bendRightElbow();       break;
    case 'q': case 'Q': straightenRightElbow(); break;

        // Legs
    case 'b': case 'B': liftLeftLeg();  break;
    case 'f': case 'F': lowerLeftLeg(); break;
    case ';': case ':': liftRightLeg(); break;
    case '[': case '{': lowerRightLeg(); break;

    default: handled = false; break;
    }

    if (handled) glutPostRedisplay();
    return handled;
}
