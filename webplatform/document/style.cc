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

const Margin &Style::GetMargin() const { return margin_; }

void Style::SetMargin(Margin margins) {
  margin_.margin_left = margins.margin_left;
  margin_.margin_right = margins.margin_right;
  margin_.margin_top = margins.margin_top;
  margin_.margin_bottom = margins.margin_bottom;
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

Margin::Margin(float margins) {
  margin_left = margins;
  margin_right = margins;
  margin_top = margins;
  margin_bottom = margins;
}

Margin::Margin(float margin_lefts, float margin_rights, float margin_tops,
               float margin_bottoms) {
  margin_left = margin_lefts;
  margin_right = margin_rights;
  margin_top = margin_tops;
  margin_bottom = margin_bottoms;
}

} // namespace webplatform
} // namespace ve
