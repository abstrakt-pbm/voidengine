#pragma once

#include "div.h"
#include "document/physicalfragment.h"

#include <algorithm>
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
  std::vector<FillRectCommand> Paint(std::vector<Div> &divs);

  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(std::vector<Div> &divs);

  std::unique_ptr<PhysicalFragment> CalculateElementGeometry(Div *div);

private:
  FillRectCommand CalculateRenderCommands(const Div &div);
};

} // namespace webplatform
} // namespace ve
