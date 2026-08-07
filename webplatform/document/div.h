#pragma once

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {
class Div {
public:
  enum class Colour { RED, GREEN };

  Div(float width, float height, Colour colour);

  void AddChild(std::unique_ptr<Div> child_div);

  float width_;
  float height_;
  Colour colour_;
  std::vector<std::unique_ptr<Div>> childs_;

private:
};
} // namespace webplatform
} // namespace ve
