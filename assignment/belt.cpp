#include "belt.hpp"
#include "utils.hpp"
#include "model.hpp"
#include <GL/freeglut.h>

void drawBraidedBelt() {
    // Ensure torus rings are visible from all sides at grazing angles
    GLboolean wasCull = glIsEnabled(GL_CULL_FACE);
    if (wasCull) glDisable(GL_CULL_FACE);

    matRope();

    // --- main rope ring around the waist ---
    const int   N = 22;         // number of small torus links
    const float y = -0.245f;    // belt height (Y)
    const float R = 0.60f;      // distance from center
    const float tube = 0.06f;      // rope thickness

    for (int i = 0; i < N; ++i) {
        float t = (float)i / N * 2.0f * PI_F;   // was 2.0f * M_PI
        float x = R * std::cos(t);
        float z = R * std::sin(t);

        glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(t * 180.0f / PI_F, 0, 1, 0);  // convert radians->degrees
        glRotatef(90, 1, 0, 0);
        glutSolidTorus(tube * 0.35f, tube, 12, 24);
        glPopMatrix();
    }

    // --- front knot (at +Z) built from two interlocked rings + small bulge ---
    const float kz = 0.54f;
    const float kx = 0.00f;
    const float ky = y + 0.01f;

    // ring A
    glPushMatrix();
    glTranslatef(kx, ky, kz);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(tube * 0.42f, tube * 1.05f, 14, 28);
    glPopMatrix();

    // ring B (slightly angled)
    glPushMatrix();
    glTranslatef(kx + 0.10f, ky, kz - 0.06f);
    glRotatef(35, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    glutSolidTorus(tube * 0.38f, tube * 0.95f, 14, 28);
    glPopMatrix();

    // small bulge in the middle of the knot
    glPushMatrix();
    glTranslatef(kx + 0.04f, ky + 0.01f, kz - 0.01f);
    glutSolidSphere(tube * 0.80f, 18, 14);
    glPopMatrix();

    // --- short rope ends / tails ---
    auto ropeEnd = [&](float sx, float sy, float sz, float yawDeg, float pitchDeg) {
        glPushMatrix();
        glTranslatef(sx, sy, sz);
        glRotatef(yawDeg, 0, 1, 0);
        glRotatef(pitchDeg, 1, 0, 0);

        GLUquadric* q = gluNewQuadric();
        gluQuadricNormals(q, GLU_SMOOTH);

        const float h = 0.34f; // tail length
        gluCylinder(q, tube * 0.70f, tube * 0.40f, h, 18, 1);               // taper
        glPushMatrix(); glTranslatef(0, 0, h); glutSolidSphere(tube * 0.40f, 14, 10); glPopMatrix(); // cap

        gluDeleteQuadric(q);
        glPopMatrix();
        };

    ropeEnd(kx - 0.06f, ky - 0.01f, kz + 0.02f, -10.0f, 75.0f); // left end
    ropeEnd(kx + 0.12f, ky - 0.01f, kz - 0.01f, 15.0f, 75.0f); // right end

    if (wasCull) glEnable(GL_CULL_FACE);
}
