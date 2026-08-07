#include "div.h"
namespace ve {
namespace webplatform {
Div::Div(float width, float height, Colour colour)
    : width_(width), height_(height), colour_(colour) {}

void Div::AddChild(Div *child) { child_ = child; }

} // namespace webplatform
} // namespace ve
