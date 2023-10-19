#pragma once

#include "utility/text.h"

namespace utility {

class Texture {
 public:
  Texture() {}

  Texture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, Color text_color) {
    std::tie(texture_, rc_.w, rc_.h) = CreateTextureFromText(renderer, font, text, text_color);
  }

  Texture(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, Color text_color, Color background_color) {
    std::tie(texture_, rc_.w, rc_.h) = CreateTextureFromFramedText(renderer, font, text, text_color, background_color);
  }

  Texture(SDL_Renderer* renderer, int x, int y, int width, int height, Color color) {
    auto target_texture = UniqueTexturePtr{ SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height) };

    SDL_SetRenderTarget(renderer, target_texture.get());
    SDL_RenderClear(renderer);

    std::apply([](auto &&... args) { SetColor(args...); }, UnpackColor(renderer, color));

    rc_ = { 0, 0, width, height };

    SDL_RenderFillRect(renderer, &rc_);
    SDL_SetRenderTarget(renderer, nullptr);

    texture_ = std::move(target_texture);

    rc_.x = x;
    rc_.y = y;
  }

  explicit Texture(const std::tuple<std::shared_ptr<SDL_Texture>, int, int>& texture) :
      texture_(UniqueTexturePtr{ std::get<0>(texture).get() })  {
    rc_.w = std::get<1>(texture);
    rc_.h = std::get<2>(texture);
  }

  inline void reset() { texture_.reset(); }

  inline bool is_null() const { return nullptr == texture_; }

  inline operator SDL_Texture*() { return texture_.get(); }

  inline operator const SDL_Rect&() { return rc_; }

  inline operator const SDL_Rect*() { return &rc_; }

  inline void SetX(int x) { rc_.x = x; }

  inline void SetY(int y) { rc_.y = y; }

  inline void SetXY(int x, int y) { rc_.x = x;  rc_.y = y; }

  inline int x() const { return rc_.x; }

  inline int y() const { return rc_.y; }

  inline int center_x(int w) const { return utility::Center(w, rc_.w); }

  inline int center_y(int h) const { return utility::Center(h, rc_.h); }

  inline int width() const { return rc_.w; }

  inline int height() const { return rc_.h; }

 private:
  UniqueTexturePtr texture_ = nullptr;
  SDL_Rect rc_ = {};
};

} // namespace texture
