#pragma once

#include "div.h"
#include "document/physicalfragment.h"

#include <variant>
#include <vector>

namespace ve {
namespace webplatform {

struct ResetClipCommand {};

struct ClipCommand {
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

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

struct GeometryConstraints {
  float max_width = 0.0f;
};

using RenderingCommand = std::variant<FillRectCommand, DrawBorderCommand,
                                      ClipCommand, ResetClipCommand>;

using DisplayList = std::vector<RenderingCommand>;

class PainterEngine {
public:
  DisplayList Paint(std::vector<Div> &divs);

  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(std::vector<Div> &divs);

  std::unique_ptr<PhysicalFragment>
  CalculateElementGeometry(Div *div, const GeometryConstraints &constrains);

private:
  // Получить команду отрисовки одного конкретного элемента
  // тут с курсором только вопрос аккумуляции локальных координат, margine
  // padding надо делать на этапе расчёта геометрии
  DisplayList PaintOneDiv(const PhysicalFragment *fragment, float offset_x,
                          float offset_y);

  DisplayList CalculateRenderCommands(const PhysicalFragment &fragment);

  // ViewPort
  float viewport_width = 1280.0f;
};

} // namespace webplatform
} // namespace ve
