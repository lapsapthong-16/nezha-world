#pragma once
#include <GL/freeglut.h>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------- Color palette ----------------
namespace Palette {
    extern const float SKIN[3], HAIR[3], HAIR_RIM[3],
        VEST[3], VEST_EDGE[3], SHIRT[3],
        BOOT[3], ROPE[3], SCARF[3], GROUND[3];
    extern const float CLEAR[4];
    extern const float BLACK_FUR[3];
    extern const float WHITE_FUR[3];
}

// ---------------- Material functions ----------------
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

// ---------------- Primitive drawing functions ----------------
void drawSpherePrim(float radius, int slices = 24, int stacks = 18);
void drawCappedCylinder(float r, float h, int slices = 24);
void drawOpenCylinderY(float rBot, float rTop, float h,
    float startDeg, float sweepDeg, int slices = 64);

// ---------------- Math utilities ----------------
constexpr float  PI_F = 3.14159265358979323846f;
constexpr double PI_D = 3.14159265358979323846;

inline float  deg2rad(float d) { return d * (PI_F / 180.0f); }
inline double deg2rad(double d) { return d * (PI_D / 180.0); }

// ---------------- Simple 3D vector ----------------
struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
};

// ---------------- Decorative elements ----------------
void drawTinySphere(float r = 0.02f);
void drawStudRing(float y, float radius, int count, float r = 0.02f);
void drawStitchStrip(const Vec3& A, const Vec3& B, int count, float r = 0.012f);

// ---------------- Additional primitives (cannon/etc.) ----------------
void drawCuboidCannon(float cubX, float cubY, float cubZ);
void drawCuboidBasedZero(float cubX, float cubY, float cubZ);
void drawTriangularPrism(float triPx, float triPy, float triPz);
void drawCylinderCannon(float br, double tr, double h);
void drawSphereWithoutGLU(float radX, float radY, float radZ, float piDivide);
void drawCircleCannon(float rx, float ry);

// ---------------- GL Primitive counting system ----------------
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
    GLU_CONE_PRIM,          // friend's addition
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
    static int  getTotalPrimitives();
    static int  getPartPrimitives(BodyPart part);
    static int  getPrimitiveCounts(BodyPart part, GLPrimitive primitive);
    static void pause();
    static void resume();
    static bool isPaused();

private:
    static int partCounts[static_cast<int>(BodyPart::TOTAL_PARTS)]
        [static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES)];
    static BodyPart currentPart;
    static const char* partNames[static_cast<int>(BodyPart::TOTAL_PARTS)];
    static const char* primitiveNames[static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES)];
};

// Legacy support - redirect to PrimitiveCounter
class PolygonCounter {
public:
    static void reset() { PrimitiveCounter::reset(); }
    static void setCurrentPart(BodyPart part) { PrimitiveCounter::setCurrentPart(part); }
    static void addPolygons(int /*count*/) { /* no-op */ }
    static void printToConsole() { PrimitiveCounter::printToConsole(); }
    static int  getTotalPolygons() { return PrimitiveCounter::getTotalPrimitives(); }
    static int  getPartPolygons(BodyPart part) { return PrimitiveCounter::getPartPrimitives(part); }
    static void pause() { PrimitiveCounter::pause(); }
    static void resume() { PrimitiveCounter::resume(); }
    static bool isPaused() { return PrimitiveCounter::isPaused(); }
};

// Helper functions for counting GLUT primitives
void countGlutSolidSphere(double radius, int slices = 24, int stacks = 18);
void countGlutSolidCube(double size);
void countGlutSolidTorus(double innerRadius, double outerRadius, int nsides, int rings);

// Helper functions for counting specific primitives (friend’s enhancements)
void countGLPolygon(int vertexCount);
void countGLQuads(int quadCount);
void countGLTriangles(int triangleCount);
void countGLTriangleFan(int vertexCount);
void countGLTriangleStrip(int vertexCount);
void countGLLineLoop(int vertexCount);
void countGLLines(int lineCount);
void countGLPoints(int pointCount);
void countGLQuadStrip(int stripCount);
void countGLLineStrip(int vertexCount);
void countGLUCone();

// ---------------- Texture support (MULTIBYTE API) ----------------
// Canonical fields
struct Textures {
    // character / clothing
    GLuint pandaWhite = 0;      // panda_white_fur.bmp
    GLuint pandaBlack = 0;      // panda_black_fur.bmp
    GLuint goldBelt = 0;      // gold_belt.bmp

    // shirts (canonical names)
    GLuint redSilk = 0;      // red_silk_shirt.bmp
    GLuint blueSilk = 0;      // blue_silk_shirt.bmp
    GLuint orangeSilk = 0;      // orange_silk_shirt.bmp
    GLuint goldSilk = 0;      // gold_silk_shirt.bmp

    // ribbon / weapons
    GLuint redRibbon = 0;      // red_silk_ribbon.bmp
    GLuint blade = 0;      // sword_blade.bmp
    GLuint bamboo = 0;      // bamboo_boom.bmp
    GLuint bronze = 0;      // bronze_bullet.bmp

    // extras
    GLuint dragonSkin = 0;      // dragon_skin.bmp
    GLuint lotusPetal = 0;      // lotus_petal.bmp
    GLuint cloudTex = 0;      // cloud_texture.bmp

    GLuint body;   // NEW: torso texture
    GLuint leg;    // NEW: leg texture
};
extern Textures gTex;

// Back-compat ALIASES so older code compiles without edits.
// These are simple macro renames: gTex.redShirt -> gTex.redSilk, etc.
#define redShirt      redSilk
#define orangeShirt   orangeSilk
#define blueShirt     blueSilk
#define goldShirt     goldSilk
#define red_silk_shirt redSilk

#define dragon        dragonSkin
#define lotus         lotusPetal
#define cloud         cloudTex

// Shirt material helpers used by torso.cpp (implemented in customization.cpp)
void beginShirtMaterial();
void endShirtMaterial();

// init/quit and file loader (takes const char* for MBCS; converts internally)
void startGDIplus();
void stopGDIplus();
GLuint loadTexture2D(const char* path, bool mipmaps = true);
