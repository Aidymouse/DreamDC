#pragma once

#include <util/Vec2.hpp>

enum MOVEMENT_STATES {
  TURNING,
  TRANSLATING,
  STILL
}

class Player {
  MOVEMENT_STATES movement_state;

public:
  Vec2 position;
  Vec2 dir;

  void handle_control(SDL_EVENT_KEY_DOWN key);
};
