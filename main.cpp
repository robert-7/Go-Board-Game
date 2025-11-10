// Standard libraries
#include <array>
#include <cmath>
#include <iostream> // For std::cout
#include <list>
#if defined(__has_include)
#if __has_include(<numbers>)
#include <numbers>
#endif
#endif
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

constexpr GLenum GL_CLAMP_TO_EDGE_VALUE = 0x812F;

// Camera methods
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

const float ZOOM_SCALE = 0.01F;

constexpr int BOARD_SIZE = 19;
constexpr int BOARD_CENTER = BOARD_SIZE / 2;

// Global Constants
// Camera initial position
constexpr GLdouble INITIAL_CAM_X = 0.0;
constexpr GLdouble INITIAL_CAM_Y = 0.0;
constexpr GLdouble INITIAL_CAM_Z = -1.5;
#if defined(__cpp_lib_math_constants)
constexpr float PI = std::numbers::pi_v<float>; // Pi constant for trigonometric helpers
#else
constexpr float PI = 3.14159265358979323846F; // NOLINT(modernize-use-std-numbers)
#endif

struct Camera {
    GLdouble x;
    GLdouble y;
    GLdouble z;
};

constexpr int INITIAL_BOARD_ANGLE_X = 45;
constexpr int INITIAL_BOARD_ANGLE_Y = 0;
constexpr float TIME_INCREMENT = 0.002F; // Depicts how fast time increments.
constexpr int DEFAULT_SLEEP_TIME = 3000; // Default sleep time in microseconds.

struct GameSession {
    std::array<ILuint, 3> image_ids{0, 1, 2};
    std::array<GLuint, 3> textures{0, 1, 2};

    int window_id = 0; // Glut window ID (for display)

    bool update_cam_z_pos = false;
    int last_x = 0;
    int last_y = 0;

    int wireframe = 0;
    int lighting = 1;
    int material = 1;
    int pause_board_rotation_y = 1;
    int pause_board_rotation_x = 1;
    int pause_lighting = 0;
    int depth_test = 1;
    int cull_face = 1;
    int smooth_shading = 1;
    int enable_texture = 0;

    float angle_x = static_cast<float>(INITIAL_BOARD_ANGLE_X);
    float angle_y = static_cast<float>(INITIAL_BOARD_ANGLE_Y);
    float translate_light = 0.0F;

    Camera camera{.x = INITIAL_CAM_X, .y = INITIAL_CAM_Y, .z = INITIAL_CAM_Z};

    int place_x = 0;
    int place_y = 0;
    int stone_color = 1;
    float animation_time = 0.0F;

    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> board_status{};
    std::array<std::array<int, BOARD_SIZE>, BOARD_SIZE> liberties_status{};
    int restart_option = 0;
    std::list<std::vector<int>> captured_groups;
};

auto current_session() -> GameSession & {
    static GameSession session;
    return session;
}

// float rm_array[1083];     // Holds the objects that will be removed.
// int p = 0;                // Always points to the last index of rm_array.

// declaration, forward
void display();
void display(GameSession &session);
void keyboard(unsigned char, int, int);
void keyboard(GameSession &session, unsigned char, int, int);
void mouse(int button, int state, int x, int y);
void mouse(GameSession &session, int button, int state, int x, int y);
void motion(int x, int y);
void motion(GameSession &session, int x, int y);
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
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMouseFunc(mouse);   // Call mouse whenever mouse button pressed
    glutMotionFunc(motion); // Call motion whenever mouse moves while button
                            // pressed let's rock ...
    glutMainLoop();

    return 0;
}

void keyboard(unsigned char key, [[maybe_unused]] int x, [[maybe_unused]] int y) {
    auto &session = current_session();
    keyboard(session, key, x, y);
}

