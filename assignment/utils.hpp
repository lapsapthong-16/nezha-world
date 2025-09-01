#pragma once
#include <GL/freeglut.h>
#include <cmath>

// Palette declarations (extern in header, defined in utils.cpp)
namespace Palette {
    extern const float SKIN[3], HAIR[3], HAIR_RIM[3],
        VEST[3], VEST_EDGE[3], SHIRT[3],
        BOOT[3], ROPE[3], SCARF[3], GROUND[3];
    extern const float CLEAR[4];
    extern const float BLACK_FUR[3];  // New black fur color
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

// Primitive helpers (implemented in utils.cpp)
void drawSpherePrim(float radius, int slices = 24, int stacks = 18);
void drawCappedCylinder(float r, float h, int slices = 24);
void drawOpenCylinderY(float rBot, float rTop, float h,
    float startDeg, float sweepDeg, int slices = 64);

constexpr float  PI_F = 3.14159265358979323846f;
constexpr double PI_D = 3.14159265358979323846;

inline float  deg2rad(float d) { return d * (PI_F / 180.0f); }
inline double deg2rad(double d) { return d * (PI_D / 180.0); }