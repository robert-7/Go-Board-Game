#include "GameSession.h"

auto current_session() -> GameSession & {
    static GameSession session;
    return session;
}