void keyboard(GameSession &session, unsigned char key, [[maybe_unused]] int x,
              [[maybe_unused]] int y) {
    /*

    1 : Toggle specularity
    2 : Toggle lighting
    3 : Toggle wireframe
    4 : Toggle light animation
    5 : Toggle depth testing
    6 : Toggle culling (try disabling both depth testing and culling)
    7 : Toggle between smooth and flat shading
    8 : Toggle texture

    o,O : Toggle board rotation along the y-axis
    p,P : Toggle board rotation along the x-axis
    r,R : Activate "reset board option"
    y,Y : Confirm  "reset board option"
    n,N : Cancel   "reset board option"
    v,V : Set color to white (This makes testing a bit faster)
    b,B : Set color to black (This makes testing a bit faster)

    wasd  : Move a piece around the board
    enter : Place a piece on the board

    ON RIGHT CLICK AND HOLD MOUSE:
    slide left  : move camera left
    slide right : move camera right
    slide up    : zoom camera in
    slide down  : zoom camera out
    */

    switch (key) {
    case '1':
        // Enable/disable wireframe mode
        session.wireframe = !session.wireframe;

        if (session.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        break;

    case '3':
        session.material = !session.material;

        break;

    case '5':
        session.depth_test = !session.depth_test;

        if (session.depth_test) {
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        break;

    case '6':
        session.cull_face = !session.cull_face;

        if (session.cull_face) {
            glEnable(GL_CULL_FACE);
        } else {
            glDisable(GL_CULL_FACE);
        }

        break;

    case '7':
        session.smooth_shading = !session.smooth_shading;

        if (session.smooth_shading) {
            glShadeModel(GL_SMOOTH);
        } else {
            glShadeModel(GL_FLAT);
        }

        break;

    case '8':
        session.enable_texture = !session.enable_texture;

        if (session.enable_texture) {
            glEnable(GL_TEXTURE_2D);
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        break;

    case 'o':
    case 'O':
        std::cout << "PRESSED O\n";
        session.pause_board_rotation_y = !session.pause_board_rotation_y;

        break;

    case 'p':
    case 'P':
        std::cout << "PRESSED P\n";
        session.pause_board_rotation_x = !session.pause_board_rotation_x;

        break;

    case 'a':
    case 'A':
        if (session.place_x > -BOARD_CENTER) {
            session.place_x -= 1;
        }

        break;

    case 'w':
    case 'W':
        if (session.place_y < BOARD_CENTER) {
            session.place_y += 1;
        }

        break;
    case 's':
    case 'S':
        if (session.place_y > -BOARD_CENTER) {
            session.place_y -= 1;
        }

        break;
    case 'd':
    case 'D':
        if (session.place_x < BOARD_CENTER) {
            session.place_x += 1;
        }

        break;

    case '\r':
        if (session.board_status[session.place_x + BOARD_CENTER]
                                [session.place_y + BOARD_CENTER] != 0) {
            std::cout << "YOU CAN'T PLACE A PIECE HERE BECAUSE THERE ALREADY IS A "
                         "PIECE HERE!!!\n";
        } else {
            std::cout << "ENTER KEY PRESSED!!!\n";
            make_move(session, session.place_x + BOARD_CENTER,
                      session.place_y + BOARD_CENTER, session.stone_color);
            if (session.stone_color == 1) {
                session.stone_color = 2;
            } else {
                session.stone_color = 1;
            }
        }
        break;
    case 'r':
    case 'R':
        std::cout << "You pressed 'r', the restart button. Press 'y' to confirm "
                     "restart. Press 'n' to cancel.\n";
        session.restart_option = 1;
        break;
    case 'n':
    case 'N':
        if (session.restart_option) {
            std::cout << "You pressed 'n'. Restart option cancelled.\n";
            session.restart_option = 0;
        }
        break;
    case 'y':
    case 'Y':
        if (session.restart_option) {
            std::cout << "You pressed 'y'. The game has been restarted.\n";
            init_board(session);
            session.stone_color = 1;
        }
        break;

    case 'b':
    case 'B':
        session.stone_color = 2;
        break;
    case 'v':
    case 'V':
        session.stone_color = 1;
        break;
    }
}

// Handles mouse motion events while a button is pressed
void motion(int x, int y) {
    auto &session = current_session();
    motion(session, x, y);
}

void motion(GameSession &session, int x, int y) {
    // If the RMB is pressed and dragged then zoom in / out
    if (session.update_cam_z_pos) {
        // Update camera position while the mouse is dragged
        session.camera.z += (y - session.last_y) * ZOOM_SCALE;
        session.camera.x += (x - session.last_x) * ZOOM_SCALE;
        session.last_x = x;
        session.last_y = y;

        // Redraw the scene from updated camera position
        glutSetWindow(session.window_id);
        glutPostRedisplay();
    }
}

// Handles mouse button pressed / released events
void mouse(int button, int state, int x, int y) {
    auto &session = current_session();
    mouse(session, button, state, x, y);
}

void mouse(GameSession &session, int button, int state, int x, int y) {
    // If the RMB is pressed and dragged then zoom in / out
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            session.last_x = x;
            session.last_y = y;
            session.update_cam_z_pos = true;
        } else {
            session.update_cam_z_pos = false;
        }
    }
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
