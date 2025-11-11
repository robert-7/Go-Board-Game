#include "graphics/Renderer.h"

#include <cmath>
#include <numbers>

#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <IL/il.h>

#include "game/GameSession.h"
#include "graphics/Lights.h"
#include "graphics/Meshes.h"

namespace {

constexpr GLenum GL_CLAMP_TO_EDGE_VALUE = 0x812F;
#if defined(__cpp_lib_math_constants)
constexpr float PI = std::numbers::pi_v<float>;
#else
constexpr float PI = 3.14159265358979323846F; // NOLINT(modernize-use-std-numbers)
#endif

void set_images(GameSession &session) {
    ilBindImage(session.image_ids[0]);
    ilLoadImage("images/wooden.jpg");
    ilConvertImage(IL_RGB, IL_UNSIGNED_SHORT);

    glBindTexture(GL_TEXTURE_2D, session.textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    static_cast<GLfloat>(GL_CLAMP_TO_EDGE_VALUE));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    static_cast<GLfloat>(GL_CLAMP_TO_EDGE_VALUE));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH),
                 ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_SHORT,
                 ilGetData());

    ilBindImage(session.image_ids[1]);
    ilLoadImage("images/checkerboard.png");
    ilConvertImage(IL_RGB, IL_UNSIGNED_SHORT);

    glBindTexture(GL_TEXTURE_2D, session.textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    static_cast<GLfloat>(GL_CLAMP_TO_EDGE_VALUE));
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    static_cast<GLfloat>(GL_CLAMP_TO_EDGE_VALUE));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ilGetInteger(IL_IMAGE_WIDTH),
                 ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGB, GL_UNSIGNED_SHORT,
                 ilGetData());

    glEnable(GL_NORMALIZE);
}

