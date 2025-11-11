// includes, graphics
#include <GL/freeglut_std.h>
#include <GL/gl.h>
#include <array>
// DevIL includes
#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

// Project includes
#include "game/GameSession.h"
#include "game/Rules.h"
#include "graphics/Renderer.h"
#include "input/InputRouter.h"

// float rm_array[1083];     // Holds the objects that will be removed.
// int p = 0;                // Always points to the last index of rm_array.

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
    graphics::initialize(session);
    rules::init_board(session);

    // register rendering mainloop
    graphics::register_callbacks();
    input::register_callbacks();
    glutMainLoop();

    return 0;
}
