#include "geometryengine.h"

#include "document/containernode.h"
#include "document/domnode.h"
#include "document/elementnode.h"
#include "document/htmlelementnode.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/style.h"
#include "document/textelement.h"

#include "rendering/layoutcontext.h"

#include <memory>

namespace ve {
namespace webplatform {

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateElementGeometry(
    const DomNode &dom_node, const GeometryConstraints &constrains) {
  if (auto *text = dynamic_cast<const TextElement *>(&dom_node)) {
    return CalculateTextGeometry(*text, constrains);
  } else if (auto *img = dynamic_cast<const ImageElement *>(&dom_node)) {
    return CalculateImageGeometry(*img, constrains);
  }
  if (auto *container_node = dynamic_cast<const ElementNode *>(&dom_node)) {
    return CalculateContainerNodeGeometry(*container_node, constrains);
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
GeometryEngine::CalculateImageGeometry(const ImageElement &img,
                                       const GeometryConstraints &constrains) {
  return std::make_unique<ImagePhysicalFragment>(0, 0, img.height_, img.widht_,
                                                 img.path_to_img_);
}

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateDocumentGeometry(
    const HtmlElementNode &htmlelementnode) {
  //  Стандартные стили рута 100% выставляются вне движка геометрии
  Colour colour(Colour::ColourName::WHITE);
  std::unique_ptr<Style> html_style_ptr = std::make_unique<Style>();
  html_style_ptr->colour_ = colour;
  html_style_ptr->height_mode_ = Style::HeightMode::AUTO;
  html_style_ptr->width_mode_ = Style::WidthMode::AUTO;
  HtmlElementNode &nonconst_htmlelem =
      const_cast<HtmlElementNode &>(htmlelementnode);
  const Style &html_style = *html_style_ptr.get();
  LayoutContext root_layout_context = LayoutContext(html_style);

  BoxPhysicalFragment::Overflow bf_overflow =
      BoxPhysicalFragment::Overflow::VISIBLE;

  nonconst_htmlelem.SetStyle(std::move(html_style_ptr));
  const GeometryConstraints constrains{.max_width = viewport_width};
  return CalculateElementGeometry(nonconst_htmlelem, constrains);
}

std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateContainerNodeGeometry(
    const ElementNode &container_node, const GeometryConstraints &constrains) {
  const Style &container_node_style = *container_node.GetStyle();
  const Margin &container_node_margin = container_node_style.GetMargin();
  const Padding &container_node_padding = container_node_style.GetPadding();
  const Colour &container_node_colour = container_node_style.GetColour();
  LayoutContext layout_context = LayoutContext(container_node_style);

  // потом ибавиться от проблемы трансляции типов Overflow
  BoxPhysicalFragment::Overflow container_node_overflow =
      container_node_style.overflow_ == Style::Overflow::VISIBLE
          ? BoxPhysicalFragment::Overflow::VISIBLE
          : BoxPhysicalFragment::Overflow::HIDDEN;

  auto fragment = std::make_unique<BoxPhysicalFragment>(
      0, 0, container_node_style.Height(), container_node_style.Width(),
      container_node_style.border_width, container_node_overflow,
      container_node_colour.red_, container_node_colour.green_,
      container_node_colour.blue_);

  // Логику авто ширины
  if (container_node_style.width_mode_ == Style::WidthMode::FIXED) {
    fragment->width_ = container_node_style.Width();
  } else {
    fragment->width_ = constrains.max_width -
                       container_node_margin.margin_left -
                       container_node_margin.margin_right;
  }

  float padding_left = container_node_padding.paddig_left;
  float padding_right = container_node_padding.paddig_right;

  const GeometryConstraints new_constrains{
      .max_width = fragment->width_ - padding_left - padding_right -
                   2 * container_node_style.border_width};

  float childs_bottom =
      container_node_padding.paddig_top + container_node_style.border_width;
  for (const auto &child_element : container_node.childs_) {
    auto child_fragment =
        CalculateElementGeometry(*child_element, new_constrains);

    const Style *child_style = nullptr;
    if (auto *child_node =
            dynamic_cast<const ElementNode *>(child_element.get())) {
      child_style = child_node->GetStyle();
    }
    layout_context.LayoutChild(child_style, *child_fragment.get());
    float child_margin_bottom = 0.0f;
    if (child_style) {
      child_margin_bottom = child_style->GetMargin().margin_bottom;
    }

    float current_child_bottom =
        child_fragment->y_ + child_fragment->height_ + child_margin_bottom;
    childs_bottom = std::max(childs_bottom, current_child_bottom);
    fragment->AddChild(std::move(child_fragment));
  }

  // логика авто высоты
  if (container_node_style.height_mode_ == Style::HeightMode::FIXED) {
    fragment->height_ = container_node_style.Height();
  } else {
    fragment->height_ = childs_bottom + container_node_padding.paddig_bottom +
                        container_node_style.border_width;
  }

  return fragment;
}

} // namespace webplatform
} // namespace ve
