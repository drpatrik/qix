#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

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

  void Update(double delta_timer);

 protected:
  void Render(double delta_timer);

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  SDL_Texture* playfield_texture_;
  int x_ = 0;
  int y_ = 0;
};
