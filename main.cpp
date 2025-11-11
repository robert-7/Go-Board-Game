// Standard libraries
#include <array>
#include <cmath>
#include <iostream> // For std::cout
#include <list>
#include <numbers>
#include <unistd.h> // For usleep
#include <vector>

// includes, graphics
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <GL/glu.h>
// DevIL includes
#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

// Project includes
#include "game/GameSession.h"
#include "input/InputRouter.h"

constexpr GLenum GL_CLAMP_TO_EDGE_VALUE = 0x812F;
#if defined(__cpp_lib_math_constants)
constexpr float PI = std::numbers::pi_v<float>; // Pi constant for trigonometric helpers
#else
constexpr float PI = 3.14159265358979323846F; // NOLINT(modernize-use-std-numbers)
#endif

// float rm_array[1083];     // Holds the objects that will be removed.
// int p = 0;                // Always points to the last index of rm_array.

// declaration, forward
void display();
void display(GameSession &session);
void init(GameSession &session);
void idle();
void idle(GameSession &session);
void set_images(GameSession &session);
void draw_sphere(int color);
void draw_unit_cube(GameSession &session, int color);
void apply_transformations(float indx, float indy, float z);
void apply_transformation_general(float indx, float indy, float z);

// Behind the Scenes
void init_board(GameSession &session);
void clear_liberties(GameSession &session);
auto make_move(GameSession &session, int x, int y, int piece) -> int;
auto check_liberties(GameSession &session, int x, int y, int originx, int originy,
                     int piece) -> int;
void remove_block(GameSession &session, int x, int y, int piece);
void jump_off(GameSession &session, int x0, int y0, int color);

//! Program entry point
auto main(int argc, char **argv) -> int {
    const int default_window_width = 768;
    const int default_window_height = 768;
    const int default_window_x = 256;
    const int default_window_y = 256;
    const char *window_title = "Go Board Game";
    // initialize GLUT
    glutInit(&argc, argv);
    // set visual
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    // set physical window properties
    glutInitWindowSize(default_window_width, default_window_height);
    // position window on the screen
    glutInitWindowPosition(default_window_x, default_window_y);
    auto &session = current_session();

    // create window
    session.window_id = glutCreateWindow(window_title);

    ilInit();
    iluInit();
    ilutInit();
    ilutRenderer(ILUT_OPENGL);

    ilGenImages(2, session.image_ids.data());
    glGenTextures(2, session.textures.data());

    // initialize OpenGL
    init(session);

    // register rendering mainloop
    input::register_callbacks();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}

void idle() {
    auto &session = current_session();
    idle(session);
}

void idle(GameSession &session) {
    const float board_rotation_step = 0.5f;
    const float angle_limit = 360.0f;
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

    /*
    if (!session.pause_lighting) {
    session.translate_light += board_rotation_step;

    if (session.translate_light > 360) {
        session.translate_light -= 360;
    }
    }
    */

    glutPostRedisplay();
}

void lighting_func(const GameSession &session) {
    // Specify light position
    const std::array<GLfloat, 4> light_position{0.0F, 0.0F, -0.5F, 1.0F};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position.data());

    // Specify diffuse component.  Diffuse component is white.
    // GLfloat lightdiffuse[4] = {0.0, 0.0, 0.0, 1.0};
    const std::array<GLfloat, 4> light_strength{2.0F, 2.0F, 2.0F, 2.0F};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_strength.data());

    // Specify specular component.  Specular component is green.
    // TODO: Decide on desired specular component -- IF NEEDED
    // GLfloat lightspecular[4] = {0.0, 0.0, 0.0, 1.0};
    // GLfloat lightspecular[4] = { 0.0, 2.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_strength.data());

    GLfloat spread = 90.0F - 90.0F * session.animation_time;
    glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &spread);

    glEnable(GL_LIGHT0);
}

