#pragma once

// ----- Build guard: we expect Multi-Byte (ANSI), not UNICODE -----
#if defined(UNICODE) || defined(_UNICODE)
#  error "UNICODE/_UNICODE is defined. Set Project > Properties > Advanced > Character Set = 'Use Multi-Byte Character Set'."
#endif

#include <GL/freeglut.h>

// Simple RGB
struct RGB { float r, g, b; };

// Weapon selection
enum { WEAPON_SWORD = 1, WEAPON_STAFF = 2 };

// Shirt styles (4 only)
enum ShirtStyle { SHIRT_RED = 0, SHIRT_ORANGE = 1, SHIRT_BLUE = 2, SHIRT_GOLD = 3 };

// ===== Global state (used by other modules) =====
extern bool  gWeaponOn;
extern int   gWeaponType;
extern float gWeaponLenScale;
extern RGB   gWeaponColor;   // used by sword
extern RGB   gOutfitColor;   // palette color for outfit

// Current shirt choice (texture + tint)
extern GLuint     gShirtTex;       // GL texture id to bind
extern float      gShirtTint[3];   // diffuse tint (MODULATE)
extern ShirtStyle gShirtStyle;

// Outfit material helper
void applyOutfitMaterial();

// ===== Shirt material bind/unbind around textured draws =====
void beginShirtMaterial();
void endShirtMaterial();

// Change shirt style programmatically (also updates sword/outfit color)
void setShirtStyle(ShirtStyle s);

// Accessor for “the active shirt texture” so shorts (etc.) can match the shirt
GLuint getCurrentShirtTexture();

// Key handler (call from main keyboard): implements O / R / T / U / Y behavior
bool handleCustomizationKey(unsigned char key);

// Cannon + head helpers declared elsewhere, only referenced here
void toggleCannonVisibility();  // from cannon.hpp
void spinHeadLeft();            // from head.hpp
void spinHeadRight();           // from head.hpp
