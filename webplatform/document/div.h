#pragma once

namespace ve {
namespace webplatform {
class Div {
public:
  enum class Colour { RED, GREEN };

  Div(float width, float height, Colour colour);

  float width_;
  float height_;
  Colour colour_;

private:
};
} // namespace webplatform
} // namespace ve
