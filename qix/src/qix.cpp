#include <SDL2/SDL.h>
#include <iostream>
#include <cstdio>

const int WIN_WIDTH = 640;
const int WIN_HEIGHT = 480;

int main(int, char *[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }
  SDL_Window *win = SDL_CreateWindow("Rendering to a texture!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     WIN_WIDTH, WIN_HEIGHT, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

  // Make a target texture to render too
  SDL_Texture *target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIN_WIDTH, WIN_HEIGHT);

  // Now render to the texture
  SDL_SetRenderTarget(renderer, target);
  SDL_RenderClear(renderer);

  SDL_Rect rc { 50, 50, 50, 50 };

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
  SDL_RenderDrawRect(renderer, &rc);

  // Detach the texture
  SDL_SetRenderTarget(renderer, NULL);

  // Now render the texture target to our screen, but upside down
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, target, nullptr, nullptr);
  SDL_RenderPresent(renderer);

  std::getchar();

  // Now render to the texture
  SDL_SetRenderTarget(renderer, target);

  SDL_Rect rc2 { 55, 55, 50, 50 };

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
  SDL_RenderDrawRect(renderer, &rc2);

  // Detach the texture
  SDL_SetRenderTarget(renderer, NULL);

  // Now render the texture target to our screen, but upside down
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, target, nullptr, nullptr);
  SDL_RenderPresent(renderer);

  std::getchar();

  SDL_DestroyTexture(target);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
