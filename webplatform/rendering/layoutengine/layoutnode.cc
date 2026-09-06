#include "rendering/layoutengine/layoutnode.h"

namespace ve::webplatform {
void LayoutNode::AddChild(std::unique_ptr<LayoutNode> layout_node) {
  children_.push_back(std::move(layout_node));
}

const std::vector<std::unique_ptr<LayoutNode>> &LayoutNode::Children() const {
  return children_;
}

} // namespace ve::webplatform
