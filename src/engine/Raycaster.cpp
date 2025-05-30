#include <SDL3/SDL.h>
#include <cmath>
#include <engine/LevelData.h>
#include <engine/Raycaster.h>
#include <iostream>

#define screenscale 100.f // size at which debug stuff is drawn to the screen

void Raycaster::render_level(LevelData *level_data, RaycastCamera camera,
                             SDL_Renderer *renderer) {

  // Debug - draw level
  for (int row = 0; row < 5; row++) {
    for (int col = 0; col < 5; col++) {
      if (level_data->tile_data[row][col] != 0) {
        SDL_FRect r = {row * screenscale, col * screenscale, screenscale,
                       screenscale};
        SDL_RenderRect(renderer, &r);
      }
    }
  }
  //
  // Vec2 plane(PLANE_WIDTH, 0);

  for (int screenX = 0; screenX < 800; screenX++) {
    float cameraX = 2 * (screenX / 800.f) - 1;
    // Got to rememebr plane dist! Because dir is normalized
    Vec2 rayDir = (camera.dir * PLANE_DIST) + (camera.plane * cameraX);
    std::cout << camera.plane.x << std::endl;
    // rayDir.normalize();

    Vec2 screenPos = (camera.pos / 100.f) * screenscale;
    SDL_RenderLine(renderer, screenPos.x, screenPos.y, screenPos.x + rayDir.x,
                   screenPos.y + rayDir.y);

    Vec2 deltaDist = Vec2(abs(1 / rayDir.x), abs(1 / rayDir.y));
  }
}
