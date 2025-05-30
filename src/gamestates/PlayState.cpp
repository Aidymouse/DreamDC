#include <gamestates/PlayState.h>

static LevelData test_level;

void PlayState::update(float dt) {}

void PlayState::draw(SDL_Renderer *renderer) {
  main_raycaster.render_level(&test_level);
}
