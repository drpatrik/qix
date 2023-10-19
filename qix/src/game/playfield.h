#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <deque>

#include "game/objects.h"
#include "utility/game_controller.h"

class Playfield final {
 public:
  enum class Controls { None, Left, Right, Up, Down, Start, Pause, Quit, Slow, Fast };

  Playfield();

  Playfield(const Playfield&) = delete;

  Playfield(const Playfield&&) = delete;

  ~Playfield() noexcept;

  void NewGame();

  void Pause() {}

  void GameControl(Controls control_pressed);

  void HandleGameControllerEvent(SDL_Event& event) { game_controller_->HandleEvents(event); }

  void Update(double delta_timer);

 protected:
  template<class T, class ...Args>
  void AddObject(Args&&... args) { objects_.emplace_back(std::make_shared<T>(std::forward<Args>(args)...)); }
  void Render(double delta_timer);

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  SDL_Texture* surface_ = nullptr;

  int x_ = 0;
  int y_ = 0;
  int direction_ = 0;
  std::deque<std::shared_ptr<Object>> objects_;
  std::shared_ptr<utility::GameController> game_controller_;
};
