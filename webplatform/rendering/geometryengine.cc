#include "geometryengine.h"

#include "document/div.h"
#include "document/domnode.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/style.h"
#include "document/textelement.h"

#include <memory>

namespace ve {
namespace webplatform {

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
