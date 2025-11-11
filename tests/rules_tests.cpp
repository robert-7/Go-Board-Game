#include "game/Board.h"
#include "game/GameSession.h"
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

TEST(RulesTest, PlacesStoneOnEmptyPoint) {
    auto session = make_session();
    const Point move{0, 0};

    const auto result = rules::make_move(session, move, Stone::Black);

    EXPECT_EQ(result, 1);
    EXPECT_EQ(session.board.stone_at(move), Stone::Black);
}

TEST(RulesTest, RejectsMoveOutOfBounds) {
    auto session = make_session();
    const Point move{Board::CENTER + 1, 0};

    const auto result = rules::make_move(session, move, Stone::White);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(session.board.is_empty(Point{0, 0}));
}

TEST(RulesTest, RejectsMoveOnOccupiedPoint) {
    auto session = make_session();
    const Point target{1, 1};

    ASSERT_EQ(rules::make_move(session, target, Stone::Black), 1);

    const auto second_try = rules::make_move(session, target, Stone::White);

    EXPECT_EQ(second_try, 0);
    EXPECT_EQ(session.board.stone_at(target), Stone::Black);
}

TEST(RulesTest, CapturesSurroundedStone) {
    auto session = make_session();
    const Point white_stone{0, 0};

    ASSERT_EQ(rules::make_move(session, white_stone, Stone::White), 1);
    ASSERT_EQ(rules::make_move(session, Point{1, 0}, Stone::Black), 1);
    ASSERT_EQ(rules::make_move(session, Point{-1, 0}, Stone::Black), 1);
    ASSERT_EQ(rules::make_move(session, Point{0, 1}, Stone::Black), 1);

    session.board.clear_captured_groups();

    const auto capture_result = rules::make_move(session, Point{0, -1}, Stone::Black);

    EXPECT_EQ(capture_result, 1);
    EXPECT_EQ(session.board.stone_at(white_stone), Stone::Empty);

    const auto &captured = session.board.captured_groups();
    ASSERT_EQ(captured.size(), 1U);
    const auto &group = captured.front();
    ASSERT_EQ(group.size(), 1U);
    EXPECT_EQ(group.front().location.x, white_stone.x);
    EXPECT_EQ(group.front().location.y, white_stone.y);
    EXPECT_EQ(group.front().color, Stone::White);
}

TEST(RulesTest, PreventsSelfCaptureWithoutCapture) {
    auto session = make_session();

    ASSERT_EQ(rules::make_move(session, Point{1, 0}, Stone::Black), 1);
    ASSERT_EQ(rules::make_move(session, Point{-1, 0}, Stone::Black), 1);
    ASSERT_EQ(rules::make_move(session, Point{0, 1}, Stone::Black), 1);
    ASSERT_EQ(rules::make_move(session, Point{0, -1}, Stone::Black), 1);

    session.board.clear_captured_groups();

    const auto result = rules::make_move(session, Point{0, 0}, Stone::White);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(session.board.is_empty(Point{0, 0}));
    EXPECT_TRUE(session.board.captured_groups().empty());
}
