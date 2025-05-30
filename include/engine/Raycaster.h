#pragma once

#include <SDL3/SDL.h>
#include <engine/LevelData.h>
#include <util/Vec2.hpp>

/** Distance from player position to camera plane
 * Set so that there are 10 units between camera and an adjacent wall, if you're
 * in the center of a space
 * */

#define PLANE_DIST 40

class RaycastCamera {
public:
  Vec2 pos;
  Vec2 dir;
  Vec2 plane;

  RaycastCamera(float FOV);
  void rotate(float degrees);
};

class Raycaster {

public:
  void render_level(LevelData *level_data, RaycastCamera camera,
                    SDL_Renderer *renderer);
};