void jump_off(GameSession &session, int x0, int z0, int color) {
    const float animation = session.state.animation_time();
    const bool early_animation = animation < 0.1F;

    int x1 = -4;
    const float xt = x0 * (1 - animation) + static_cast<float>(x1) * animation;
    int y0 = 9;
    const auto y0_float = static_cast<float>(y0);
    const float yt =
        -y0_float * std::pow(animation, 2.0F) + y0_float * animation + y0_float;
    int z1 = -4;
    const float zt = z0 * (1 - animation) + static_cast<float>(z1) * animation;

    glPushMatrix();
    graphics::mesh::apply_transformation_general((xt - 9), (yt - 9), -(zt - 9));
    graphics::mesh::draw_sphere(color);

    glPushMatrix();
    const float thigh_angle =
        early_animation ? -80 + 80 * std::sin(animation * 10.0F * PI) : -80;
    glRotatef(thigh_angle, 1.0F, 0.0F, 1.0F);
    glScalef(2.0F / 5.0F, 1.0F, 2.0F / 5.0F);
    glTranslatef(0.0F, -1.0F, 0.0F);
    graphics::mesh::draw_unit_cube(session, 3);
    glScalef(5.0F / 2.0F, 1.0F, 5.0F / 2.0F);

    glPushMatrix();

    glTranslatef(0.0F, -0.8F, 0.0F);
    const float shin_angle =
        early_animation ? 90 - 90 * std::sin(animation * 10.0F * PI) : 90;
    glRotatef(shin_angle, 1.0F, 0.0F, 1.0F);
    glScalef(1.0F / 5.0F, 1.0F, 1.0F / 5.0F);
    glTranslatef(0.0F, -1.0F, 0.0F);
    graphics::mesh::draw_unit_cube(session, 4);
    glScalef(5.0F, 1.0F, 5.0F);

    glPushMatrix();
    glTranslatef(0.0F, -1.0F, 0.0F);
    const float ankle_angle =
        early_animation ? -100 + 100 * std::sin(animation * 10.0F * PI) : -100;
    glRotatef(ankle_angle, 1.0F, 0.0F, 1.0F);
    glScalef(1.0F / 2.0F, 2.0F / 3.0F, 1.0F / 2.0F);
    glTranslatef(0.0F, -1.0F, 0.0F);
    graphics::mesh::draw_sphere(0);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void display(GameSession &session) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor4f(1.0F, 1.0F, 1.0F, 1.0F);

    glPushAttrib(GL_LIGHTING_BIT);
    glLoadIdentity();

    glTranslatef(static_cast<GLfloat>(session.camera.x),
                 static_cast<GLfloat>(session.camera.y),
                 static_cast<GLfloat>(session.camera.z));

    glPushMatrix();
    const float light_offset = std::sin(session.translate_light * 2.0F * PI / 360.0F);
    glTranslatef(2.0F * light_offset, 0.0F, 0.0F);

    if (session.lighting) {
        graphics::lights::apply(session);
    }

    glPopMatrix();

    glTranslatef(0.0F, 0.0F, -2.0F);
    glRotatef(session.angle_x, 1.0F, 0.0F, 0.0F);
    glRotatef(session.angle_y, 0.0F, 1.0F, 0.0F);
    glPushMatrix();

    glScalef(1.0F, 0.05F, 1.0F);

    if (session.material) {
        graphics::lights::apply_material();
    }

    graphics::mesh::draw_unit_cube(session, 0);

    glPopMatrix();
    glPushMatrix();

    glBindTexture(GL_TEXTURE_2D, 0);

    glScalef(0.0525F, 0.0525F, 0.0525F);
    glTranslatef(0.0F, 1.0F, 0.0F);

    glPushMatrix();
    const int cursor_x = session.state.cursor_x();
    const int cursor_y = session.state.cursor_y();
    graphics::mesh::apply_transformations(static_cast<float>(cursor_x),
                                          static_cast<float>(cursor_y), 0.0F);
    graphics::mesh::draw_sphere(0);
    glPopMatrix();

    const auto &stones = session.board.stones();
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (stones[i][j] != 0) {
                glPushMatrix();
                graphics::mesh::apply_transformations(
                    static_cast<float>(i - BOARD_CENTER),
                    static_cast<float>(j - BOARD_CENTER), 0.0F);
                graphics::mesh::draw_sphere(stones[i][j]);
                glPopMatrix();
            }
        }
    }

    const auto &captured = session.board.captured_groups();
    if (!captured.empty()) {
        for (const auto &piece : captured) {
            jump_off(session, piece[0], piece[1], piece[2]);
        }
        session.state.advance_animation(TIME_INCREMENT *
                                        static_cast<float>(captured.size()));
    }
    if (session.state.animation_time() > 1.0F) {
        session.state.reset_animation();
        session.board.clear_captured_groups();
    }

    glPopMatrix();
    glPopAttrib();

    glFlush();
    glutSwapBuffers();
}

void idle(GameSession &session) {
    const float board_rotation_step = 0.5F;
    const float angle_limit = 360.0F;
    if (!session.pause_board_rotation_y) {
        session.angle_y += board_rotation_step;
        if (session.angle_y > angle_limit) {
            session.angle_y -= angle_limit;
        }
    }

    if (!session.pause_board_rotation_x) {
        session.angle_x += board_rotation_step;
        if (session.angle_x > angle_limit) {
            session.angle_x -= angle_limit;
        }
    }

    glutPostRedisplay();
}

void display_callback() {
    auto &session = current_session();
    display(session);
}

void idle_callback() {
    auto &session = current_session();
    idle(session);
}

} // namespace

namespace graphics {

void initialize(GameSession &session) {
    glClearColor(0.0F, 0.0F, 0.0F, 0.0F);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    constexpr double FOV_Y = 45.0;
    constexpr double ASPECT = 1.0;
    constexpr double Z_NEAR = 0.1;
    constexpr double Z_FAR = 100.0;

    gluPerspective(FOV_Y, ASPECT, Z_NEAR, Z_FAR);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    set_images(session);
}

void register_callbacks() {
    glutDisplayFunc(display_callback);
    glutIdleFunc(idle_callback);
}

} // namespace graphics
