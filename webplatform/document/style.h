#pragma once

#include <cstdint>

namespace ve {
namespace webplatform {

class Padding {
public:
  Padding() = default;
  Padding(float paddings);
  Padding(float paddig_left, float paddig_right, float paddig_top,
          float paddig_bottom);

  float paddig_left = 0.0f;
  float paddig_right = 0.0f;
  float paddig_top = 0.0f;
  float paddig_bottom = 0.0f;
};

class Margin {
public:
  Margin() = default;
  Margin(float margins);
  Margin(float margin_left, float margin_right, float margin_top,
         float margin_bottom);

  float margin_left = 0.0f;
  float margin_right = 0.0f;
  float margin_top = 0.0f;
  float margin_bottom = 0.0f;
};

class Colour {
public:
  enum class ColourName { RED, GREEN, BLUE, BLACK, WHITE };
  Colour(ColourName colour_name);
  Colour(uint8_t red, uint8_t green, uint8_t blue);
  Colour() = default;

  uint8_t red_ = 255;
  uint8_t green_ = 255;
  uint8_t blue_ = 255;
};

class Style {
public:
  enum class Display { BLOCK };
  enum class HeightMode { FIXED, AUTO };
  enum class WidthMode { FIXED, AUTO };
  enum class Overflow { VISIBLE, HIDDEN };

  Style(float width, float height, Colour colour);
  Style() = default;

  float Width() const;
  float Height() const;
  Colour GetColour() const;

  const Padding &GetPadding() const;
  const Margin &GetMargin() const;

  void SetPadding(Padding paddings);
  void SetMargin(Margin margins);

  HeightMode height_mode_ = HeightMode::FIXED;
  WidthMode width_mode_ = WidthMode::FIXED;
  Overflow overflow_ = Overflow::VISIBLE;
  float width_ = 0.0f;
  float height_ = 0.0f;

  float border_width = 0.0f;

  Colour colour_;
  Padding padding_;
  Margin margin_;

  // layout algo choose
  Display display_ = Display::BLOCK;
};
} // namespace webplatform
} // namespace ve
