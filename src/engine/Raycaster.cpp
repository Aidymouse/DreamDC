#include <SDL3/SDL.h>
#include <cmath>
#include <engine/LevelData.h>
#include <engine/Raycaster.h>
#include <engine/TextureStore.hpp>
#include <iostream>

#define screenscale 100.f // size at which debug stuff is drawn to the screen

TextureStore &texture_store_r = TextureStore::getInstance();

const bool DEBUG = false;
const int debug_x = 200;

void Raycaster::render_level(LevelData *level_data, RaycastCamera camera,
                             SDL_Renderer *renderer) {

  //
  // Vec2 plane(PLANE_WIDTH, 0);
  SDL_Texture *blue = texture_store_r.get("bluestone");

  /** Floor Casting */
  // 600 is the screen height
  // Kinda doesnt work.. bleh...
  for (int screenY = 600; screenY >= 0; screenY--) {
    Vec2 rayDirOne = (camera.dir * PLANE_DIST) - camera.plane;
    Vec2 rayDirTwo = (camera.dir * PLANE_DIST) + camera.plane;

    float yRelative = screenY - 600 / 2;
    float zPos = 0.5 * 600; // change for heights
                            //
    float rowDist = zPos / yRelative;

    Vec2 floorStep(rowDist * (rayDirTwo.x - rayDirOne.x) / 800,
                   rowDist * (rayDirTwo.y - rayDirOne.y) / 800);

    Vec2 floor = camera.pos + (rayDirOne * rowDist);
    for (int screenX = 0; screenX < 800; screenX++) {
      Vec2 mapCell((int)floor.x, (int)floor.y);
      Vec2 texCoords((int)(100 * (floor.x - mapCell.x)) & (100 - 1),
                     (int)(100 * (floor.y - mapCell.y)) & (100 - 1));
      floor += floorStep;

      SDL_FRect texel = {texCoords.x, texCoords.y, 1, 1};
      SDL_FRect draw = {screenX, screenY, 1, 1};
      SDL_RenderTexture(renderer, blue, &texel, &draw);
    }
  }

  // WALLS
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

    int side = distToGridX < distToGridY
                   ? 1
                   : 0; // 1 = X hit (hit column), 0 = y hit (hit row)
    float perpWallDist = side == 1 ? distToGridX : distToGridY;

    Vec2 wallPoint =
        camera.pos + (rayDir * (side == 1 ? distToGridX : distToGridY));

    // TODO: make it tex width
    int texX = (side == 1 ? wallPoint.y - floor(wallPoint.y)
                          : wallPoint.x - floor(wallPoint.x)) *
               100;

    if (screenX == debug_x) {
      std::cout << "side " << side << std::endl;
      std::cout << "tex x " << texX << std::endl;
      std::cout << wallPoint.x << ", " << wallPoint.y << std::endl;
    }

    // std::cout << wallPoint.x << ", "
    // << wallPoint.y << std::endl;

    int line_height = 600 / perpWallDist;
    SDL_FRect src = {texX, 0, texX, 100};
    SDL_FRect strip = {screenX, 300 - (line_height / 2), 1, line_height};
    SDL_RenderTexture(renderer, blue, &src, &strip);

    // SDL_RenderTexture();
    // SDL_RenderLine(renderer, screenX, 300 - line_height / 2, screenX,
    //                300 + line_height / 2);

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

      if (screenX == debug_x) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        Vec2 screenPos = camera.pos * screenscale;
        SDL_RenderLine(
            renderer, screenPos.x, screenPos.y,
            screenPos.x +
                rayDir.x * std::min(distToGridY, distToGridX) * screenscale,
            screenPos.y +
                rayDir.y * std::min(distToGridY, distToGridX) * screenscale);
      }

      if (screenX == debug_x) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      }
    }
  }
}
