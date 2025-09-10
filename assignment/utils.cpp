#include "utils.hpp"
#include <cmath>
#include <cstdio>

const float Palette::SKIN[3] = { 1.00f, 0.85f, 0.75f };
const float Palette::HAIR[3] = { 0.08f, 0.08f, 0.08f };
const float Palette::HAIR_RIM[3] = { 0.06f, 0.06f, 0.06f };
const float Palette::VEST[3] = { 0.55f, 0.20f, 0.12f };
const float Palette::VEST_EDGE[3] = { 0.65f, 0.12f, 0.10f };
const float Palette::SHIRT[3] = { 0.62f, 0.62f, 0.62f };
const float Palette::BOOT[3] = { 0.15f, 0.15f, 0.15f };
const float Palette::ROPE[3] = { 0.90f, 0.70f, 0.15f };
const float Palette::SCARF[3] = { 0.80f, 0.10f, 0.10f };
const float Palette::GROUND[3] = { 0.10f, 0.12f, 0.14f };
const float Palette::CLEAR[4] = { 0.85f, 0.85f, 0.88f, 1.0f };
const float Palette::BLACK_FUR[3] = { 0.12f, 0.12f, 0.12f };
const float Palette::WHITE_FUR[3] = { 0.95f, 0.95f, 0.95f };

static void setMaterial(float r, float g, float b, float shininess = 16.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.90f, 0.90f, 0.90f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

static void setMaterialHair(float r, float g, float b, float shininess = 8.0f) {
    const GLfloat diff[] = { r, g, b, 1.0f };
    const GLfloat amb[] = { r * 0.25f, g * 0.25f, b * 0.25f, 1.0f };
    const GLfloat spec[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

void matSkin() { setMaterial(Palette::SKIN[0], Palette::SKIN[1], Palette::SKIN[2]); }
void matHair() { setMaterialHair(Palette::HAIR[0], Palette::HAIR[1], Palette::HAIR[2]); }
void matHairRim() { setMaterialHair(Palette::HAIR_RIM[0], Palette::HAIR_RIM[1], Palette::HAIR_RIM[2]); }
void matVest() { setMaterial(Palette::VEST[0], Palette::VEST[1], Palette::VEST[2]); }
void matVestEdge() { setMaterial(Palette::VEST_EDGE[0], Palette::VEST_EDGE[1], Palette::VEST_EDGE[2]); }
void matShirt() { setMaterial(Palette::SHIRT[0], Palette::SHIRT[1], Palette::SHIRT[2]); }
void matBoot() { setMaterial(Palette::BOOT[0], Palette::BOOT[1], Palette::BOOT[2]); }
void matRope() { setMaterial(Palette::ROPE[0], Palette::ROPE[1], Palette::ROPE[2]); }
void matScarf() { setMaterial(Palette::SCARF[0], Palette::SCARF[1], Palette::SCARF[2]); }
void matGround() { setMaterial(Palette::GROUND[0], Palette::GROUND[1], Palette::GROUND[2]); }

void matBlackFur() { setMaterialHair(Palette::BLACK_FUR[0], Palette::BLACK_FUR[1], Palette::BLACK_FUR[2]); }

void matWhiteFur() {
    const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    const GLfloat diff[] = { 1.0f,  1.0f,  1.0f,  1.0f };
    const GLfloat spec[] = { 0.3f,  0.3f,  0.3f,  1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
}

void matPandaWhite() {
    const GLfloat amb[] = { 0.35f, 0.35f, 0.35f, 1.0f };
    const GLfloat diff[] = { 1.0f,  1.0f,  1.0f,  1.0f };
    const GLfloat spec[] = { 0.3f,  0.3f,  0.3f,  1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
}

void drawSpherePrim(float radius, int slices, int stacks) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_SPHERE_PRIM);
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluSphere(q, radius, slices, stacks);
    gluDeleteQuadric(q);
}

void drawCappedCylinder(float r, float h, int slices) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM, 2); // Two disk caps
    GLUquadric* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, r, r, h, slices, 1);
    gluDisk(q, 0.0, r, slices, 1);
    glPushMatrix();
    glTranslatef(0, 0, h);
    gluDisk(q, 0.0, r, slices, 1);
    glPopMatrix();
    gluDeleteQuadric(q);
}

