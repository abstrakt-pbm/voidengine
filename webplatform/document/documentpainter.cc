#include "documentpainter.h"
#include "document/physicalfragment.h"

#include <iostream>
#include <memory>

namespace ve {
namespace webplatform {

std::vector<FillRectCommand>
PainterEngine::Paint(std::vector<Div> &div_container) {
  float cursor_y = 0.0f;
  std::vector<FillRectCommand> commands;

  std::cout << CalculateDocumentGeometry(div_container)->ToString()
            << std::endl;
  /*
for (const auto &div : div_container) {

// Геометрию расчитываем отдельно от команд, так как команды строяются от
// родителя к ребёнку геометрия родителя строится когда понятны размеры
// детей

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
*/

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

std::unique_ptr<PhysicalFragment>
PainterEngine::CalculateDocumentGeometry(std::vector<Div> &divs) {

  auto root_physical_fragment = std::make_unique<PhysicalFragment>(0, 0, 0, 0);
  float root_h = 0.0f;
  float max_w = 0.0f;
  for (size_t i = 0; i < divs.size(); ++i) {
    Div &root_div = divs[i];
    auto child_geom = CalculateElementGeometry(&root_div);
    root_h += child_geom->height_;
    if (max_w < child_geom->width_) {
      max_w = child_geom->width_;
    }
    root_physical_fragment->AddChild(std::move(child_geom));
  };
  root_physical_fragment->height_ = root_h;
  root_physical_fragment->width_ = max_w;
  return root_physical_fragment;
}

std::unique_ptr<PhysicalFragment>
PainterEngine::CalculateElementGeometry(Div *div) {
  if (div == nullptr) {
    std::cout << "physical fragment is null" << std::endl;
    return nullptr;
  }
  // длину и ширину рута можно посчитать только после окончательных размерах
  // детей
  auto fragment =
      std::make_unique<PhysicalFragment>(0, 0, div->height_, div->width_);

  float current_x_cursor = 0.0f;
  float current_y_cursor = 0.0f;
  for (size_t i = 0; i < div->childs_.size(); ++i) {
    auto child_div = div->childs_[i].get();
    auto child_physical_fragment = CalculateElementGeometry(child_div);
    child_physical_fragment->x_ = current_x_cursor;
    child_physical_fragment->y_ = current_y_cursor;
    current_y_cursor += child_physical_fragment->height_;

    fragment->AddChild(std::move(child_physical_fragment));
  }

  return fragment;
};

} // namespace webplatform
} // namespace ve
