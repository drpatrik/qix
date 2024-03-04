#pragma once

#include "constants.h"
#include "utility/color.h"
#include "utility/texture.h"
#include "utility/function_caller.h"

#include <iostream>

class Object {
public:
  using Color = utility::Color;
  using Texture = utility::Texture;

  Object(SDL_Renderer *renderer, SDL_Texture* surface) : renderer_(renderer), surface_(surface) {}

  Object(SDL_Renderer *renderer, int x, int y) : x_(x), y_(y), renderer_(renderer), surface_(nullptr) {}

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

private:
  SDL_Renderer* renderer_;
  SDL_Texture* surface_;
};

class LineDraw final : public Object {
 public:
  LineDraw(SDL_Renderer *renderer, int start_x, int start_y, int direction, int length, double velocity, Color color) :
      Object(renderer, start_x, start_y), radius_(length / 2.0), velocity_(velocity), color_(color) {
    SetDirection(direction);
  }

  void SetDirection(int angle) {
    const double kPiDiv180 = M_PI / 180.0;

    angle_ = kPiDiv180 * (angle + 90);
    direction_ = kPiDiv180 * angle;
  }

  void SetVelocity(double velocity) { velocity_ = velocity; }

  void SetColor(Color color) { color_ = color; }

  void SetLength(int length) { radius_ = length / 2; }

  virtual void Render(double delta) override {
    std::apply([](auto &&... args) { utility::SetColor(args...); }, UnpackColor(*this, color_));

    SDL_RenderDrawLine(*this, x_ + cos(angle_) * radius_, y_ + -sin(angle_) * radius_,
                       x_ + cos(angle_ + M_PI) * radius_, y_ + -sin(angle_ + M_PI) * radius_);

    std::apply([](auto &&... args) { utility::SetColor(args...); }, UnpackColor(*this, Color::Black));

    x_ += cos(direction_) * delta * velocity_;
    y_ += -sin(direction_) * delta * velocity_;
  }

 private:
  double radius_;
  double angle_ = 0.0;
  double direction_ = 0.0;
  double velocity_= 0.0;
  Color color_;
};

class LineTexture final : public Object {
 public:
  LineTexture(SDL_Renderer *renderer, int start_x, int start_y, int direction, int length, double velocity, Color color) :
      Object(renderer, start_x, start_y), velocity_(velocity), color_(color) {
    SetDirection(direction);
    texture_ = std::make_unique<Texture>(*this, start_x, start_y, length, 1, color_);
  }

  void SetDirection(int angle) {
    const double kPiDiv180 = (M_PI / 180.0);

    angle_ = angle;
    direction_ = kPiDiv180 * angle;
  }

  void SetVelocity(double velocity) { velocity_ = velocity; }

  void SetColor(Color color) {
    color_ = color;
    texture_ = std::make_unique<Texture>(*this, texture_->x(), texture_->y(), texture_->width(), texture_->height(), color_);
  }

  void SetLength(int length) {
    texture_ = std::make_unique<Texture>(*this, texture_->x(), texture_->y(), length, texture_->height(), color_);
  }

  virtual void Render(double delta) override {
    x_ += cos(direction_) * delta * velocity_;
    y_ += -sin(direction_) * delta * velocity_;

    texture_->SetXY(x_, y_);
    //SDL_Point pt = { 200 / 2, 1 / 2 };
    SDL_RenderCopyEx(*this,*texture_, nullptr, *texture_, CounterClockWise(angle_ + 90), nullptr, SDL_FLIP_NONE);
  }

 protected:
  inline double CounterClockWise(double deg) const { return 360 - (360 + int(deg)) % 360; }

 private:
  int angle_ = 0.0;
  double direction_ = 0.0;
  double velocity_= 0.0;
  Color color_;
  std::unique_ptr<Texture> texture_ = nullptr;
};

class QixObject final : public Object {
 public:
  QixObject(SDL_Renderer *renderer, int start_x, int start_y) : Object(renderer, start_x, start_y)  {
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x, start_y, 65, 100, 150, Color::Red));
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x + 30, start_y - 10, 65, 100, 150, Color::Red));
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x + 60, start_y - 20, 65, 100, 150, Color::Red));
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x + 90, start_y - 30, 65, 100, 150, Color::Red));
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x + 120, start_y - 40, 65, 200, 150, Color::Green));
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x + 150, start_y - 50, 65, 100, 150, Color::Blue));
    lines_.emplace_back(std::make_shared<LineDraw>(renderer, start_x + 180, start_y - 60, 65, 100, 150, Color::Blue));
  }

  virtual void Render(double delta) override {
    for (auto& l : lines_) {
      l->Render(delta);
    }
  }

 private:
  std::vector<std::shared_ptr<LineDraw>> lines_;
};
