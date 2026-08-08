#pragma once

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

class Style {
public:
  enum class Colour { RED, GREEN, BLUE };

  Style(float width, float height, Colour colour);

  float Width() const;
  float Height() const;
  Colour GetColour() const;
  const Padding &GetPadding() const;
  void SetPadding(Padding paddings);

  float width_;
  float height_;
  Colour colour_;
  Padding padding_;
};
} // namespace webplatform
} // namespace ve
