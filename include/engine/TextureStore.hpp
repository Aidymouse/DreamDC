#pragma once

#include <iostream>
#include <string>

#include <SDL3/SDL.h>
#include <unordered_map>

/** Singleton that contains texture data */
class TextureStore {
private:
  TextureStore() {};
  // TextureStore(TextureStore const &);
  // void operator=(TextureStore const &);

public:
  std::unordered_map<std::string, SDL_Texture *> textures;
  static TextureStore &getInstance() {
    static TextureStore instance;
    return instance;
  };

  TextureStore(TextureStore const &) = delete;
  void operator=(TextureStore const &) = delete;

  void load_texture(std::string filename, std::string texture_name,
                    SDL_Renderer *r) {
    std::cout << "...loading '" << filename << "' under '" << texture_name
              << "'" << std::endl;

    SDL_Surface *s = SDL_LoadBMP(filename.c_str());
    if (s == NULL) {
      std::cout << "......Error loading surface: " << SDL_GetError()
                << std::endl;
      return;
    }

    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
    textures[texture_name] = t;

    SDL_DestroySurface(s);
  }

  SDL_Texture *get(std::string texture_name) { return textures[texture_name]; }

  void destroy_textures() {
    std::cout << "Destroying Textures..." << std::endl;
    for (auto &v : textures) {
      std::cout << "..." << v.first << std::endl;
      SDL_DestroyTexture(v.second);
    }
  };
};
