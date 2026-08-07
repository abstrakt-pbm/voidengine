#pragma once

namespace ve {
namespace webplatform {
class Div {
public:
  enum class Colour { RED, GREEN };

  Div(float x, float y, float width, float height, Colour colour);

  float x_;
  float y_;
  float width_;
  float height_;
  Colour colour_;

private:
};
} // namespace webplatform
} // namespace ve
