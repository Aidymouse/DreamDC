#pragma once

#include <SDL3/SDL.h>
#include <engine/GameState.h>
#include <engine/Raycaster.h>

class PlayState : public GameState {
  Raycaster main_raycaster;

public:
  virtual void update(float dt) override;
  virtual void draw(SDL_Renderer *renderer) override;
};
