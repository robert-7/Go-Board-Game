#ifndef GAME_SESSION_H
#define GAME_SESSION_H

#include <array>

#include <GL/gl.h>
#include <IL/il.h>

#include "game/Board.h"

constexpr int BOARD_SIZE = Board::SIZE;
constexpr int BOARD_CENTER = Board::CENTER;

// Camera initial position
constexpr GLdouble INITIAL_CAM_X = 0.0;
constexpr GLdouble INITIAL_CAM_Y = 0.0;
constexpr GLdouble INITIAL_CAM_Z = -1.5;

struct Camera {
    GLdouble x;
    GLdouble y;
    GLdouble z;
};

constexpr int INITIAL_BOARD_ANGLE_X = 45;
constexpr int INITIAL_BOARD_ANGLE_Y = 0;
constexpr float TIME_INCREMENT = 0.002F;
constexpr int DEFAULT_SLEEP_TIME = 3000;

class GameSession {
  public:
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

    int restart_option = 0;
    Board board;
};

auto current_session() -> GameSession &;

#endif // GAME_SESSION_H