void material_func() {
    // Add a diffuse component to our board.  The diffuse reflection constant
    // is white (white light source produces white reflection).
    const std::array<GLfloat, 4> material_diffuse{0.5F, 0.5F, 1.0F, 1.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse.data());

    // Add a specular component to our board.  The specular reflection constant
    // is white (white light source produces white reflection).
    const std::array<GLfloat, 4> material_specular{1.0F, 1.0F, 1.0F, 1.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular.data());

    // Defines the shinyness (the exponent to the phong model).
    const std::array<GLfloat, 1> material_shininess{100.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess.data());
}

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
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();              // Create A Pointer To The Quadric Object
    gluQuadricNormals(quadratic, GLU_SMOOTH); // Create Smooth Normals
    gluQuadricTexture(quadratic, GL_TRUE);    // Create Texture Coords

    gluSphere(quadratic, 1.0f, 32, 32);
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

    // Front Face
    glNormal3f(0.0f, 0.0f, 2.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    // Back Face
    glNormal3f(0.0f, 0.0f, -2.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    // Bottom Face
    glNormal3f(0.0f, -2.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    // Right face
    glNormal3f(2.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    // Left Face
    glNormal3f(-2.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);

    if (color == 0) {
        glEnd();

        glBindTexture(GL_TEXTURE_2D, session.textures[1]);
        glBegin(GL_QUADS);
    }

    // Top Face
    glNormal3f(0.0f, 2.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);

    glEnd();

    usleep(DEFAULT_SLEEP_TIME);
}

void display() {
    auto &session = current_session();
    display(session);
}

void display(GameSession &session) {
    // clear screen to background color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw a white quad
    glColor4f(1.0, 1.0, 1.0, 1.0);

    // Push lighting bit; we'll pop it later so that the lighting state
    // isn't saved onto the next frame.  We do this so that we can disable
    // lighting more easily.
    glPushAttrib(GL_LIGHTING_BIT);

    glLoadIdentity();

    // Specify camera transformation
    glTranslatef(static_cast<GLfloat>(session.camera.x),
                 static_cast<GLfloat>(session.camera.y),
                 static_cast<GLfloat>(session.camera.z));

    // Specify the lighting we'll be using for this app.  Note that
    // the lights can be transformed using the usual translate, rotate,
    // and scale commands.
    glPushMatrix();

    //////////////////// MOVING LIGHT!!!!! ////////////////////
    glTranslatef(2.0F * std::sin(session.translate_light * 2.0F * PI / 360.0F), 0.0F,
                 0.0F);

    if (session.lighting) {
        lighting_func(session);
    }

    glPopMatrix();

    glTranslatef(0.0, 0.0, -2.0);              // Move objects to viewing area.
    glRotatef(session.angle_x, 1.0, 0.0, 0.0); // To allow rotation on the x-axis
    glRotatef(session.angle_y, 0.0, 1.0, 0.0); // To allow rotation on the y-axis
    glPushMatrix();

    glScalef(1.0, 0.05, 1.0); // Flatten the Cube.
                              // It should look like a board now.

    // Specify material for the board we'll be drawing.
    if (session.material) {
        material_func();
    }

    draw_unit_cube(session, 0);

    glPopMatrix();
    glPushMatrix();
    // Draw Beads

    glBindTexture(GL_TEXTURE_2D, 0);

    glScalef(0.0525, 0.0525, 0.0525); // Just the right size :)
    glTranslatef(0.0, 1.0, 0.0);      // The centre

    glPushMatrix();
    apply_transformations(static_cast<float>(session.place_x),
                          static_cast<float>(session.place_y), 0);
    draw_sphere(0);
    glPopMatrix();

    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            if (session.board_status[i][j] != 0) {
                glPushMatrix();
                apply_transformations(static_cast<float>(i - 9),
                                      static_cast<float>(j - 9), 0);
                draw_sphere(session.board_status[i][j]);
                glPopMatrix();
            }
        }
    }

    // THE REMOVAL OF PIECE PORTION
    if (!session.captured_groups.empty()) {
        for (const auto &piece : session.captured_groups) {
            jump_off(session, piece[0], piece[1], piece[2]);
        }
        session.animation_time +=
            (TIME_INCREMENT * static_cast<float>(session.captured_groups.size()));
    }
    if (session.animation_time > 1.0F) {
        session.animation_time = 0.0F;
        session.captured_groups.clear();
    }

    glPopMatrix();
    glPopAttrib();

    // immediately process commands
    glFlush();

    // immediately call display loop again
    glutSwapBuffers();
}

void apply_transformations(float indx, float indy, [[maybe_unused]] float z) {
    int i;
    if (indx < 0) {
        for (i = 0; i < std::abs(indx); i++) {
            glTranslatef(-2.0, 0.0, 0.0);
        }
    } else {
        for (i = 0; i < indx; i++) {
            glTranslatef(2.0, 0.0, 0.0);
        }
    }
    if (indy < 0) {
        for (i = 0; i < std::abs(indy); i++) {
            glTranslatef(0.0, 0.0, 2.0);
        }
    } else {
        for (i = 0; i < indy; i++) {
            glTranslatef(0.0, 0.0, -2.0);
        }
    }
}

/*
We can probably get rid of the first method,
but I don't feel like doing that.
*/

void apply_transformation_general(float indx, float indy, float z) {
    glTranslatef(2 * indx, 10 * indy, 2 * z);
}

void set_images(GameSession &session) {
    ///////////////////// LOAD WOODEN TEXTURE /////////////////////
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

    ///////////////////// LOAD CHECKERBOARD TEXTURE /////////////////
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

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// BEHIND THE SCENES /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void init_board(GameSession &session) {
    for (auto &column : session.board_status) {
        column.fill(0);
    }
}

void clear_liberties(GameSession &session) {
    for (auto &column : session.liberties_status) {
        column.fill(-1);
    }
}

auto check_liberties(GameSession &session, int x, int y, int originx, int originy,
                     int piece) -> int {
    if (x < 0 || x > 18 || y < 0 || y > 18) {
        return 0;
    }
    if (session.board_status[x][y] == 0) {
        return 1;
    }
    if (session.board_status[x][y] != piece) {
        return 0;
    }
    if (session.liberties_status[x][y] != -1) {
        return session.liberties_status[x][y];
    }
    int direcx = x - originx;
    int direcy = y - originy;
    if ((direcx >= 0 && check_liberties(session, x + 1, y, originx, originy, piece)) ||
        (direcx <= 0 && check_liberties(session, x - 1, y, originx, originy, piece)) ||
        (direcy >= 0 && check_liberties(session, x, y + 1, originx, originy, piece)) ||
        (direcy <= 0 && check_liberties(session, x, y - 1, originx, originy, piece))) {
        session.liberties_status[x][y] = 1;
    } else {
        session.liberties_status[x][y] = 0;
    }
    return session.liberties_status[x][y];
}

void jump_off(GameSession &session, int x0, int z0, int color) {
    // For now, let's just assume they're all jumping to point (-4,0,-4)

    int x1 = -4;
    float xt = x0 * (1 - session.animation_time) + x1 * session.animation_time;
    int y0 = 9;
    float yt =
        -y0 * std::pow(session.animation_time, 2.0F) + y0 * session.animation_time + y0;
    int z1 = -4;
    float zt = z0 * (1 - session.animation_time) + z1 * session.animation_time;

    glPushMatrix();
    apply_transformation_general((xt - 9), (yt - 9), -(zt - 9));
    draw_sphere(color);

    // Draw Thigh
    glPushMatrix();
    if (session.animation_time < 0.1F) {
        glRotatef(-80 + 80 * std::sin(session.animation_time * 10.0F * PI), 1.0, 0.0,
                  1.0);
    } else {
        glRotatef(-80, 1.0, 0.0, 1.0);
    }
    glScalef(2.0 / 5.0, 1.0, 2.0 / 5.0);
    glTranslatef(0.0, -1.0, 0.0);
    draw_unit_cube(session, 3);
    glScalef(5.0 / 2.0, 1.0, 5.0 / 2.0);

    // Draw Shin
    glPushMatrix();

    glTranslatef(0, -0.8, 0);
    if (session.animation_time < 0.1F) {
        glRotatef(90 - 90 * std::sin(session.animation_time * 10.0F * PI), 1.0, 0.0,
                  1.0);
    } else {
        glRotatef(90, 1.0, 0.0, 1.0);
    }
    glScalef(1.0 / 5.0, 1.0, 1.0 / 5.0);
    glTranslatef(0, -1.0, 0);
    draw_unit_cube(session, 4);
    glScalef(5.0, 1.0, 5.0);

    // Draw Ankle
    glPushMatrix();
    glTranslatef(0, -1.0, 0);
    if (session.animation_time < 0.1F) {
        glRotatef(-100 + 100 * std::sin(session.animation_time * 10.0F * PI), 1.0, 0.0,
                  1.0);
    } else {
        glRotatef(-100, 1.0, 0.0, 1.0);
    }
    glScalef(1.0 / 2.0, 2.0 / 3.0, 1.0 / 2.0);
    glTranslatef(0, -1.0, 0);
    draw_sphere(0);
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
    glPopMatrix();
}

void remove_block(GameSession &session, int x, int y, int piece) {
    if (session.board_status[x][y] == piece) {
        session.board_status[x][y] = 0;
        std::cout << "Jump from ";
        std::cout << x;
        std::cout << y;
        std::cout << '\n';

        //     rm_queue.push(x);
        //     rm_queue.push(y);
        //     rm_queue.push(piece);

        std::vector<int> rm_piece;
        rm_piece.push_back(x);
        rm_piece.push_back(y);
        rm_piece.push_back(piece);
        session.captured_groups.push_back(rm_piece);

        remove_block(session, x - 1, y, piece);
        remove_block(session, x + 1, y, piece);
        remove_block(session, x, y - 1, piece);
        remove_block(session, x, y + 1, piece);
    }
}

auto make_move(GameSession &session, int x, int y, int piece) -> int {
    session.board_status[x][y] = piece;
    clear_liberties(session);
    int other;
    if (piece == 1) {
        other = 2;
    } else {
        other = 1;
    }
    int has_liberties = 0;
    if (x > 0 && session.board_status[x - 1][y] != piece) {
        if (piece == 0) {
            has_liberties = 1;
        } else if (!check_liberties(session, x - 1, y, x - 1, y, other)) {
            has_liberties = 1;
            remove_block(session, x - 1, y, other);
        }
    }
    if (x < 18 && session.board_status[x + 1][y] != piece) {
        if (piece == 0) {
            has_liberties = 1;
        } else if (!check_liberties(session, x + 1, y, x + 1, y, other)) {
            has_liberties = 1;
            remove_block(session, x + 1, y, other);
        }
    }
    if (y > 0 && session.board_status[x][y - 1] != piece) {
        if (piece == 0) {
            has_liberties = 1;
        } else if (!check_liberties(session, x, y - 1, x, y - 1, other)) {
            has_liberties = 1;
            remove_block(session, x, y - 1, other);
        }
    }
    if (y < 18 && session.board_status[x][y + 1] != piece) {
        if (piece == 0) {
            has_liberties = 1;
        } else if (!check_liberties(session, x, y + 1, x, y + 1, other)) {
            has_liberties = 1;
            remove_block(session, x, y + 1, other);
        }
    }
    if (!has_liberties && !check_liberties(session, x, y, x, y, piece)) {
        remove_block(session, x, y, piece);
        return 0;
    }
    return 1;
}

void init(GameSession &session) {
    // select clearing color
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // initialize projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double fov_y = 45;
    double aspect = 1.0;
    double z_near = 0.1;
    double z_far = 100;

    gluPerspective(fov_y,  // Field of view
                   aspect, // Aspect ratio (width / height)
                   z_near, // Near plane
                   z_far); // Far plane

    // initialize modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_LIGHTING);   // enable lighting
    glEnable(GL_CULL_FACE);  // enable backface culling
    glEnable(GL_DEPTH_TEST); // enable depth testing
                             // glEnable(GL_TEXTURE_2D); // enable textures

    set_images(session);

    init_board(session); // Create the "Board"
}
