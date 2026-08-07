#pragma once

#include "div.h"

#include <vector>

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

class PainterEngine {
public:
  std::vector<FillRectCommand> Paint(const std::vector<Div> &div_container);

private:
  FillRectCommand CalculateRenderCommands(const Div &div);
};

} // namespace webplatform
} // namespace ve
