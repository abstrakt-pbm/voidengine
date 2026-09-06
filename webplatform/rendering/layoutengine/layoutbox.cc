#include "rendering/layoutengine/layoutbox.h"

namespace ve::webplatform {

LayoutBox::LayoutBox(const Style *style) : style_(style) {}

const Style *LayoutBox::GetStyle() const { return style_; }

} // namespace ve::webplatform
