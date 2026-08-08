#include "document/style.h"

namespace ve {
namespace webplatform {

Style::Style(float width, float height, Style::Colour colour)
    : width_(width), height_(height), colour_(colour) {}

float Style::Width() const { return width_; }

float Style::Height() const { return height_; }

Style::Colour Style::GetColour() const { return colour_; }

} // namespace webplatform
} // namespace ve
