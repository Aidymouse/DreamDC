#pragma once

#include <SDL3/SDL.h>

class GameState {
public:
  virtual void key_pressed(SDL_KeyboardEvent *event) {};
  virtual void update(float dt) = 0;
  virtual void draw(SDL_Renderer *renderer) = 0;
};
