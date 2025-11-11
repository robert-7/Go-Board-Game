#ifndef GAME_RULES_H
#define GAME_RULES_H

class GameSession;
struct Point;
enum class Stone : unsigned char;

namespace rules {

void init_board(GameSession &session);
auto make_move(GameSession &session, Point point, Stone stone) -> int;

} // namespace rules

#endif // GAME_RULES_H
