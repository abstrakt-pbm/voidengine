#include "documentpainter.h"
#include "document/domnode.h"
#include "document/physicalfragment.h"
#include "document/style.h"
#include "document/textelement.h"

#include <iostream>
#include <memory>

namespace ve {
namespace webplatform {

DisplayList PainterEngine::Paint(const PhysicalFragment &fragment) {
  DisplayList commands;
  DisplayList dom_root_commmands = PaintFragment(fragment, 0, 0);

  commands.insert(commands.end(), dom_root_commmands.begin(),
                  dom_root_commmands.end());
  return commands;
}

DisplayList
PainterEngine::MakeDrawCommands(const BoxPhysicalFragment &fragment) {
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

DisplayList PainterEngine::PaintFragment(const PhysicalFragment &fragment,
                                         float offset_x, float offset_y) {
  if (auto *text_elem = dynamic_cast<const TextPhysicalFragment *>(&fragment)) {
    return PaintText(*text_elem, offset_x, offset_y);
  } else if (auto *div_elem =
                 dynamic_cast<const BoxPhysicalFragment *>(&fragment)) {
    return PaintDiv(*div_elem, offset_x, offset_y);
  }
  return DisplayList();
}

DisplayList PainterEngine::PaintDiv(const BoxPhysicalFragment &fragment,
                                    float offset_x, float offset_y) {
  DisplayList commands;

  const Div &div = *fragment.owner_;
  Style div_style = div.GetStyle();

  float cursor_x = offset_x + fragment.x_;
  float cursor_y = offset_y + fragment.y_;

  if (div_style.overflow_ == Style::Overflow::HIDDEN) {
    ClipCommand clip_command;
    clip_command.x = cursor_x;
    clip_command.y = cursor_y;
    clip_command.width = fragment.width_ - 2 * div_style.border_width;
    clip_command.height = fragment.height_ - 2 * div_style.border_width;
    clip_command_stack_.push(clip_command);
    commands.push_back(clip_command);
  }

  DisplayList fragment_comands = MakeDrawCommands(fragment);

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

  for (size_t i = 0; i < fragment.child_fragments_.size(); ++i) {
    auto child_fragment = fragment.child_fragments_[i].get();
    float child_cursor_x = cursor_x;
    float child_cursor_y = cursor_y;

    DisplayList child_commands =
        PaintFragment(*child_fragment, child_cursor_x, child_cursor_y);

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

DisplayList PainterEngine::PaintText(const TextPhysicalFragment &fragment,
                                     float offset_x, float offset_y) {
  DisplayList commands;
  const TextElement *text_element = fragment.owner_;

  if (text_element == nullptr) {
    return commands;
  }

  for (const auto &text_line : fragment.text_lines_) {

    DrawTextCommand command;

    command.x = offset_x + text_line.x_;
    command.baseline_y = offset_y + text_line.y_ + fragment.baseline_;

    command.font_size = text_element->font_size;
    command.text = text_line.payload_;

    commands.push_back(command);
  }

  return commands;
}

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateElementGeometry(
    const DomNode &dom_node, const GeometryConstraints &constrains) {
  if (auto *divc = dynamic_cast<const Div *>(&dom_node)) {
    Div *div = const_cast<Div *>(divc);
    return CalculateDivGeometry(*div, constrains);
  } else if (auto *text = dynamic_cast<const TextElement *>(&dom_node)) {
    return CalculateTextGeometry(*text, constrains);
  }
  return nullptr;
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateTextGeometry(const TextElement &text_element,
                                      const GeometryConstraints &constrains) {
  // x,y, width, height прямоугольника
  // baseline вдоль которой будет располагаться глифы
  // Глифы в порядке отрисовки

  float text_height = text_element.font_ascent + text_element.font_descent;
  float text_width = text_element.data.size() * text_element.glyph_advance;
  float baseline = text_element.font_ascent;

  size_t glyphs_in_line =
      static_cast<size_t>(constrains.max_width / text_element.glyph_advance);
  glyphs_in_line = std::max<size_t>(1, glyphs_in_line);

  size_t text_lines_count =
      (text_element.data.size() + glyphs_in_line - 1) / glyphs_in_line;

  float cursor_y = 0.0f;
  size_t data_cursor = 0;

  std::unique_ptr<TextPhysicalFragment> text_fragment =
      std::make_unique<TextPhysicalFragment>(
          0, 0, text_height * text_lines_count, text_element.text_width,
          baseline, &text_element);

  for (size_t i = 0; i < text_lines_count; ++i) {
    std::string text_substring =
        text_element.data.substr(data_cursor, glyphs_in_line);
    float substring_width = text_substring.size() * text_element.glyph_advance;

    text_fragment->text_lines_.push_back(TextLineFragment(
        0, cursor_y, text_height, substring_width, text_substring));
    data_cursor += glyphs_in_line;
    cursor_y += text_height;
  }

  return text_fragment;
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateDivGeometry(const Div &div,
                                     const GeometryConstraints &constrains) {
  const Style &div_style = div.GetStyle();
  const Padding &div_paddings = div_style.GetPadding();
  const Margin &div_margins = div_style.GetMargin();

  auto fragment = std::make_unique<BoxPhysicalFragment>(
      0, 0, div_style.Height(), div_style.Width(), &div);

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
  // div layout
  // layout алгоритмы нужно вынести отдельно
  for (size_t i = 0; i < div.childs_.size(); ++i) {
    auto child_div = div.childs_[i].get();
    const DomNode &child_dom_node = *child_div;
    GeometryConstraints child_geometry_constrains = {
        .max_width = parent_content_box_width};
    auto child_physical_fragment =
        CalculateElementGeometry(child_dom_node, child_geometry_constrains);

    // block layout algo
    if (Div *child_div_elem = dynamic_cast<Div *>(child_div)) {
      const Style &child_style = child_div_elem->GetStyle();
      const auto &margin = child_style.GetMargin();

      child_physical_fragment->x_ = current_x_cursor + margin.margin_left;

      child_physical_fragment->y_ = current_y_cursor + margin.margin_top;

      current_y_cursor = child_physical_fragment->y_ +
                         child_physical_fragment->height_ +
                         margin.margin_bottom;
    } else if (TextElement *child_div_elem =
                   dynamic_cast<TextElement *>(child_div)) {
      current_y_cursor =
          child_physical_fragment->y_ + child_physical_fragment->height_;
    }
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
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateDocumentGeometry(const DomNode &dom_node) {
  if (const auto *root_div_ptr = dynamic_cast<const Div *>(&dom_node)) {
    const Div &root_div = *root_div_ptr;
    GeometryConstraints geometry_constrains = {.max_width = viewport_width};
    return CalculateElementGeometry(root_div, geometry_constrains);
  }
  return nullptr;
}
} // namespace webplatform
} // namespace ve
