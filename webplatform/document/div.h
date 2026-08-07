#pragma once

namespace ve {
namespace webplatform {
class Div {
public:
  enum class Colour { RED, GREEN };

  Div(float width, float height, Colour colour);

  void AddChild(Div *child);

  float width_;
  float height_;
  Colour colour_;
  Div *child_ = nullptr;

private:
};
} // namespace webplatform
} // namespace ve
