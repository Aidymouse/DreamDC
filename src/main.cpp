#include <SDL3/SDL.h>
#include <iostream>

#include <engine/GameState.h>
#include <gamestates/PlayState.h>
#include <memory>

std::shared_ptr<GameState> cur_state = std::make_shared<PlayState>();

int main() {
  /** Set up window */
  SDL_Window *window =
      SDL_CreateWindow("It is like being in a dream...", 800, 600, 0);
  SDL_Renderer *main_renderer = SDL_CreateRenderer(window, NULL);

  bool window_is_open = true;

  while (window_is_open) {

    /** Event based */
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        window_is_open = false;
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        // cur_state->handle_click(&event);
      }
    }

    /* Update */
    cur_state->update(0.01);

    /* Draw */
    SDL_SetRenderDrawColor(main_renderer, 255, 255, 255, 255);
    SDL_RenderClear(main_renderer);

    // cur_state->draw(main_renderer);

    // Texture ?
    SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);

    cur_state->draw(main_renderer);

    SDL_RenderPresent(main_renderer);
  }

  return 0;
}
