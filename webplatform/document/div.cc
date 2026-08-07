#include "div.h"
namespace ve {
namespace webplatform {
Div::Div(float width, float height, Colour colour)
    : width_(width), height_(height), colour_(colour) {}

void Div::AddChild(std::unique_ptr<Div> child_div) {
  childs_.push_back(std::move(child_div));
}

} // namespace webplatform
} // namespace ve
