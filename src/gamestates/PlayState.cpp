#include <SDL3/SDL.h>
#include <gamestates/PlayState.h>
#include <util/Vec2.hpp>

PlayState::PlayState() {
  int d[5][5] = {
      {1, 1, 1, 1, 1}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1},
      {1, 0, 0, 0, 4}, {1, 1, 1, 1, 1},
  };
  test_level.set_tile_data(d, 5, 5);
}

void PlayState::key_down(SDL_KeyboardEvent *event) {
  main_player.key_down(event);
}

void PlayState::update(float dt) { main_player.update(dt); }

void PlayState::draw(SDL_Renderer *renderer) {
  RaycastCamera c = main_player.get_raycast_camera();

  main_raycaster.render_level(&test_level, c, renderer);
}
