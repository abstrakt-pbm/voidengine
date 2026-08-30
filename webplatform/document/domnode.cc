#include "domnode.h"

namespace ve {
namespace webplatform {

DomNode::DomNode(std::unique_ptr<Style> style) : style_(std::move(style)) {}

void DomNode::SetStyle(std::unique_ptr<Style> style) {
  style_ = std::move(style);
}

const Style *DomNode::GetStyle() const { return style_.get(); }

} // namespace webplatform
} // namespace ve
