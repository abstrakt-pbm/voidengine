#pragma once

namespace ve {
namespace webplatform {
class Style {
public:
  enum class Colour { RED, GREEN, BLUE };

  Style(float width, float height, Colour colour);

  float Width() const;
  float Height() const;
  Colour GetColour() const;

  float width_;
  float height_;
  Colour colour_;
};
} // namespace webplatform
} // namespace ve
