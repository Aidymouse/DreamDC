#include <SDL3/SDL.h>
#include <cmath>
#include <engine/LevelData.h>
#include <engine/Raycaster.h>
#include <iostream>

#define screenscale 100.f // size at which debug stuff is drawn to the screen

const bool DEBUG = false;

void Raycaster::render_level(LevelData *level_data, RaycastCamera camera,
                             SDL_Renderer *renderer) {

  //
  // Vec2 plane(PLANE_WIDTH, 0);

  for (int screenX = 0; screenX < 800; screenX++) {
    float cameraX = 2 * (screenX / 800.f) - 1;
    // Got to rememebr plane dist! Because dir is normalized
    Vec2 rayDir = (camera.dir * PLANE_DIST) + (camera.plane * cameraX);
    // rayDir.normalize();

    Vec2 gridCell((int)camera.pos.x, (int)camera.pos.y); // x = col

    // Length of ray from cell boundary to cell boundary
    float gridDeltaDistX = rayDir.x == 0 ? 1e30 : std::abs(1.f / rayDir.x);
    float gridDeltaDistY = rayDir.y == 0 ? 1e30 : std::abs(1.f / rayDir.y);

    // Length from cur pos to next grid cell
    float distToGridX;
    float distToGridY;

    // Which dir we goin?
    int stepX;
    int stepY;

    // This math goes way over my head... damn
    if (rayDir.x < 0) {
      stepX = -1;
      distToGridX = (camera.pos.x - gridCell.x) * gridDeltaDistX;
    } else {
      stepX = 1;
      distToGridX = (gridCell.x + 1 - camera.pos.x) * gridDeltaDistX;
    }
    if (rayDir.y < 0) {
      stepY = -1;
      distToGridY = (camera.pos.y - gridCell.y) * gridDeltaDistY;
    } else {
      stepY = 1;
      distToGridY = (gridCell.y + 1 - camera.pos.y) * gridDeltaDistY;
    }

    int hit = 0;
    while (hit == 0) {
      if (distToGridX < distToGridY) {
        gridCell.x += stepX;
      } else {
        gridCell.y += stepY;
      }

      if (gridCell.x < 0 || gridCell.y < 0)
        hit = -1;
      if (gridCell.x >= level_data->width || gridCell.y > level_data->height)
        hit = -1;
      if (level_data->tile_data[(int)gridCell.y][(int)gridCell.x] != 0)
        hit = 1;

      if (hit == 0) {
        if (distToGridX < distToGridY) {
          distToGridX += gridDeltaDistX;
        } else {
          distToGridY += gridDeltaDistY;
        }
      }
    }

    // Get straight distance from line along camera plane to impact point

    if (hit == -1)
      continue;

    float perpWallDist = distToGridX < distToGridY ? distToGridX : distToGridY;

    int line_height = 600 / perpWallDist;
    SDL_RenderLine(renderer, screenX, 300 - line_height / 2, screenX,
                   300 + line_height / 2);

    if (DEBUG) {
      for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++) {
          if (level_data->tile_data[row][col] != 0) {
            SDL_FRect r = {row * screenscale, col * screenscale, screenscale,
                           screenscale};
            SDL_RenderRect(renderer, &r);
          }
        }
      }
      //  if (distToGridX < distToGridY) perpWallDist = (distToGridX -
      //  gridDeltaDistX)

      // Vec2 screenPos = camera.pos * screenscale;
      // SDL_RenderLine(renderer, screenPos.x, screenPos.y,
      //                screenPos.x + rayDir.x * std::min(distToGridY,
      //                distToGridX) *
      //                                  screenscale,
      //                screenPos.y + rayDir.y * std::min(distToGridY,
      //                distToGridX) *
      //                                  screenscale);
    }
  }
}
