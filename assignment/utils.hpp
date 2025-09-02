#pragma once
#include <GL/freeglut.h>
#include <cmath>

// Fallback for M_PI if <cmath> didn’t define it
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
// Palette declarations (extern in header, defined in utils.cpp)
namespace Palette {
    extern const float SKIN[3], HAIR[3], HAIR_RIM[3],
        VEST[3], VEST_EDGE[3], SHIRT[3],
        BOOT[3], ROPE[3], SCARF[3], GROUND[3];
    extern const float CLEAR[4];
    extern const float BLACK_FUR[3];  // New black fur color
    extern const float WHITE_FUR[3];  // Pure white for panda belly
}

// Materials (implemented in utils.cpp)
void matSkin();
void matHair();
void matHairRim();
void matVest();
void matVestEdge();
void matShirt();
void matBoot();
void matRope();
void matScarf();
void matGround();
void matBlackFur();  // New black fur material
void matWhiteFur();  // Pure white fur material for panda belly
void matPandaWhite();  // Exact copy of head's matWhite() function

// Primitive helpers (implemented in utils.cpp)
void drawSpherePrim(float radius, int slices = 24, int stacks = 18);
void drawCappedCylinder(float r, float h, int slices = 24);
void drawOpenCylinderY(float rBot, float rTop, float h,
    float startDeg, float sweepDeg, int slices = 64);

constexpr float  PI_F = 3.14159265358979323846f;
constexpr double PI_D = 3.14159265358979323846;

inline float  deg2rad(float d) { return d * (PI_F / 180.0f); }
inline double deg2rad(double d) { return d * (PI_D / 180.0); }
// utils.hpp  (add near the top, after includes/guards)

// Simple 3D vec so we don't depend on GLM here
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

// --- Declaration of the helper primitives so other .cpps can call them ---
void drawTinySphere(float r = 0.02f);
void drawStudRing(float y, float radius, int count, float r = 0.02f);
void drawStitchStrip(const Vec3& A, const Vec3& B, int count, float r = 0.012f);
