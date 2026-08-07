#pragma once

#include "div.h"

namespace ve {
namespace webplatform {

struct FillRectCommand {
  float x;
  float y;
  float width;
  float height;

  int r;
  int g;
  int b;
};

FillRectCommand CalculateRenderCommands(const Div &div);

class LayoutEngine {
public:
  FillRectCommand Layout(const Div &div);

private:
  FillRectCommand CalculateRenderCommands(const Div &div);

  float cursor_y = 0.0f;
};

} // namespace webplatform
} // namespace ve
