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

} // namespace webplatform
} // namespace ve
