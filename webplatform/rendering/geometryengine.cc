#include "geometryengine.h"

#include "document/physicalfragment.h"
#include "document/style.h"

#include "rendering/fragmentbuilder.h"
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
    const LayoutBox &layout_box, const GeometryConstraints &constrains) {
  if (!layout_box.GetStyle()) {
    std::cout << "LayoutBox styles is NULL!" << std::endl;
    return nullptr;
  }
  const Style &layout_box_style = *layout_box.GetStyle();

  const GeometryConstraints new_constrains{
      .max_width = constrains.max_width -
                   layout_box_style.GetMargin().margin_left -
                   layout_box_style.GetMargin().margin_right};
  FragmentBuilder fragment_builder(layout_box_style, new_constrains);
  for (const auto &child_element : layout_box.Children()) {
    auto child_fragment =
        CalculateLayoutNodeGeometry(*child_element, new_constrains);
    const Style *child_style = nullptr;
    if (const LayoutBox *child_box =
            dynamic_cast<const LayoutBox *>(child_element.get())) {
      child_style = child_box->GetStyle();
    }
    fragment_builder.AddFragmentChild(child_style, std::move(child_fragment));
  }
  return fragment_builder.Build();
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
