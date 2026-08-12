#pragma once

#include "div.h"
#include "document/physicalfragment.h"
#include "document/textelement.h"

#include <stack>
#include <string>
#include <variant>
#include <vector>

namespace ve {
namespace webplatform {

struct ResetClipCommand {};

struct DrawTextCommand {
  float x;
  float baseline_y;
  float font_size;

  std::string text;
};

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

using RenderingCommand =
    std::variant<FillRectCommand, DrawBorderCommand, ClipCommand,
                 ResetClipCommand, DrawTextCommand>;

using DisplayList = std::vector<RenderingCommand>;

class GeometryEngine {
public:
  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(std::vector<Div> &divs);
  std::unique_ptr<PhysicalFragment>
  CalculateElementGeometry(const DomNode &dom_node,
                           const GeometryConstraints &constrains);

private:
  std::unique_ptr<PhysicalFragment>
  CalculateTextGeometry(const TextElement *text_element,
                        const GeometryConstraints &constrains);

  std::unique_ptr<PhysicalFragment>
  CalculateDivGeometry(const Div *div, const GeometryConstraints &constrains);

  float viewport_width = 1280.0f;
};

class PainterEngine {
public:
  DisplayList Paint(std::vector<Div> &divs);

  DisplayList PaintOneDiv(const PhysicalFragment *fragment, float offset_x,
                          float offset_y);

  DisplayList PaintText(const TextPhysicalFragment *fragment, float offset_x,
                        float offset_y);
  DisplayList CalculateRenderCommands(const PhysicalFragment &fragment);

  // ViewPort
  std::stack<ClipCommand> clip_command_stack_;
  GeometryEngine geometry_engine;
};

} // namespace webplatform
} // namespace ve
