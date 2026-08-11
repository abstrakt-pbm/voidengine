#pragma once

#include "document/domnode.h"

namespace ve {
namespace webplatform {
class TextElement : public DomNode {
public:
  float font_size = 16.0f;

  // над чертой
  float font_ascent = 12.0f;
  // под чертой
  float font_descent = 4.0f;

  // расстояние между глифами
  float glyph_advance = 8.0f;
  std::string data;
};

} // namespace webplatform
} // namespace ve
