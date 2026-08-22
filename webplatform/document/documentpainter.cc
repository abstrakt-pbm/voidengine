#include "documentpainter.h"
#include "document/domnode.h"
#include "document/imageelement.h"
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
  } else if (auto *img_elem =
                 dynamic_cast<const ImagePhysicalFragment *>(&fragment)) {
    return PaintImage(*img_elem, offset_x, offset_y);
  }
  return DisplayList();
}

DisplayList PainterEngine::PaintImage(const ImagePhysicalFragment &fragment,
                                      float offset_x, float offset_y) {

  DrawImageCommand command;

  command.x = offset_x + fragment.x_;

  command.y = offset_y + fragment.y_;

  command.width = fragment.width_;

  command.height = fragment.height_;

  command.path_to_png_img = fragment.path_to_img_;

  return DisplayList{command};
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
    clip_command.x = cursor_x + div_style.border_width;
    clip_command.y = cursor_y + div_style.border_width;
    clip_command.width = fragment.width_ - 2 * div_style.border_width;
    clip_command.height = fragment.height_ - 2 * div_style.border_width;

    ClipCommand result_command = clip_command;
    if (!clip_command_stack_.empty()) {
      result_command =
          IntersectClipCommands(clip_command_stack_.top(), clip_command);
    }

    clip_command_stack_.push(result_command);
    commands.push_back(result_command);
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

ClipCommand
PainterEngine::IntersectClipCommands(const ClipCommand &current_clip_command,
                                     const ClipCommand &new_clip_command) {
  const float left = std::max(current_clip_command.x, new_clip_command.x);

  const float top = std::max(current_clip_command.y, new_clip_command.y);

  const float right =
      std::min(current_clip_command.x + current_clip_command.width,
               new_clip_command.x + new_clip_command.width);

  const float bottom =
      std::min(current_clip_command.y + current_clip_command.height,
               new_clip_command.y + new_clip_command.height);

  ClipCommand command;
  command.x = left;
  command.y = top;
  command.width = std::max(0.0f, right - left);
  command.height = std::max(0.0f, bottom - top);

  return command;
}

DisplayList PainterEngine::PaintText(const TextPhysicalFragment &fragment,
                                     float offset_x, float offset_y) {

  DisplayList commands;

  const TextElement *text_element = fragment.owner_;

  if (text_element == nullptr) {
    return commands;
  }

  const float fragment_x = offset_x + fragment.x_;

  const float fragment_y = offset_y + fragment.y_;

  for (const auto &text_line : fragment.text_lines_) {
    DrawTextCommand command;

    command.x = fragment_x + text_line.x_;

    command.baseline_y = fragment_y + text_line.y_ + fragment.baseline_;

    command.font_size = text_element->font_size;

    command.text = text_line.payload_;

    commands.push_back(command);
  }

  return commands;
}

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateElementGeometry(
    const DomNode &dom_node, const GeometryConstraints &constrains) {
  if (auto *div = dynamic_cast<const Div *>(&dom_node)) {
    return CalculateDivGeometry(*div, constrains);
  } else if (auto *text = dynamic_cast<const TextElement *>(&dom_node)) {
    return CalculateTextGeometry(*text, constrains);
  } else if (auto *img = dynamic_cast<const ImageElement *>(&dom_node)) {
    return CalculateImageGeometry(*img, constrains);
  }
  return nullptr;
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateTextGeometry(const TextElement &text_element,
                                      const GeometryConstraints &constraints) {

  const float line_height =
      text_element.font_ascent + text_element.font_descent;

  const float baseline = text_element.font_ascent;

  size_t glyphs_in_line =
      static_cast<size_t>(constraints.max_width / text_element.glyph_advance);

  glyphs_in_line = std::max<size_t>(1, glyphs_in_line);

  const size_t line_count =
      (text_element.data.size() + glyphs_in_line - 1) / glyphs_in_line;

  auto fragment = std::make_unique<TextPhysicalFragment>(
      0, 0, line_height * line_count, 0.0f, baseline, &text_element);

  float cursor_y = 0.0f;
  float max_line_width = 0.0f;
  size_t data_cursor = 0;

  for (size_t i = 0; i < line_count; ++i) {
    std::string line = text_element.data.substr(data_cursor, glyphs_in_line);

    const float line_width = line.size() * text_element.glyph_advance;

    max_line_width = std::max(max_line_width, line_width);

    fragment->text_lines_.push_back(
        TextLineFragment(0, cursor_y, line_height, line_width, line));

    data_cursor += glyphs_in_line;
    cursor_y += line_height;
  }

  fragment->width_ = max_line_width;

  return fragment;
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateDivGeometry(const Div &div,
                                     const GeometryConstraints &constraints) {

  const Style &style = div.GetStyle();
  const Padding &padding = style.GetPadding();
  const Margin &margin = style.GetMargin();

  auto fragment = std::make_unique<BoxPhysicalFragment>(0, 0, style.Height(),
                                                        style.Width(), &div);

  //
  // 1. Calculate own width.
  //

  if (style.width_mode_ == Style::WidthMode::FIXED) {
    fragment->width_ = style.Width();
  } else {
    fragment->width_ =
        constraints.max_width - margin.margin_left - margin.margin_right;
  }

  //
  // 2. Calculate content box.
  //

  const float content_x = padding.paddig_left + style.border_width;

  float current_y = padding.paddig_top + style.border_width;

  const float content_width = fragment->width_ - padding.paddig_left -
                              padding.paddig_right - 2.0f * style.border_width;

  //
  // 3. Normal vertical flow.
  //

  for (const auto &child : div.childs_) {
    const DomNode &child_node = *child;

    GeometryConstraints child_constraints{
        .max_width = content_width,
    };

    auto child_fragment =
        CalculateElementGeometry(child_node, child_constraints);

    //
    // Пока margins существуют только у Div.
    //

    float margin_left = 0.0f;
    float margin_top = 0.0f;
    float margin_bottom = 0.0f;

    if (const auto *child_div = dynamic_cast<const Div *>(&child_node)) {

      const Margin &child_margin = child_div->GetStyle().GetMargin();

      margin_left = child_margin.margin_left;
      margin_top = child_margin.margin_top;
      margin_bottom = child_margin.margin_bottom;
    }

    //
    // Place child in parent's content box.
    //

    child_fragment->x_ = content_x + margin_left;

    child_fragment->y_ = current_y + margin_top;

    //
    // Advance normal-flow cursor.
    //

    current_y = child_fragment->y_ + child_fragment->height_ + margin_bottom;

    fragment->AddChild(std::move(child_fragment));
  }

  //
  // 4. Calculate own height.
  //

  if (style.height_mode_ == Style::HeightMode::FIXED) {
    fragment->height_ = style.Height();
  } else {
    fragment->height_ = current_y + padding.paddig_bottom + style.border_width;
  }

  return fragment;
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateImageGeometry(const ImageElement &img,
                                       const GeometryConstraints &constrains) {
  return std::make_unique<ImagePhysicalFragment>(0, 0, img.height_, img.widht_,
                                                 img.path_to_img_);
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
