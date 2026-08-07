#include "documentpainter.h"

namespace ve {
namespace webplatform {

FillRectCommand LayoutEngine::Layout(const Div &div) {
  FillRectCommand command = CalculateRenderCommands(div);
  command.x = 0;
  command.y = cursor_y;
  cursor_y += command.height;
  return command;
}

FillRectCommand LayoutEngine::CalculateRenderCommands(const Div &div) {
  FillRectCommand render_command;
  render_command.width = div.width_;
  render_command.height = div.height_;
  if (div.colour_ == Div::Colour::RED) {
    render_command.r = 255;
    render_command.g = 0;
    render_command.b = 0;
  } else if (div.colour_ == Div::Colour::GREEN) {
    render_command.r = 0;
    render_command.g = 255;
    render_command.b = 0;
  }
  return render_command;
}

} // namespace webplatform
} // namespace ve
