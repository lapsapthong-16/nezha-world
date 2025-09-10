#pragma once
#include <GL/freeglut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Color palette
namespace Palette {
    extern const float SKIN[3], HAIR[3], HAIR_RIM[3],
        VEST[3], VEST_EDGE[3], SHIRT[3],
        BOOT[3], ROPE[3], SCARF[3], GROUND[3];
    extern const float CLEAR[4];
    extern const float BLACK_FUR[3];
    extern const float WHITE_FUR[3];
}

// Material functions
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
void matBlackFur();
void matWhiteFur();
void matPandaWhite();

// Primitive drawing functions
void drawSpherePrim(float radius, int slices = 24, int stacks = 18);
void drawCappedCylinder(float r, float h, int slices = 24);
void drawOpenCylinderY(float rBot, float rTop, float h,
    float startDeg, float sweepDeg, int slices = 64);

// Math constants and utilities
constexpr float  PI_F = 3.14159265358979323846f;
constexpr double PI_D = 3.14159265358979323846;

inline float  deg2rad(float d) { return d * (PI_F / 180.0f); }
inline double deg2rad(double d) { return d * (PI_D / 180.0); }

// Simple 3D vector
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

// Decorative element functions
void drawTinySphere(float r = 0.02f);
void drawStudRing(float y, float radius, int count, float r = 0.02f);
void drawStitchStrip(const Vec3& A, const Vec3& B, int count, float r = 0.012f);

// Additional primitive functions for cannon
void drawCuboidCannon(float cubX, float cubY, float cubZ);
void drawCuboidBasedZero(float cubX, float cubY, float cubZ);
void drawTriangularPrism(float triPx, float triPy, float triPz);
void drawCylinderCannon(float br, double tr, double h);
void drawSphereWithoutGLU(float radX, float radY, float radZ, float piDivide);
void drawCircleCannon(float rx, float ry);

// GL Primitive counting system
enum class BodyPart {
    HEAD,
    ARMS,
    TORSO,
    LEGS,
    SHORTS,
    CANNON,
    TOTAL_PARTS
};

enum class GLPrimitive {
    GL_POINTS_PRIM,
    GL_LINES_PRIM,
    GL_LINE_STRIP_PRIM,
    GL_LINE_LOOP_PRIM,
    GL_TRIANGLES_PRIM,
    GL_TRIANGLE_STRIP_PRIM,
    GL_TRIANGLE_FAN_PRIM,
    GL_QUADS_PRIM,
    GL_QUAD_STRIP_PRIM,
    GL_POLYGON_PRIM,
    GLU_SPHERE_PRIM,
    GLU_CYLINDER_PRIM,
    GLU_DISK_PRIM,
    GLUT_CUBE_PRIM,
    GLUT_SPHERE_PRIM,
    GLUT_TORUS_PRIM,
    TOTAL_PRIMITIVES
};

class PrimitiveCounter {
public:
    static void reset();
    static void setCurrentPart(BodyPart part);
    static void addPrimitive(GLPrimitive primitive, int count = 1);
    static void printToConsole();
    static int getTotalPrimitives();
    static int getPartPrimitives(BodyPart part);
    static int getPrimitiveCounts(BodyPart part, GLPrimitive primitive);
    static void pause();
    static void resume();
    static bool isPaused();
    
private:
    static int partCounts[static_cast<int>(BodyPart::TOTAL_PARTS)][static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES)];
    static BodyPart currentPart;
    static const char* partNames[static_cast<int>(BodyPart::TOTAL_PARTS)];
    static const char* primitiveNames[static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES)];
};

// Legacy support - redirect to PrimitiveCounter
class PolygonCounter {
public:
    static void reset() { PrimitiveCounter::reset(); }
    static void setCurrentPart(BodyPart part) { PrimitiveCounter::setCurrentPart(part); }
    static void addPolygons(int count) { /* Legacy function - no longer used */ }
    static void printToConsole() { PrimitiveCounter::printToConsole(); }
    static int getTotalPolygons() { return PrimitiveCounter::getTotalPrimitives(); }
    static int getPartPolygons(BodyPart part) { return PrimitiveCounter::getPartPrimitives(part); }
    static void pause() { PrimitiveCounter::pause(); }
    static void resume() { PrimitiveCounter::resume(); }
    static bool isPaused() { return PrimitiveCounter::isPaused(); }
};

// Helper functions for counting GLUT primitives
void countGlutSolidSphere(double radius, int slices = 24, int stacks = 18);
void countGlutSolidCube(double size);
void countGlutSolidTorus(double innerRadius, double outerRadius, int nsides, int rings);