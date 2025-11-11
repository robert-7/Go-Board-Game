#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

class GameSession;

namespace graphics {

void initialize(GameSession &session);
void register_callbacks();

}

#endif // GRAPHICS_RENDERER_H
