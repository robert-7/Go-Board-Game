#include "input/InputRouter.h"

#include <GL/freeglut_std.h>
#include <GL/gl.h>

#include <iostream>

#include "game/GameSession.h"
#include "game/Rules.h"

namespace {

constexpr float ZOOM_SCALE = 0.01F;

void keyboard(GameSession &session, unsigned char key, [[maybe_unused]] int x,
              [[maybe_unused]] int y);
void mouse(GameSession &session, int button, int state, int x, int y);
void motion(GameSession &session, int x, int y);

void keyboard_callback(unsigned char key, int x, int y) {
    auto &session = current_session();
    keyboard(session, key, x, y);
}

void mouse_callback(int button, int state, int x, int y) {
    auto &session = current_session();
    mouse(session, button, state, x, y);
}

void motion_callback(int x, int y) {
    auto &session = current_session();
    motion(session, x, y);
}

void keyboard(GameSession &session, unsigned char key, [[maybe_unused]] int x,
              [[maybe_unused]] int y) {
    switch (key) {
    case '1':
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

    case '\r': {
        const auto &stones = session.board.stones();
        if (stones[session.place_x + BOARD_CENTER][session.place_y + BOARD_CENTER] !=
            0) {
            std::cout << "YOU CAN'T PLACE A PIECE HERE BECAUSE THERE ALREADY IS A "
                      << "PIECE HERE!!!\n";
        } else {
            std::cout << "ENTER KEY PRESSED!!!\n";
            rules::make_move(session, session.place_x + BOARD_CENTER,
                             session.place_y + BOARD_CENTER, session.stone_color);
            if (session.stone_color == 1) {
                session.stone_color = 2;
            } else {
                session.stone_color = 1;
            }
        }
        break;
    }
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
            rules::init_board(session);
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

void mouse(GameSession &session, int button, int state, int x, int y) {
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

void motion(GameSession &session, int x, int y) {
    if (session.update_cam_z_pos) {
        session.camera.z += (y - session.last_y) * ZOOM_SCALE;
        session.camera.x += (x - session.last_x) * ZOOM_SCALE;
        session.last_x = x;
        session.last_y = y;

        glutSetWindow(session.window_id);
        glutPostRedisplay();
    }
}

} // namespace

namespace input {

void register_callbacks() {
    glutKeyboardFunc(keyboard_callback);
    glutMouseFunc(mouse_callback);
    glutMotionFunc(motion_callback);
}

} // namespace input
