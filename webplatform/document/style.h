#pragma once

#include <cstdint>
#include <optional>

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

class Height {
public:
  enum class HeightMode { FIXED, AUTO };
  Height() = default;
  Height(HeightMode mode);
  Height(float value);

  HeightMode Mode() const;
  std::optional<float> Value() const;

private:
  float value_ = 0.f;
  HeightMode mode_ = HeightMode::FIXED;
};

class Width {
public:
  enum class WidthMode { FIXED, AUTO };
  Width() = default;
  Width(WidthMode mode);
  Width(float value);

  WidthMode Mode() const;
  std::optional<float> Value() const;

private:
  float value_ = 0.f;
  WidthMode mode_ = WidthMode::FIXED;
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
  enum class Overflow { VISIBLE, HIDDEN };

  Style(class Width width, class Height height, Colour colour);
  Style() = default;

  class Width Width() const;
  class Height Height() const;
  Colour GetColour() const;

  const Padding &GetPadding() const;
  const Margin &GetMargin() const;

  void SetPadding(Padding paddings);
  void SetMargin(Margin margins);

  class Width width_;
  class Height height_;

  Overflow overflow_ = Overflow::VISIBLE;

  float border_width = 0.0f;

  Colour colour_;
  Padding padding_;
  Margin margin_;

  // layout algo choose
  Display display_ = Display::BLOCK;
};
} // namespace webplatform
} // namespace ve
