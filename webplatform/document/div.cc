#include "div.h"

namespace ve {
namespace webplatform {
Div::Div(Style style) : style_(style) {}

void Div::AddChild(std::unique_ptr<DomNode> child_node) {
  childs_.push_back(std::move(child_node));
}

const Style &Div::GetStyle() const { return style_; }

} // namespace webplatform
} // namespace ve
