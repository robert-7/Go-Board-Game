#include "game/Rules.h"

#include <array>
#include <iostream>
#include <utility>

#include "game/GameSession.h"

namespace {

void remove_block(GameSession &session, int x, int y, int piece) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return;
    }
    auto &stones = session.board.stones();
    if (stones[x][y] != piece) {
        return;
    }

    stones[x][y] = 0;
    std::cout << "Jump from " << x << y << '\n';

    Board::CaptureGroup rm_piece{x, y, piece};
    session.board.add_captured_group(std::move(rm_piece));

    remove_block(session, x - 1, y, piece);
    remove_block(session, x + 1, y, piece);
    remove_block(session, x, y - 1, piece);
    remove_block(session, x, y + 1, piece);
}

auto check_liberties(GameSession &session, int x, int y, int originx, int originy,
                     int piece) -> int {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return 0;
    }
    auto &stones = session.board.stones();
    if (stones[x][y] == 0) {
        return 1;
    }
    if (stones[x][y] != piece) {
        return 0;
    }
    auto &liberties = session.board.liberties();
    if (liberties[x][y] != -1) {
        return liberties[x][y];
    }

    const int direcx = x - originx;
    const int direcy = y - originy;
    if ((direcx >= 0 && check_liberties(session, x + 1, y, originx, originy, piece)) ||
        (direcx <= 0 && check_liberties(session, x - 1, y, originx, originy, piece)) ||
        (direcy >= 0 && check_liberties(session, x, y + 1, originx, originy, piece)) ||
        (direcy <= 0 && check_liberties(session, x, y - 1, originx, originy, piece))) {
        liberties[x][y] = 1;
    } else {
        liberties[x][y] = 0;
    }
    return liberties[x][y];
}

} // namespace

namespace rules {

void init_board(GameSession &session) { session.board.clear(); }

auto make_move(GameSession &session, int x, int y, int piece) -> int {
    auto &board = session.board;
    auto &stones = board.stones();

    stones[x][y] = piece;
    board.clear_liberties();

    const int other = piece == 1 ? 2 : 1;
    int has_liberties = 0;

    const std::array<std::pair<int, int>, 4> neighbors{{
        {x - 1, y},
        {x + 1, y},
        {x, y - 1},
        {x, y + 1},
    }};

    for (const auto &[nx, ny] : neighbors) {
        if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) {
            continue;
        }

        if (stones[nx][ny] == piece) {
            continue;
        }

        if (piece == 0) {
            has_liberties = 1;
        } else if (!check_liberties(session, nx, ny, nx, ny, other)) {
            has_liberties = 1;
            remove_block(session, nx, ny, other);
        }
    }

    if (!has_liberties && !check_liberties(session, x, y, x, y, piece)) {
        remove_block(session, x, y, piece);
        return 0;
    }

    return 1;
}

} // namespace rules
