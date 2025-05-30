#include <gamestates/PlayState.h>
#include <util/Vec2.hpp>

PlayState::PlayState() {
  int d[5][5] = {
      {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1},
      {1, 0, 0, 0, 4}, {1, 1, 1, 1, 1},
  };
  test_level.set_tile_data(d, 5, 5);
}

void PlayState::update(float dt) {}

void PlayState::draw(SDL_Renderer *renderer) {

  RaycastCamera c(120);
  c.pos.x = 1.5;
  c.pos.y = 1.5;

  main_raycaster.render_level(&test_level, c, renderer);
}
