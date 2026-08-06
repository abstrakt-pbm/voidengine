#include "documentpainter.h"

namespace ve {
namespace webplatform {
FillRectCommand CalculateRenderCommands(const Div &div) {
  FillRectCommand render_command;
  render_command.x = div.x_;
  render_command.y = div.y_;
  render_command.width = div.width_;
  render_command.height = div.height_;
  if (div.colour_ == Div::Colour::RED) {
    render_command.r = 255;
    render_command.g = 0;
    render_command.b = 0;
  }
  return render_command;
}
} // namespace webplatform
} // namespace ve
