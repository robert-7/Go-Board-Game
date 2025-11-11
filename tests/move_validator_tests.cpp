#include "game/Board.h"
#include "game/GameSession.h"
#include "game/Move.h"
#include "game/MoveValidator.h"
#include "game/Rules.h"

#include <gtest/gtest.h>

namespace {

auto make_session() -> GameSession {
    GameSession session;
    rules::init_board(session);
    session.board.clear_captured_groups();
    return session;
}

} // namespace

TEST(MoveValidatorTest, RejectsOutOfBoundsMove) {
    auto session = make_session();
    const Move move{.point = Point{Board::CENTER + 1, 0}, .stone = Stone::Black};

    EXPECT_FALSE(MoveValidator::is_legal(session, move));
}

TEST(MoveValidatorTest, RejectsMoveOnOccupiedPoint) {
    auto session = make_session();
    const Point point{0, 0};

    ASSERT_EQ(rules::make_move(session, point, Stone::Black), 1);

    const Move attempted_move{.point = point, .stone = Stone::White};

    EXPECT_FALSE(MoveValidator::is_legal(session, attempted_move));
}

TEST(MoveValidatorTest, AllowsMoveThatCapturesOpponent) {
    auto session = make_session();
    auto &board = session.board;

    const Point white{0, 0};
    board.place_stone(white, Stone::White);
    board.place_stone(Point{1, 0}, Stone::Black);
    board.place_stone(Point{-1, 0}, Stone::Black);
    board.place_stone(Point{0, 1}, Stone::Black);

    const Move capturing_move{.point = Point{0, -1}, .stone = Stone::Black};

    EXPECT_TRUE(MoveValidator::is_legal(session, capturing_move));
}

TEST(MoveValidatorTest, RejectsSelfCaptureWithoutCapturing) {
    auto session = make_session();
    auto &board = session.board;

    board.place_stone(Point{1, 0}, Stone::Black);
    board.place_stone(Point{-1, 0}, Stone::Black);
    board.place_stone(Point{0, 1}, Stone::Black);
    board.place_stone(Point{0, -1}, Stone::Black);

    const Move suicide_move{.point = Point{0, 0}, .stone = Stone::White};

    EXPECT_FALSE(MoveValidator::is_legal(session, suicide_move));
}

TEST(MoveValidatorTest, DoesNotMutateBoardWhenEvaluating) {
    auto session = make_session();
    auto &board = session.board;

    board.place_stone(Point{1, 0}, Stone::Black);
    board.place_stone(Point{-1, 0}, Stone::Black);
    board.place_stone(Point{0, 1}, Stone::Black);
    board.place_stone(Point{0, -1}, Stone::Black);

    const Move illegal_move{.point = Point{0, 0}, .stone = Stone::White};

    EXPECT_FALSE(MoveValidator::is_legal(session, illegal_move));

    EXPECT_TRUE(board.is_empty(Point{0, 0}));
    EXPECT_EQ(board.stone_at(Point{1, 0}), Stone::Black);
    EXPECT_EQ(board.stone_at(Point{-1, 0}), Stone::Black);
    EXPECT_EQ(board.stone_at(Point{0, 1}), Stone::Black);
    EXPECT_EQ(board.stone_at(Point{0, -1}), Stone::Black);
}
