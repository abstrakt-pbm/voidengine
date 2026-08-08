#include "div.h"

namespace ve {
namespace webplatform {
Div::Div(Style style) : style_(style) {}

void Div::AddChild(std::unique_ptr<Div> child_div) {
  childs_.push_back(std::move(child_div));
}

const Style &Div::GetStyle() const { return style_; }

} // namespace webplatform
} // namespace ve
