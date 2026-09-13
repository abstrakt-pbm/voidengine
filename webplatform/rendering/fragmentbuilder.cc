#include "rendering/fragmentbuilder.h"
namespace ve::webplatform {

FragmentBuilder::FragmentBuilder(const Style &style,
                                 const GeometryConstraints &geometry_constrains)
    : style_(style), geometry_constrains_(geometry_constrains),
      layout_context_(style_) {
  const Colour &colour = style_.GetColour();

  // потом ибавиться от проблемы трансляции типов Overflow
  BoxPhysicalFragment::Overflow layout_root_overflow =
      style.overflow_ == Style::Overflow::VISIBLE
          ? BoxPhysicalFragment::Overflow::VISIBLE
          : BoxPhysicalFragment::Overflow::HIDDEN;
  fragment_ = std::move(std::make_unique<BoxPhysicalFragment>(
      0, 0, style_.Height(), style_.Width(), style_.border_width,
      layout_root_overflow, colour.red_, colour.green_, colour.blue_));
}

std::unique_ptr<PhysicalFragment> FragmentBuilder::Build() {
  const Padding &padding = style_.GetPadding();

  if (style_.height_mode_ == Style::HeightMode::FIXED) {
    fragment_->height_ = style_.Height();
  } else {
    auto content_box = ContentBoxSnapshot();
    fragment_->height_ =
        content_box.Height() + padding.paddig_bottom + style_.border_width;
  }
  return std::move(fragment_);
}

void FragmentBuilder::AddFragmentChild(
    const Style *child_style,
    std::unique_ptr<PhysicalFragment> child_fragment) {
  layout_context_.LayoutChild(child_style, *child_fragment.get());
  fragment_->AddChild(std::move(child_fragment));
}

Box FragmentBuilder::ContentBoxSnapshot() {
  float width = 0.f;
  float height = 0.f;

  const Padding &padding = style_.GetPadding();
  const float padding_left = padding.paddig_left;
  const float padding_right = padding.paddig_right;
  const float padding_top = padding.paddig_top;
  const float padding_bottom = padding.paddig_bottom;

  const float border_width = style_.border_width;
  const float border_box_height = style_.Height();

  const float border_box_width = style_.width_mode_ == Style::WidthMode::AUTO
                                     ? geometry_constrains_.max_width
                                     : style_.Width();
  // Ширина
  const float raw_calc_width =
      border_box_width - padding_left - padding_right - 2 * border_width;
  width = std::max(0.f, raw_calc_width);

  // Высотой
  if (style_.height_mode_ == Style::HeightMode::AUTO) {
    height = layout_context_.OccupiedBlockSize();
  } else {
    const float raw_calc_height =
        border_box_height - padding_top - padding_bottom - 2 * border_width;
    height = std::max(0.f, raw_calc_height);
  }

  return Box(width, height);
}

Box::Box(float width, float height) : width_(width), height_(height) {}

float Box::Width() const { return width_; }
float Box::Height() const { return height_; }
} // namespace ve::webplatform