void drawOpenCylinderY(float rBot, float rTop, float h, float startDeg, float sweepDeg, int slices) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUAD_STRIP_PRIM);
    const float y0 = -h * 0.5f, y1 = h * 0.5f;
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; ++i) {
        float t = startDeg + sweepDeg * (float)i / (float)slices;
        float a = deg2rad(t), c = cosf(a), s = sinf(a);
        glNormal3f(c, 0.0f, s);
        glVertex3f(rBot * c, y0, rBot * s);
        glVertex3f(rTop * c, y1, rTop * s);
    }
    glEnd();
}

void drawTinySphere(float r) { drawSpherePrim(r, 10, 8); }

void drawStudRing(float y, float radius, int count, float r) {
    glPushMatrix();
    glTranslatef(0.0f, y, 0.0f);
    for (int i = 0; i < count; ++i) {
        float a = (2.0f * (float)M_PI * (float)i) / (float)count;
        float x = radius * cosf(a);
        float z = radius * sinf(a);
        glPushMatrix();
        glTranslatef(x, 0.0f, z);
        drawTinySphere(r);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawStitchStrip(const Vec3& A, const Vec3& B, int count, float r) {
    glPushMatrix();
    for (int i = 0; i < count; ++i) {
        float t = (count <= 1) ? 0.5f : (float)i / (float)(count - 1);
        Vec3 P(
            A.x + t * (B.x - A.x),
            A.y + t * (B.y - A.y),
            A.z + t * (B.z - A.z)
        );
        glPushMatrix();
        glTranslatef(P.x, P.y, P.z);
        drawTinySphere(r);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawCuboidCannon(float cubX, float cubY, float cubZ) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUADS_PRIM);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-cubX, -cubY, -cubZ);
    glVertex3f(-cubX, -cubY, cubZ);
    glVertex3f(cubX, -cubY, cubZ);
    glVertex3f(cubX, -cubY, -cubZ);

    glNormal3f(-1, 0, 0);
    glVertex3f(-cubX, cubY, -cubZ);
    glVertex3f(-cubX, cubY, cubZ);
    glVertex3f(-cubX, -cubY, cubZ);
    glVertex3f(-cubX, -cubY, -cubZ);

    glNormal3f(0, 0, 1);
    glVertex3f(-cubX, -cubY, cubZ);
    glVertex3f(-cubX, cubY, cubZ);
    glVertex3f(cubX, cubY, cubZ);
    glVertex3f(cubX, -cubY, cubZ);

    glNormal3f(1, 0, 0);
    glVertex3f(cubX, -cubY, cubZ);
    glVertex3f(cubX, cubY, cubZ);
    glVertex3f(cubX, cubY, -cubZ);
    glVertex3f(cubX, -cubY, -cubZ);

    glNormal3f(0, 0, -1);
    glVertex3f(cubX, -cubY, -cubZ);
    glVertex3f(-cubX, -cubY, -cubZ);
    glVertex3f(-cubX, cubY, -cubZ);
    glVertex3f(cubX, cubY, -cubZ);

    glNormal3f(0, 1, 0);
    glVertex3f(cubX, cubY, cubZ);
    glVertex3f(-cubX, cubY, cubZ);
    glVertex3f(-cubX, cubY, -cubZ);
    glVertex3f(cubX, cubY, -cubZ);
    glEnd();
}

void drawCuboidBasedZero(float cubX, float cubY, float cubZ) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUADS_PRIM);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(-cubX, -cubY, 0);
    glVertex3f(-cubX, -cubY, cubZ);
    glVertex3f(cubX, -cubY, cubZ);
    glVertex3f(cubX, -cubY, 0);

    glNormal3f(-1, 0, 0);
    glVertex3f(-cubX, cubY, 0);
    glVertex3f(-cubX, cubY, cubZ);
    glVertex3f(-cubX, -cubY, cubZ);
    glVertex3f(-cubX, -cubY, 0);

    glNormal3f(0, 0, 1);
    glVertex3f(-cubX, -cubY, cubZ);
    glVertex3f(-cubX, cubY, cubZ);
    glVertex3f(cubX, cubY, cubZ);
    glVertex3f(cubX, -cubY, cubZ);

    glNormal3f(1, 0, 0);
    glVertex3f(cubX, -cubY, cubZ);
    glVertex3f(cubX, cubY, cubZ);
    glVertex3f(cubX, cubY, 0);
    glVertex3f(cubX, -cubY, 0);

    glNormal3f(0, 0, -1);
    glVertex3f(cubX, -cubY, 0);
    glVertex3f(-cubX, -cubY, 0);
    glVertex3f(-cubX, cubY, 0);
    glVertex3f(cubX, cubY, 0);

    glNormal3f(0, 1, 0);
    glVertex3f(cubX, cubY, cubZ);
    glVertex3f(-cubX, cubY, cubZ);
    glVertex3f(-cubX, cubY, 0);
    glVertex3f(cubX, cubY, 0);
    glEnd();
}

