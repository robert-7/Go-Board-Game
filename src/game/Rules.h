#ifndef GAME_RULES_H
#define GAME_RULES_H

class GameSession;

namespace rules {

void init_board(GameSession &session);
auto make_move(GameSession &session, int x, int y, int piece) -> int;

} // namespace rules

#endif // GAME_RULES_H
