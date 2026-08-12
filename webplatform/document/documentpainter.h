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

// Работает исключительно с DomNode
class GeometryEngine {
public:
  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(const DomNode &dom_node);

  std::unique_ptr<PhysicalFragment>
  CalculateElementGeometry(const DomNode &dom_node,
                           const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateTextGeometry(const TextElement *text_element,
                        const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateDivGeometry(const Div *div, const GeometryConstraints &constrains);

  float viewport_width = 1280.0f;
};

// Работает исключительно с PhysicalFragment
class PainterEngine {
public:
  DisplayList Paint(const PhysicalFragment *fragment);

  DisplayList PaintFragment(const PhysicalFragment *fragment, float offset_x,
                            float offset_y);

  DisplayList PaintDiv(const PhysicalFragment *fragment, float offset_x,
                       float offset_y);
  DisplayList PaintText(const TextPhysicalFragment *fragment, float offset_x,
                        float offset_y);
  DisplayList CalculateRenderCommands(const PhysicalFragment &fragment);

  // ViewPort
  std::stack<ClipCommand> clip_command_stack_;
};

} // namespace webplatform
} // namespace ve
