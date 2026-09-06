#include "geometryengine.h"

#include "document/physicalfragment.h"
#include "document/style.h"

#include "rendering/layoutcontext.h"
#include "rendering/layoutengine/layoutbox.h"
#include "rendering/layoutengine/layoutimage.h"

#include <iostream>
#include <memory>

namespace ve {
namespace webplatform {

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateLayoutNodeGeometry(
    const LayoutNode &layout_node, const GeometryConstraints &constrains) {
  if (auto *box = dynamic_cast<const LayoutBox *>(&layout_node)) {
    return CalculateLayoutBoxGeometry(*box, constrains);
  } else if (auto *img = dynamic_cast<const LayoutImage *>(&layout_node)) {
    return CalculateLayoutImageGeometry(*img, constrains);
  } else if (auto *text = dynamic_cast<const LayoutText *>(&layout_node)) {
    return CalculateLayoutTextGeometry(*text, constrains);
  }
  return nullptr;
}

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateLayoutBoxGeometry(
    const LayoutBox &layout_root, const GeometryConstraints &constrains) {
  if (!layout_root.GetStyle()) {
    std::cout << "layout_root style undefined" << std::endl;
  }

  const Style &layout_root_style = *layout_root.GetStyle();
  const Margin &layout_root_margin = layout_root_style.GetMargin();
  const Padding &layout_root_padding = layout_root_style.GetPadding();
  const Colour &layout_root_colour = layout_root_style.GetColour();

  // потом ибавиться от проблемы трансляции типов Overflow
  BoxPhysicalFragment::Overflow layout_root_overflow =
      layout_root_style.overflow_ == Style::Overflow::VISIBLE
          ? BoxPhysicalFragment::Overflow::VISIBLE
          : BoxPhysicalFragment::Overflow::HIDDEN;

  LayoutContext layout_context = LayoutContext(layout_root_style);
  auto fragment = std::make_unique<BoxPhysicalFragment>(
      0, 0, layout_root_style.Height(), layout_root_style.Width(),
      layout_root_style.border_width, layout_root_overflow,
      layout_root_colour.red_, layout_root_colour.green_,
      layout_root_colour.blue_);

  // логика автоширины
  if (layout_root_style.width_mode_ == Style::WidthMode::FIXED) {
    fragment->width_ = layout_root_style.Width();
  } else {
    fragment->width_ = constrains.max_width - layout_root_margin.margin_left -
                       layout_root_margin.margin_right;
  }
  float padding_left = layout_root_padding.paddig_left;
  float padding_right = layout_root_padding.paddig_right;

  const GeometryConstraints new_constrains{
      .max_width = fragment->width_ - padding_left - padding_right -
                   2 * layout_root_style.border_width};

  float childs_bottom =
      layout_root_padding.paddig_top + layout_root_style.border_width;
  for (auto &child_element : layout_root.Children()) {
    auto child_fragment =
        CalculateLayoutNodeGeometry(*child_element, new_constrains);
    if (child_fragment) {
      const Style *child_style = nullptr;
      if (auto *child_node =
              dynamic_cast<const LayoutBox *>(child_element.get())) {
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
  }

  // логика авто высоты
  if (layout_root_style.height_mode_ == Style::HeightMode::FIXED) {
    fragment->height_ = layout_root_style.Height();
  } else {
    fragment->height_ = childs_bottom + layout_root_padding.paddig_bottom +
                        layout_root_style.border_width;
  }
  return fragment;
}

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateLayoutTextGeometry(
    const LayoutText &layout_text, const GeometryConstraints &constrains) {

  constexpr float line_height = 16.0f;
  constexpr float baseline = 12.0f;
  constexpr float glyph_advance = 8.0f;
  const float line_width = layout_text.data_.size() * glyph_advance;

  auto fragment = std::make_unique<TextPhysicalFragment>(
      0, 0, line_height, line_width, baseline, nullptr);

  fragment->text_lines_.push_back(
      TextLineFragment(0, 0, line_height, line_width, layout_text.data_));

  return fragment;
}
std::unique_ptr<PhysicalFragment>
GeometryEngine::CalculateDocumentGeometry(const LayoutNode &root_box) {
  Colour colour(Colour::ColourName::WHITE);
  std::unique_ptr<Style> html_style_ptr = std::make_unique<Style>();
  html_style_ptr->colour_ = colour;
  html_style_ptr->height_mode_ = Style::HeightMode::AUTO;
  html_style_ptr->width_mode_ = Style::WidthMode::AUTO;
  LayoutNode *root_box_unconst_ptr = const_cast<LayoutNode *>(&root_box);
  if (auto root_node = dynamic_cast<LayoutBox *>(root_box_unconst_ptr)) {
    root_node->style_ = html_style_ptr.get();
  }
  const GeometryConstraints constrains{.max_width = viewport_width};
  return CalculateLayoutNodeGeometry(root_box, constrains);
}

std::unique_ptr<PhysicalFragment> GeometryEngine::CalculateLayoutImageGeometry(
    const LayoutImage &img, const GeometryConstraints &constrains) {

  return std::make_unique<ImagePhysicalFragment>(0, 0, img.height_, img.width_,
                                                 img.path_);
}

} // namespace webplatform
} // namespace ve
