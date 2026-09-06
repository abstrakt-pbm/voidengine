#include "document/elementnode.h"

namespace ve::webplatform {
ElementNode::ElementNode(std::unique_ptr<Style> style)
    : style_(std::move(style)) {}

void ElementNode::SetStyle(std::unique_ptr<Style> style) {
  style_ = std::move(style);
}

const Style *ElementNode::GetStyle() const { return style_.get(); }
} // namespace ve::webplatform
