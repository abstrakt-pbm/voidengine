#include "div.h"

namespace ve {
namespace webplatform {
Div::Div(Style style) : style_(style) {}

const Style &Div::GetStyle() const { return style_; }

} // namespace webplatform
} // namespace ve
