#ifndef GRAPHICS_MESHES_H
#define GRAPHICS_MESHES_H

class GameSession;

namespace graphics::mesh {

void draw_sphere(int color);
void draw_unit_cube(GameSession &session, int color);
void apply_transformations(float indx, float indy, float z = 0.0F);
void apply_transformation_general(float indx, float indy, float z);

} // namespace graphics::mesh

#endif // GRAPHICS_MESHES_H
