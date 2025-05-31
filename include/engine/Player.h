#pragma once

#include <SDL3/SDL.h>
#include <engine/Raycaster.h>
#include <util/Vec2.hpp>

enum class MOVEMENT_STATES { TRANSLATING, STILL };
enum class TURNING_STATES { TURNING, STILL };

class Player {
  MOVEMENT_STATES movement_state = MOVEMENT_STATES::STILL;
  TURNING_STATES turning_state = TURNING_STATES::STILL;

  Vec2 pos;
  Vec2 dir;
  float angle = 0; // angle should really be found from dir but w/e

  /* Moving & Turning */
  float move_speed = 20;
  Vec2 destination;

  /* Turning */
  float turn_speed = 600;
  float turn_dir = 1; // 1 = clockwise
  float desired_angle = 0;

public:
  Player();
  void key_down(SDL_KeyboardEvent *event);
  void update(float dt);
  RaycastCamera get_raycast_camera();
};
