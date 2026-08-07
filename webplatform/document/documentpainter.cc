#include "documentpainter.h"

namespace ve {
namespace webplatform {

std::vector<FillRectCommand>
PainterEngine::Paint(const std::vector<Div> &div_container) {
  float cursor_y = 0.0f;
  std::vector<FillRectCommand> commands;

  for (const auto &div : div_container) {

    FillRectCommand command = CalculateRenderCommands(div);
    command.x = 0;
    command.y = cursor_y;
    cursor_y += command.height;
    commands.push_back(command);
    while (div.child_ != nullptr) {
      auto div_child = *div.child_;
      FillRectCommand child_command = CalculateRenderCommands(div_child);
      child_command.x = command.x;
      child_command.y = command.y;
      commands.push_back(child_command);
      break;
    }
  }

  return commands;
}

FillRectCommand PainterEngine::CalculateRenderCommands(const Div &div) {
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
