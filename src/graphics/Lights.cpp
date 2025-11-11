#include "graphics/Lights.h"

#include "game/GameSession.h"
#include "game/GameState.h"

#include <GL/gl.h>
#include <array>

namespace graphics::lights {

void apply(const GameSession &session) {
    const std::array<GLfloat, 4> light_position{0.0F, 0.0F, -0.5F, 1.0F};
    glLightfv(GL_LIGHT0, GL_POSITION, light_position.data());

    const std::array<GLfloat, 4> light_strength{2.0F, 2.0F, 2.0F, 2.0F};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_strength.data());
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_strength.data());

    GLfloat spread = 90.0F - 90.0F * session.state.animation_time();
    glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, &spread);

    glEnable(GL_LIGHT0);
}

void apply_material() {
    const std::array<GLfloat, 4> material_diffuse{0.5F, 0.5F, 1.0F, 1.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse.data());

    const std::array<GLfloat, 4> material_specular{1.0F, 1.0F, 1.0F, 1.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular.data());

    const std::array<GLfloat, 1> material_shininess{100.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess.data());
}

} // namespace graphics::lights
