#include "documentpainter.h"
#include "document/physicalfragment.h"
#include "document/style.h"

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

DisplayList
PainterEngine::CalculateRenderCommands(const PhysicalFragment &fragment) {
  FillRectCommand render_command;
  DrawBorderCommand border_command;
  const Div &div = *fragment.owner_;

  border_command.width = fragment.width_;
  border_command.height = fragment.height_;
  border_command.border_width = div.GetStyle().border_width;

  render_command.width = fragment.width_ - 2 * div.GetStyle().border_width;
  render_command.height = fragment.height_ - 2 * div.GetStyle().border_width;

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
    GeometryConstraints geometry_constrains = {.max_width = viewport_width};
    auto child_geom = CalculateElementGeometry(&root_div, geometry_constrains);
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
PainterEngine::CalculateElementGeometry(Div *div,
                                        const GeometryConstraints &constrains) {
  if (div == nullptr) {
    std::cout << "div is null" << std::endl;
    return nullptr;
  }
  const Style &div_style = div->GetStyle();
  const Padding &div_paddings = div_style.GetPadding();
  const Margin &div_margins = div_style.GetMargin();

  std::unique_ptr<PhysicalFragment> fragment =
      std::make_unique<PhysicalFragment>(0, 0, div_style.Height(),
                                         div_style.Width(), div);

  float current_x_cursor = div_paddings.paddig_left + div_style.border_width;
  float current_y_cursor = div_paddings.paddig_top + div_style.border_width;

  // Calculate fragment width
  if (div_style.width_mode_ == Style::WidthMode::FIXED) {
    fragment->width_ = div_style.Width();
  } else if (div_style.width_mode_ == Style::WidthMode::AUTO) {
    fragment->width_ = constrains.max_width - div_margins.margin_left -
                       div_margins.margin_right;
  }
  // content box width
  float parent_content_box_width = fragment->width_ - div_paddings.paddig_left -
                                   div_paddings.paddig_right -
                                   2 * div_style.border_width;
  for (size_t i = 0; i < div->childs_.size(); ++i) {
    auto child_div = div->childs_[i].get();

    GeometryConstraints child_geometry_constrains = {
        .max_width = parent_content_box_width};
    auto child_physical_fragment =
        CalculateElementGeometry(child_div, child_geometry_constrains);
    const Style &child_style = child_div->GetStyle();
    const auto &margin = child_style.GetMargin();

    child_physical_fragment->x_ = current_x_cursor + margin.margin_left;

    child_physical_fragment->y_ = current_y_cursor + margin.margin_top;

    current_y_cursor = child_physical_fragment->y_ +
                       child_physical_fragment->height_ + margin.margin_bottom;

    fragment->AddChild(std::move(child_physical_fragment));
  }

  // Calculate fragment height
  if (div_style.height_mode_ == Style::HeightMode::FIXED) {
    fragment->height_ = div_style.Height();
  } else if (div_style.height_mode_ == Style::HeightMode::AUTO) {
    float fragment_auto_height = current_y_cursor +
                                 div_style.GetPadding().paddig_bottom +
                                 div_style.border_width;
    fragment->height_ = fragment_auto_height;
  }

  return fragment;
};

DisplayList PainterEngine::PaintOneDiv(const PhysicalFragment *fragment,
                                       float offset_x, float offset_y) {
  if (fragment == nullptr) {
    return DisplayList();
  }

  DisplayList commands;

  const Div &div = *fragment->owner_;
  Style div_style = div.GetStyle();

  float cursor_x = offset_x + fragment->x_;
  float cursor_y = offset_y + fragment->y_;

  if (div_style.overflow_ == Style::Overflow::HIDDEN) {
    ClipCommand clip_command;
    clip_command.x = cursor_x;
    clip_command.y = cursor_y;
    clip_command.width = fragment->width_ - 2 * div_style.border_width;
    clip_command.height = fragment->height_ - 2 * div_style.border_width;
    clip_command_stack_.push(clip_command);
    commands.push_back(clip_command);
  }

  if (fragment->owner_) {
    DisplayList fragment_comands = CalculateRenderCommands(*fragment);
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
    float child_cursor_x = cursor_x;
    float child_cursor_y = cursor_y;
    auto child_commands =
        PaintOneDiv(child_fragment, child_cursor_x, child_cursor_y);
    commands.insert(commands.end(), child_commands.begin(),
                    child_commands.end());
  }
  if (div_style.overflow_ == Style::Overflow::HIDDEN) {
    clip_command_stack_.pop();
    if (clip_command_stack_.empty()) {
      commands.push_back(ResetClipCommand{});
    } else {
      commands.push_back(clip_command_stack_.top());
    }
  }
  return commands;
}

} // namespace webplatform
} // namespace ve
