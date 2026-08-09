#pragma once

#include "div.h"
#include "document/physicalfragment.h"

#include <variant>
#include <vector>

namespace ve {
namespace webplatform {

struct FillRectCommand {
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;

  int r = 0;
  int g = 0;
  int b = 0;
};

struct DrawBorderCommand {
  float x = 0.0f;
  float y = 0.0f;

  float width = 0.0f;
  float height = 0.0f;

  float border_width = 0.0f;
};

using RenderingCommand = std::variant<FillRectCommand, DrawBorderCommand>;

using DisplayList = std::vector<RenderingCommand>;

class PainterEngine {
public:
  DisplayList Paint(std::vector<Div> &divs);

  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(std::vector<Div> &divs);

  std::unique_ptr<PhysicalFragment> CalculateElementGeometry(Div *div);

private:
  // Получить команду отрисовки одного конкретного элемента
  // тут с курсором только вопрос аккумуляции локальных координат, margine
  // padding надо делать на этапе расчёта геометрии
  DisplayList PaintOneDiv(const PhysicalFragment *fragment, float offset_x,
                          float offset_y);

  DisplayList CalculateRenderCommands(const Div &div);
};

} // namespace webplatform
} // namespace ve
