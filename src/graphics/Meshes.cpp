#include "graphics/Meshes.h"

#include <array>
#include <cmath>
#include <unistd.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "game/GameSession.h"

namespace graphics::mesh {

void draw_sphere(int color) {
    const std::array<GLfloat, 4> color_none{0.0F, 0.0F, 0.0F, 0.0F};

    if (color == 0) {
        const std::array<GLfloat, 4> color_red{1.0F, 0.0F, 0.0F, 0.0F};
        glColor4fv(color_red.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_red.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    }

    if (color == 1) {
        const std::array<GLfloat, 4> color_white{1.0F, 1.0F, 1.0F, 1.0F};
        glColor4fv(color_white.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_white.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    }

    if (color == 2) {
        const std::array<GLfloat, 4> color_black{0.0F, 0.0F, 0.0F, 0.0F};
        glColor4fv(color_black.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_black.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    }

    if (color == 3) {
        const std::array<GLfloat, 4> color_green{0.0F, 0.0F, 1.0F, 0.0F};
        glColor4fv(color_green.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_green.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    }

    if (color == 4) {
        const std::array<GLfloat, 4> color_blue{0.0F, 1.0F, 0.0F, 0.0F};
        glColor4fv(color_blue.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_blue.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    }

    glBegin(GL_TRIANGLES);
    GLUquadricObj *quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricTexture(quadratic, GL_TRUE);

    gluSphere(quadratic, 1.0F, 32, 32);
    glEnd();
}

void draw_unit_cube(GameSession &session, int color) {
    if (color == 0) {
        glBindTexture(GL_TEXTURE_2D, session.textures[0]);
    } else if (color == 3) {
        const std::array<GLfloat, 4> color_none{0.0F, 0.0F, 0.0F, 0.0F};
        const std::array<GLfloat, 4> color_green{0.0F, 0.0F, 1.0F, 0.0F};
        glColor4fv(color_green.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_green.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    } else if (color == 4) {
        const std::array<GLfloat, 4> color_none{0.0F, 0.0F, 0.0F, 0.0F};
        const std::array<GLfloat, 4> color_blue{0.0F, 1.0F, 0.0F, 0.0F};
        glColor4fv(color_blue.data());
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color_blue.data());
        glMaterialfv(GL_FRONT, GL_SPECULAR, color_none.data());
    }

    glBegin(GL_QUADS);

    glNormal3f(0.0F, 0.0F, 2.0F);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f(-1.0F, -1.0F, 1.0F);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f(1.0F, -1.0F, 1.0F);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(1.0F, 1.0F, 1.0F);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(-1.0F, 1.0F, 1.0F);

    glNormal3f(0.0F, 0.0F, -2.0F);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f(-1.0F, -1.0F, -1.0F);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(-1.0F, 1.0F, -1.0F);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(1.0F, 1.0F, -1.0F);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f(1.0F, -1.0F, -1.0F);

    glNormal3f(0.0F, -2.0F, 0.0F);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(-1.0F, -1.0F, -1.0F);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(1.0F, -1.0F, -1.0F);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f(1.0F, -1.0F, 1.0F);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f(-1.0F, -1.0F, 1.0F);

    glNormal3f(2.0F, 0.0F, 0.0F);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f(1.0F, -1.0F, -1.0F);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(1.0F, 1.0F, -1.0F);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(1.0F, 1.0F, 1.0F);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f(1.0F, -1.0F, 1.0F);

    glNormal3f(-2.0F, 0.0F, 0.0F);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f(-1.0F, -1.0F, -1.0F);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f(-1.0F, -1.0F, 1.0F);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(-1.0F, 1.0F, 1.0F);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(-1.0F, 1.0F, -1.0F);

    if (color == 0) {
        glEnd();

        glBindTexture(GL_TEXTURE_2D, session.textures[1]);
        glBegin(GL_QUADS);
    }

    glNormal3f(0.0F, 2.0F, 0.0F);
    glTexCoord2f(0.0F, 1.0F);
    glVertex3f(-1.0F, 1.0F, -1.0F);
    glTexCoord2f(0.0F, 0.0F);
    glVertex3f(-1.0F, 1.0F, 1.0F);
    glTexCoord2f(1.0F, 0.0F);
    glVertex3f(1.0F, 1.0F, 1.0F);
    glTexCoord2f(1.0F, 1.0F);
    glVertex3f(1.0F, 1.0F, -1.0F);

    glEnd();

    usleep(DEFAULT_SLEEP_TIME);
}

void apply_transformations(float indx, float indy, [[maybe_unused]] float z) {
    const int steps_x = static_cast<int>(std::round(std::fabs(indx)));
    const int steps_y = static_cast<int>(std::round(std::fabs(indy)));

    const float step_x = indx < 0 ? -2.0F : 2.0F;
    const float step_y = indy < 0 ? 2.0F : -2.0F;

    for (int i = 0; i < steps_x; i++) {
        glTranslatef(step_x, 0.0F, 0.0F);
    }

    for (int i = 0; i < steps_y; i++) {
        glTranslatef(0.0F, 0.0F, step_y);
    }
}

void apply_transformation_general(float indx, float indy, float z) {
    glTranslatef(2 * indx, 10 * indy, 2 * z);
}

} // namespace graphics::mesh
