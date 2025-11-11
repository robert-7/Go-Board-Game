// Standard libraries
#include <iostream> // For std::cout
#include <list>
#include <vector>

// includes, graphics
#include <GL/freeglut_std.h>
#include <GL/gl.h>
// DevIL includes
#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

// Project includes
#include "game/GameSession.h"
#include "graphics/Renderer.h"
#include "input/InputRouter.h"

// float rm_array[1083];     // Holds the objects that will be removed.
// int p = 0;                // Always points to the last index of rm_array.

// declaration, forward

// Behind the Scenes
void init_board(GameSession &session);
void clear_liberties(GameSession &session);
auto make_move(GameSession &session, int x, int y, int piece) -> int;
auto check_liberties(GameSession &session, int x, int y, int originx, int originy,
                     int piece) -> int;
void remove_block(GameSession &session, int x, int y, int piece);

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
    init_board(session);

    // register rendering mainloop
    graphics::register_callbacks();
    input::register_callbacks();
    glutMainLoop();

    return 0;
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