void drawTriangularPrism(float triPx, float triPy, float triPz) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GL_QUADS_PRIM);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glVertex3f(triPx, 0, triPz);
    glVertex3f(triPx, 0, -triPz);
    glVertex3f(-triPx, 0, -triPz);
    glVertex3f(-triPx, 0, triPz);

    glNormal3f(0, 0, -1);
    glVertex3f(triPx, 0, -triPz);
    glVertex3f(triPx, triPy, -triPz);
    glVertex3f(-triPx, triPy, -triPz);
    glVertex3f(-triPx, 0, -triPz);

    Vec3 v1(triPx * 2, 0, 0);
    Vec3 v2(0, triPy, triPz * 2);
    Vec3 normal(-v1.y * v2.z + v1.z * v2.y,
        v1.x * v2.z - v1.z * v2.x,
        -v1.x * v2.y + v1.y * v2.x);
    float len = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    normal.x /= len; normal.y /= len; normal.z /= len;

    glNormal3f(normal.x, normal.y, normal.z);
    glVertex3f(triPx, triPy, -triPz);
    glVertex3f(-triPx, triPy, -triPz);
    glVertex3f(-triPx, 0, triPz);
    glVertex3f(triPx, 0, triPz);
    glEnd();

    PrimitiveCounter::addPrimitive(GLPrimitive::GL_TRIANGLES_PRIM);
    glBegin(GL_TRIANGLES);
    glNormal3f(1, 0, 0);
    glVertex3f(triPx, 0, triPz);
    glVertex3f(triPx, triPy, -triPz);
    glVertex3f(triPx, 0, -triPz);

    glNormal3f(-1, 0, 0);
    glVertex3f(-triPx, 0, triPz);
    glVertex3f(-triPx, triPy, -triPz);
    glVertex3f(-triPx, 0, -triPz);
    glEnd();
}

void drawCylinderCannon(float br, double tr, double h) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_CYLINDER_PRIM);
    GLUquadric* cylinder = gluNewQuadric();
    gluQuadricNormals(cylinder, GLU_SMOOTH);
    gluCylinder(cylinder, br, tr, h, 30, 30);
    gluDeleteQuadric(cylinder);

    PrimitiveCounter::addPrimitive(GLPrimitive::GLU_DISK_PRIM, 2); // Two disk caps
    GLUquadric* disk = gluNewQuadric();
    gluQuadricNormals(disk, GLU_SMOOTH);
    gluDisk(disk, 0.0, br, 30, 1);
    glPushMatrix();
    glTranslatef(0, 0, h);
    gluDisk(disk, 0.0, tr, 30, 1);
    glPopMatrix();
    gluDeleteQuadric(disk);
}

void drawCircleCannon(float rx, float ry) {
    const int segments = 30;
    PrimitiveCounter::addPrimitive(GLPrimitive::GL_TRIANGLE_FAN_PRIM);
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    for (int i = 0; i <= segments; ++i) {
        float angle = (2.0f * (float)M_PI * (float)i) / (float)segments;
        float x = rx * cosf(angle);
        float y = ry * sinf(angle);
        glVertex3f(x, y, 0.0f);
    }
    glEnd();
}

void drawSphereWithoutGLU(float radX, float radY, float radZ, float piDivide) {
    const float PI = 3.141592f;
    GLfloat x, y, z, sliceA, stackA;
    int sliceNo = 30, stackNo = 30;
    int sliceCount = (int)(2 * PI / piDivide / (PI / sliceNo));
    
    for (sliceA = 0.0f; sliceA < 2 * PI / piDivide; sliceA += PI / sliceNo) {
        PrimitiveCounter::addPrimitive(GLPrimitive::GL_TRIANGLE_STRIP_PRIM);
        glBegin(GL_TRIANGLE_STRIP);
        for (stackA = 0.0f; stackA < 2 * PI / piDivide; stackA += PI / stackNo) {
            x = radX * cosf(stackA) * sinf(sliceA);
            y = radY * sinf(stackA) * sinf(sliceA);
            z = radZ * cosf(sliceA);
            glNormal3f(x / radX, y / radY, z / radZ);
            glVertex3f(x, y, z);

            float nextSlice = sliceA + PI / sliceNo;
            x = radX * cosf(stackA) * sinf(nextSlice);
            y = radY * sinf(stackA) * sinf(nextSlice);
            z = radZ * cosf(nextSlice);
            glNormal3f(x / radX, y / radY, z / radZ);
            glVertex3f(x, y, z);
        }
        glEnd();
    }
}

