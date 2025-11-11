#ifndef GRAPHICS_LIGHTS_H
#define GRAPHICS_LIGHTS_H

class GameSession;

namespace graphics::lights {

void apply(const GameSession &session);
void apply_material();

} // namespace graphics::lights

#endif // GRAPHICS_LIGHTS_H
