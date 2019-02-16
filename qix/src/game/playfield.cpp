#include "game/playfield.h"
#include "utility/timer.h"

#include <iostream>

namespace {

const int kWidth = 640;
const int kHeight = 480;

void ClearTexture(SDL_Renderer* renderer, SDL_Texture* texture) {
  SDL_SetRenderTarget(renderer, texture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderClear(renderer);
  SDL_SetRenderTarget(renderer, nullptr);
}

void DrawPixel(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);
  SDL_SetRenderTarget(renderer, texture);
  SDL_RenderDrawPoint(renderer, x, y);
  SDL_SetRenderTarget(renderer, nullptr);
}

}

using namespace utility;

Playfield::Playfield() {
  window_ = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, kWidth, kHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  if (nullptr == window_) {
    std::cout << "Failed to create window : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == renderer_) {
    std::cout << "Failed to create renderer : " << SDL_GetError() << std::endl;
    exit(-1);
  }
  playfield_texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, kWidth, kHeight);
  ClearTexture(renderer_, playfield_texture_);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_RaiseWindow(window_);
}

Playfield::~Playfield() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Playfield::NewGame() {
  x_ = y_ = 0;
  ClearTexture(renderer_, playfield_texture_);
}

void Playfield::GameControl(Controls control_pressed) {
  switch (control_pressed) {
    case Controls::Up:
      if (y_ <= 0) {
        break;
      }
      y_--;
      DrawPixel(renderer_, playfield_texture_, x_, y_);
      break;
    case Controls::Down:
      if (y_ >= kHeight) {
        break;
      }
      y_++;
      DrawPixel(renderer_, playfield_texture_, x_, y_);
      break;
    case Controls::Left:
      if (x_ <= 0) {
        break;
      }
      x_--;
      DrawPixel(renderer_, playfield_texture_, x_, y_);
      break;
    case Controls::Right:
      if (x_ >= kWidth) {
        break;
      }
      x_++;
      DrawPixel(renderer_, playfield_texture_, x_, y_);
      break;
    case Controls::Fast:
      break;
    case Controls::Slow:
      break;
    default:
      break;
  }
}

void Playfield::Render(double) {
  SDL_RenderClear(renderer_);
  SDL_RenderCopy(renderer_, playfield_texture_, nullptr, nullptr);
  SDL_RenderPresent(renderer_);
}

void Playfield::Update(double delta_time) { Render(delta_time); }
