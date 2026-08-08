#include "documentpainter.h"
#include "document/physicalfragment.h"

#include <iostream>
#include <memory>

namespace ve {
namespace webplatform {

DisplayList PainterEngine::Paint(std::vector<Div> &divs) {
  float cursor_y = 0.0f;
  float cursor_x = 0.0f;
  DisplayList commands;

  auto document_geometry = CalculateDocumentGeometry(divs);
  for (size_t i = 0; i < document_geometry->child_fragments_.size(); ++i) {
    float local_cursor_y = cursor_y;
    float local_cursor_x = cursor_x;

    auto child_fragment = document_geometry->child_fragments_[i].get();
    auto child_commands =
        PaintOneDiv(child_fragment, local_cursor_x, local_cursor_y);
    commands.insert(commands.end(), child_commands.begin(),
                    child_commands.end());
    cursor_y += child_fragment->height_;
  }
  return commands;
}

DisplayList PainterEngine::CalculateRenderCommands(const Div &div) {
  FillRectCommand render_command;
  DrawBorderCommand border_command;

  border_command.width = div.GetStyle().Width();
  border_command.height = div.GetStyle().Height();
  border_command.border_width = div.GetStyle().border_width;

  render_command.width =
      div.GetStyle().Width() - 2 * div.GetStyle().border_width;
  render_command.height =
      div.GetStyle().Height() - 2 * div.GetStyle().border_width;

  if (div.GetStyle().GetColour() == Style::Colour::RED) {
    render_command.r = 255;
    render_command.g = 0;
    render_command.b = 0;
  } else if (div.GetStyle().GetColour() == Style::Colour::GREEN) {
    render_command.r = 0;
    render_command.g = 255;
    render_command.b = 0;
  } else if (div.GetStyle().GetColour() == Style::Colour::BLUE) {
    render_command.r = 0;
    render_command.g = 0;
    render_command.b = 255;
  }
  return DisplayList{border_command, render_command};
}

std::unique_ptr<PhysicalFragment>
PainterEngine::CalculateDocumentGeometry(std::vector<Div> &divs) {

  auto root_physical_fragment =
      std::make_unique<PhysicalFragment>(0, 0, 0, 0, nullptr);
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
    std::cout << "div is null" << std::endl;
    return nullptr;
  }

  auto fragment = std::make_unique<PhysicalFragment>(
      0, 0, div->GetStyle().Height(), div->GetStyle().Width(), div);

  float current_x_cursor = 0.0f;
  float current_y_cursor = 0.0f;
  for (size_t i = 0; i < div->childs_.size(); ++i) {
    auto child_div = div->childs_[i].get();
    auto child_physical_fragment = CalculateElementGeometry(child_div);
    child_physical_fragment->x_ = current_x_cursor;
    child_physical_fragment->y_ = current_y_cursor;
    child_physical_fragment->owner_ = child_div;
    current_y_cursor += child_physical_fragment->height_;

    fragment->AddChild(std::move(child_physical_fragment));
  }

  return fragment;
};

DisplayList PainterEngine::PaintOneDiv(const PhysicalFragment *fragment,
                                       float offset_x, float offset_y) {
  if (fragment == nullptr) {
    return DisplayList();
  }

  DisplayList commands;
  float cursor_x = offset_x + fragment->x_;
  float cursor_y = offset_y + fragment->y_;

  const Div &div = *fragment->owner_;
  Style div_style = div.GetStyle();
  if (fragment->owner_) {
    // комманда отрисовки рамки
    // потом закрасить рект

    DisplayList fragment_comands = CalculateRenderCommands(div);
    for (auto &render_command : fragment_comands) {
      std::visit(
          [&](auto &command) {
            using Command = std::decay_t<decltype(command)>;

            if constexpr (std::is_same_v<Command,
                                         ve::webplatform::FillRectCommand>) {
              command.x = cursor_x + div_style.border_width;
              command.y = cursor_y + div_style.border_width;
            } else if constexpr (std::is_same_v<
                                     Command,
                                     ve::webplatform::DrawBorderCommand>) {
              command.x = cursor_x;
              command.y = cursor_y;
            }
          },
          render_command);
    }
    commands.insert(commands.end(), fragment_comands.begin(),
                    fragment_comands.end());
  }

  for (size_t i = 0; i < fragment->child_fragments_.size(); ++i) {
    auto child_fragment = fragment->child_fragments_[i].get();
    float child_cursor_x =
        cursor_x + div_style.GetPadding().paddig_left + div_style.border_width;
    float child_cursor_y =
        cursor_y + div_style.GetPadding().paddig_top + div_style.border_width;
    auto child_commands =
        PaintOneDiv(child_fragment, child_cursor_x, child_cursor_y);
    commands.insert(commands.end(), child_commands.begin(),
                    child_commands.end());
  }

  return commands;
}

} // namespace webplatform
} // namespace ve
