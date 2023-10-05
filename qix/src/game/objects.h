#pragma once

#include "utility/color.h"
#include "utility/texture.h"
#include "constants.h"

#include <iostream>

class Object {
public:
  using Color = utility::Color;
  using Texture = utility::Texture;

  explicit Object(SDL_Renderer *renderer, SDL_Texture* surface) : renderer_(renderer), surface_(surface) {}

  virtual ~Object() noexcept = default;

  //virtual std::pair<bool, Event::Type> IsReady() const = 0;

  //inline const Assets& GetAsset() const { return *assets_; }

  virtual void Render(double) = 0;

  inline operator SDL_Renderer*() const { return renderer_; }
  inline operator SDL_Texture*() const { return surface_; }

  virtual std::string name() const { return typeid(*this).name(); }

protected:
  double x_ = 0.0;
  double y_ = 0.0;

  inline void RenderCopy(SDL_Texture* texture, const SDL_Rect& rc) { SDL_RenderCopy(*this, texture, nullptr, &rc); }

  inline void RenderCopy(Texture& texture) { SDL_RenderCopy(*this, texture, nullptr, texture); }

  inline void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) { SDL_SetRenderDrawColor(*this, r, g, b, alpha); }

private:
  SDL_Renderer* renderer_;
  SDL_Texture* surface_;
};

class Line final : public Object {
 public:
  Line(SDL_Renderer *renderer, SDL_Texture* surface, int start_x, int start_y, int length, Color color) : Object(renderer, surface), radius_(length / 2), color_(color) {
    x_ = start_x;
    y_ = start_y;
  }

  virtual void Render(double delta) override {
    const double kPi = 3.14159265358;
    const double kTwoTimesPi = 2.0 * kPi;

    angle_ += (delta * 3);
    if (angle_ > kTwoTimesPi) {
      angle_ = 0.0;
    }
    std::apply([this](auto &&... args) { Line::SetColor(args...); }, UnpackColor(color_));

    const int x1 = x_ + cos(angle_) * radius_;
    const int y1 = y_ + sin(angle_) * radius_;
    const int x2 = x_ + cos(angle_ + kPi) * radius_;
    const int y2 = y_ + sin(angle_ + kPi) * radius_;

    SDL_RenderDrawLine(*this, x1, y1, x2, y2);

    std::apply([this](auto &&... args) { Line::SetColor(args...); }, UnpackColor(Color::Black));

    x_ += delta * 75.0;
  }

 private:
  double radius_;
  double angle_ = 0.0;
  Color color_;
};
