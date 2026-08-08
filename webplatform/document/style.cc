#include "document/style.h"

namespace ve {
namespace webplatform {

Style::Style(float width, float height, Style::Colour colour)
    : width_(width), height_(height), colour_(colour) {}

float Style::Width() const { return width_; }

float Style::Height() const { return height_; }

Style::Colour Style::GetColour() const { return colour_; }

const Padding &Style::GetPadding() const { return padding_; }

void Style::SetPadding(Padding paddings) {
  padding_.paddig_left = paddings.paddig_left;
  padding_.paddig_right = paddings.paddig_right;
  padding_.paddig_top = paddings.paddig_top;
  padding_.paddig_bottom = paddings.paddig_bottom;
}

Padding::Padding(float paddings) {
  paddig_left = paddings;
  paddig_right = paddings;
  paddig_top = paddings;
  paddig_bottom = paddings;
}

Padding::Padding(float paddig_lefts, float paddig_rights, float paddig_tops,
                 float paddig_bottoms) {
  paddig_left = paddig_lefts;
  paddig_right = paddig_rights;
  paddig_top = paddig_tops;
  paddig_bottom = paddig_bottoms;
}

} // namespace webplatform
} // namespace ve
