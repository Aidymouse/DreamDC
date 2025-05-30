#pragma once

#include <SDL3/SDL.h>
#include <engine/GameState.h>
#include <engine/Raycaster.h>

class PlayState : public GameState {
  Raycaster main_raycaster;
  LevelData test_level;

public:
  PlayState();
  virtual void update(float dt) override;
  virtual void draw(SDL_Renderer *renderer) override;
};
