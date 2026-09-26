#include "rendering/layoutcontext.h"
#include <algorithm>
#include <memory>

namespace ve::webplatform {

LayoutContext::LayoutContext(const Style style) {
  if (style.display_ == Style::Display::BLOCK) {
    layout_algo_ = std::make_unique<BlockLayoutAlgo>(style);
  }
}

void LayoutContext::LayoutChild(const Style *child_style,
                                PhysicalFragment &child_fragment) {
  if (layout_algo_) {
    layout_algo_->LayoutChild(child_style, child_fragment);
  }
}

float LayoutContext::OccupiedBlockSize() const {

  if (layout_algo_) {
    return layout_algo_->OccupiedBlockSize();
  }
  return 0.f;
}

void BlockLayoutAlgo::LayoutChild(const Style *child_style,
                                  PhysicalFragment &child_fragment) {

  float margin_left = 0.0f;
  float margin_top = 0.0f;
  float margin_bottom = 0.0f;

  if (child_style) {
    const Margin &child_margin = child_style->GetMargin();
    margin_left = child_margin.margin_left;
    margin_top = child_margin.margin_top;
    margin_bottom = child_margin.margin_bottom;
  }

  const float collapsed_margin =
      std::max({0.f, margin_top, prev_child_margin_bot_}) +
      std::min({0.f, margin_top, prev_child_margin_bot_});

  child_fragment.x_ = cursor_x_ + margin_left;
  child_fragment.y_ = cursor_y_ - prev_child_margin_bot_ + collapsed_margin;

  prev_child_margin_bot_ = margin_bottom;
  cursor_y_ = child_fragment.y_ + child_fragment.height_ + margin_bottom;
}

BlockLayoutAlgo::BlockLayoutAlgo(const Style &style) {
  const Padding &padding = style.GetPadding();
  cursor_x_ = padding.paddig_left + style.border_width;
  cursor_y_ = padding.paddig_top + style.border_width;
  content_box_top_ = padding.paddig_top + style.border_width;
}

float BlockLayoutAlgo::OccupiedBlockSize() const {
  return cursor_y_ - content_box_top_;
}

} // namespace ve::webplatform
