#include "game/Rules.h"

#include <array>
#include <iostream>
#include <utility>
#include <vector>

#include "game/GameSession.h"

namespace {

void init_board_state(GameSession &session) {
    for (auto &column : session.board_status) {
        column.fill(0);
    }
}

void clear_liberties(GameSession &session) {
    for (auto &column : session.liberties_status) {
        column.fill(-1);
    }
}

void remove_block(GameSession &session, int x, int y, int piece) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return;
    }
    if (session.board_status[x][y] != piece) {
        return;
    }

    session.board_status[x][y] = 0;
    std::cout << "Jump from " << x << y << '\n';

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

auto check_liberties(GameSession &session, int x, int y, int originx, int originy,
                     int piece) -> int {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
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

    const int direcx = x - originx;
    const int direcy = y - originy;
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

} // namespace

namespace rules {

void init_board(GameSession &session) { init_board_state(session); }

auto make_move(GameSession &session, int x, int y, int piece) -> int {
    session.board_status[x][y] = piece;
    clear_liberties(session);

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

        if (session.board_status[nx][ny] == piece) {
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
