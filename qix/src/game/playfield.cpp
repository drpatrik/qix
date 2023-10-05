#include "game/assets.h"
#include "game/playfield.h"
#include "utility/timer.h"

#include <iostream>
#include <memory>

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

void RenderObjects(std::deque<std::shared_ptr<Object>>& objects, double delta_time) {
  for (auto it = objects.begin(); it != objects.end();) {
    (*it)->Render(delta_time);
    /*if (auto [status, event] = (*it)->IsReady(); status) {
      events.Push(event);
      it = animations.erase(it);
    } else {
      ++it;
      }*/
    ++it;
  }
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
  surface_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, kWidth, kHeight);
  ClearTexture(renderer_, surface_);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  game_controller_ = std::make_shared<utility::GameController>(kAssetFolder);
  SDL_RaiseWindow(window_);
  AddObject<Line>(renderer_, surface_, 200, 200, 150, Color::Red);
}

Playfield::~Playfield() noexcept {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
}

void Playfield::NewGame() {
  x_ = y_ = 0;
  ClearTexture(renderer_, surface_);
}

void Playfield::GameControl(Controls control_pressed) {
  switch (control_pressed) {
    case Controls::Up:
      if (y_ <= 0) {
        break;
      }
      y_--;
      DrawPixel(renderer_, surface_, x_, y_);
      break;
    case Controls::Down:
      if (y_ >= kHeight) {
        break;
      }
      y_++;
      DrawPixel(renderer_, surface_, x_, y_);
      break;
    case Controls::Left:
      if (x_ <= 0) {
        break;
      }
      x_--;
      DrawPixel(renderer_, surface_, x_, y_);
      break;
    case Controls::Right:
      if (x_ >= kWidth) {
        break;
      }
      x_++;
      DrawPixel(renderer_, surface_, x_, y_);
      break;
    case Controls::Fast:
      break;
    case Controls::Slow:
      break;
    default:
      break;
  }
}

void Playfield::Render(double delta) {
  SDL_RenderClear(renderer_);
  //SDL_RenderCopy(renderer_, surface_, nullptr, nullptr);
  RenderObjects(objects_, delta);
  SDL_RenderPresent(renderer_);
}

void Playfield::Update(double delta_time) { Render(delta_time); }
