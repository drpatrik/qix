#pragma once

#include "utility/color.h"
#include "utility/texture.h"
#include "constants.h"

#include <iostream>

class Object {
public:
  using Color = utility::Color;
  using Texture = utility::Texture;

  Object(SDL_Renderer *renderer, SDL_Texture* surface) : renderer_(renderer), surface_(surface) {}

  explicit Object(SDL_Renderer *renderer) : renderer_(renderer), surface_(nullptr) {}

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
  Line(SDL_Renderer *renderer, int start_x, int start_y, int direction, int length, double velocity, Color color) :
      Object(renderer), radius_(length / 2.0), velocity_(velocity), color_(color) {
    x_ = start_x;
    y_ = start_y;
    SetDirection(direction);
  }

  void SetDirection(int angle) {
    if (-1 == new_direction_) {
      angle_ = new_angle_ = kPiDiv180 * (angle + 90);
      direction_ = new_direction_ = kPiDiv180 * angle;
      new_direction_reached_ = true;
    } else {
      new_angle_ = kPiDiv180 * (angle + 90);
      new_direction_ = kPiDiv180 * angle;
      new_direction_reached_ = false;
    }
  }

  void SetVelocity(double velocity) { velocity_ = velocity; }

  void SetColor(Color color) { color_ = color; }

  virtual void Render(double delta) override {
    std::apply([this](auto &&... args) { Object::SetColor(args...); }, UnpackColor(color_));

    const int x1 = x_ + cos(angle_) * radius_;
    const int y1 = y_ + -sin(angle_) * radius_;
    const int x2 = x_ + cos(angle_ + kPi) * radius_;
    const int y2 = y_ + -sin(angle_ + kPi) * radius_;

    SDL_RenderDrawLine(*this, x1, y1, x2, y2);

    std::apply([this](auto &&... args) { Object::SetColor(args...); }, UnpackColor(Color::Black));

    if (!new_direction_reached_) {
      if (direction_ < new_direction_) {
        angle_ += delta * (velocity_ / 2.0);
        direction_ += delta * (velocity_ / 2.0);
        new_direction_reached_ = (direction_ >= new_direction_);
      } else if (direction_ > new_direction_) {
        angle_ -= delta * (velocity_ / 2.0);
        direction_ -= delta * (velocity_  / 2.0);
        new_direction_reached_ = (direction_ <= new_direction_);
      }
      if (new_direction_reached_) {
        angle_ = new_angle_;
        direction_ = new_direction_;
      }
    }
    x_ += cos(direction_) * delta * velocity_;
    y_ += -sin(direction_) * delta * velocity_;
  }

 protected:
  const double kPi = 3.14159265358;
  const double kPiDiv180 = kPi / 180.0;

 private:
  double radius_;
  double angle_ = 0.0;
  double direction_ = 0.0;
  double new_direction_ = -1;
  double new_angle_ = -1;
  bool new_direction_reached_ = false;
  double velocity_= 0.0;
  Color color_;
};
