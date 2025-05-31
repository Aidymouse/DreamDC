#pragma once

#include <SDL3/SDL.h>
#include <engine/Raycaster.h>
#include <util/Vec2.hpp>

enum class MOVEMENT_STATES { TRANSLATING = 0, STILL = 1 };
enum class TURNING_STATES { TURNING = 0, STILL = 1 };

class Player {
  MOVEMENT_STATES movement_state = MOVEMENT_STATES::STILL;
  TURNING_STATES turning_state = TURNING_STATES::STILL;

  Vec2 pos;
  Vec2 dir;
  float angle = 0; // angle should really be found from dir but w/e

  /* Moving & Turning */
  float move_speed = 10;
  Vec2 destination;

  /* Turning */
  float turn_speed = 600;
  float turn_dir = 1; // 1 = clockwise, -1 = anticlockwise
  float desired_angle = 0;

public:
  Player();
  void key_down(SDL_KeyboardEvent *event);
  void update(float dt);
  RaycastCamera get_raycast_camera();
};
