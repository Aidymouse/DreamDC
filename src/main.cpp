#include <SDL3/SDL.h>
#include <iostream>

#include <engine/GameState.h>
#include <engine/TextureStore.hpp>
#include <gamestates/PlayState.h>
#include <memory>

std::shared_ptr<GameState> cur_state = std::make_shared<PlayState>();

TextureStore &texture_store = TextureStore::getInstance();

int main() {
  /** Set up window */
  SDL_Window *window =
      SDL_CreateWindow("It is like being in a dream...", 800, 600, 0);
  SDL_Renderer *main_renderer = SDL_CreateRenderer(window, NULL);

  texture_store.load_texture("../graphics/bluestone.bmp", "bluestone",
                             main_renderer);
  texture_store.load_texture("../graphics/test.bmp", "test", main_renderer);

  float now = SDL_GetPerformanceCounter();
  float last = 0;
  float dt = 0;

  bool window_is_open = true;
  float fps_timer = 0;
  float frames = 0;

  while (window_is_open) {

    /** Event based */
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        window_is_open = false;
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        // cur_state->handle_click(&event);
      } else if (event.type == SDL_EVENT_KEY_DOWN) {
        cur_state->key_down(&event.key);
      }
    }

    /* Update DT */
    last = now;
    now = SDL_GetPerformanceCounter();
    dt = ((now - last)) / SDL_GetPerformanceFrequency();
    frames++;
    fps_timer += dt;
    if (fps_timer > 1) {
      // std::cout << "FPS: " << frames << std::endl;
      fps_timer -= 1;
      frames = 0;
    }

    /* Update */
    cur_state->update(dt);

    /* Draw */
    SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
    SDL_RenderClear(main_renderer);

    // cur_state->draw(main_renderer);

    // Texture ?
    SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);

    cur_state->draw(main_renderer);

    SDL_RenderPresent(main_renderer);
  }

  texture_store.destroy_textures();

  return 0;
}