// PrimitiveCounter implementation
int PrimitiveCounter::partCounts[static_cast<int>(BodyPart::TOTAL_PARTS)][static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES)] = { 0 };
BodyPart PrimitiveCounter::currentPart = BodyPart::HEAD;
static int g_countPauseDepth = 0;

const char* PrimitiveCounter::partNames[static_cast<int>(BodyPart::TOTAL_PARTS)] = {
    "Head","Arms","Torso","Legs","Shorts","Cannon"
};

const char* PrimitiveCounter::primitiveNames[static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES)] = {
    "GL_POINTS", "GL_LINES", "GL_LINE_STRIP", "GL_LINE_LOOP",
    "GL_TRIANGLES", "GL_TRIANGLE_STRIP", "GL_TRIANGLE_FAN",
    "GL_QUADS", "GL_QUAD_STRIP", "GL_POLYGON",
    "GLU_SPHERE", "GLU_CYLINDER", "GLU_DISK",
    "GLUT_CUBE", "GLUT_SPHERE", "GLUT_TORUS"
};

void PrimitiveCounter::reset() {
    for (int i = 0; i < static_cast<int>(BodyPart::TOTAL_PARTS); ++i) {
        for (int j = 0; j < static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES); ++j) {
            partCounts[i][j] = 0;
        }
    }
}

void PrimitiveCounter::setCurrentPart(BodyPart part) { currentPart = part; }

void PrimitiveCounter::addPrimitive(GLPrimitive primitive, int count) {
    if (g_countPauseDepth == 0)
        partCounts[static_cast<int>(currentPart)][static_cast<int>(primitive)] += count;
}

void PrimitiveCounter::printToConsole() {
    std::printf("\n=== GL PRIMITIVE COUNT REPORT ===\n");
    for (int i = 0; i < static_cast<int>(BodyPart::TOTAL_PARTS); ++i) {
        std::printf("\n%s:\n", partNames[i]);
        int partTotal = 0;
        for (int j = 0; j < static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES); ++j) {
            if (partCounts[i][j] > 0) {
                std::printf("  %s: %d calls\n", primitiveNames[j], partCounts[i][j]);
                partTotal += partCounts[i][j];
            }
        }
        std::printf("  Part Total: %d primitives\n", partTotal);
    }
    std::printf("\nTOTAL PRIMITIVES: %d calls\n", getTotalPrimitives());
    std::printf("==================================\n");
    
    // Show pause status
    std::printf("Counter status: %s\n", 
        g_countPauseDepth > 0 ? "PAUSED" : "ACTIVE");
    if (g_countPauseDepth > 0) {
        std::printf("Pause depth: %d\n", g_countPauseDepth);
    }
    std::printf("\n");
}

int PrimitiveCounter::getTotalPrimitives() {
    int total = 0;
    for (int i = 0; i < static_cast<int>(BodyPart::TOTAL_PARTS); ++i) {
        for (int j = 0; j < static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES); ++j) {
            total += partCounts[i][j];
        }
    }
    return total;
}

int PrimitiveCounter::getPartPrimitives(BodyPart part) {
    int total = 0;
    int partIndex = static_cast<int>(part);
    for (int j = 0; j < static_cast<int>(GLPrimitive::TOTAL_PRIMITIVES); ++j) {
        total += partCounts[partIndex][j];
    }
    return total;
}

int PrimitiveCounter::getPrimitiveCounts(BodyPart part, GLPrimitive primitive) {
    return partCounts[static_cast<int>(part)][static_cast<int>(primitive)];
}

void countGlutSolidSphere(double radius, int slices, int stacks) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GLUT_SPHERE_PRIM);
    glutSolidSphere(radius, slices, stacks);
}

void countGlutSolidCube(double size) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GLUT_CUBE_PRIM);
    glutSolidCube(size);
}

void PrimitiveCounter::pause() { ++g_countPauseDepth; }

void PrimitiveCounter::resume() { if (g_countPauseDepth > 0) --g_countPauseDepth; }

bool PrimitiveCounter::isPaused() { return g_countPauseDepth > 0; }

void countGlutSolidTorus(double innerRadius, double outerRadius, int nsides, int rings) {
    PrimitiveCounter::addPrimitive(GLPrimitive::GLUT_TORUS_PRIM);
    glutSolidTorus(innerRadius, outerRadius, nsides, rings);
}
