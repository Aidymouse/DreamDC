#pragma once

#include <SDL3/SDL.h>
#include <engine/GameState.h>
#include <engine/Player.h>
#include <engine/Raycaster.h>

class PlayState : public GameState {
  Raycaster main_raycaster;
  LevelData test_level;
  Player main_player;

public:
  PlayState();
  virtual void key_down(SDL_KeyboardEvent *event) override;
  virtual void update(float dt) override;
  virtual void draw(SDL_Renderer *renderer) override;
};
