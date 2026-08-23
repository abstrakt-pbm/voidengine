#pragma once

#include "document/physicalfragment.h"

#include <stack>
#include <string>
#include <variant>
#include <vector> // Работает исключительно с PhysicalFragment

namespace ve::webplatform {
struct ResetClipCommand {};

struct DrawImageCommand {
  float x;
  float y;
  float width;
  float height;
  std::string path_to_png_img;
};

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

using RenderingCommand =
    std::variant<FillRectCommand, DrawBorderCommand, ClipCommand,
                 ResetClipCommand, DrawTextCommand, DrawImageCommand>;

using DisplayList = std::vector<RenderingCommand>;
// Зоны ответственности Формирование комманд отрисовки
class PainterEngine {
public:
  DisplayList Paint(const PhysicalFragment &fragment);
  DisplayList PaintFragment(const PhysicalFragment &fragment, float offset_x,
                            float offset_y);
  DisplayList PaintDiv(const BoxPhysicalFragment &fragment, float offset_x,
                       float offset_y);
  DisplayList PaintText(const TextPhysicalFragment &fragment, float offset_x,
                        float offset_y);

  DisplayList PaintImage(const ImagePhysicalFragment &fragment, float offset_x,
                         float offset_y);
  DisplayList MakeDrawCommands(const BoxPhysicalFragment &fragment);

  ClipCommand IntersectClipCommands(const ClipCommand &current_clip_command,
                                    const ClipCommand &new_clip_command);

  // Active clipping state during fragment tree traversal.
  std::stack<ClipCommand> clip_command_stack_;
};
} // namespace ve::webplatform
