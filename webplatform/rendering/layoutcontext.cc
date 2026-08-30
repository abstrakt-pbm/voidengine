#include "rendering/layoutcontext.h"
#include <memory>

namespace ve::webplatform {

LayoutContext::LayoutContext(const Style style) : style_(style) {
  if (style_.display_ == Style::Display::BLOCK) {
    layout_algo_ = std::make_unique<BlockLayoutAlgo>(style);
  }
}

void LayoutContext::LayoutChild(const Style *child_style,
                                PhysicalFragment &child_fragment) {
  if (layout_algo_) {
    layout_algo_->LayoutChild(child_style, child_fragment);
  }
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

  child_fragment.x_ = cursor_x + margin_left;
  child_fragment.y_ = cursor_y + margin_top;

  cursor_y = child_fragment.y_ + child_fragment.height_ + margin_bottom;
}
BlockLayoutAlgo::BlockLayoutAlgo(const Style &style) : style_(style) {
  const Padding &padding = style.GetPadding();
  cursor_x = padding.paddig_left + style.border_width;
  cursor_y = padding.paddig_top + style.border_width;
}

} // namespace ve::webplatform
