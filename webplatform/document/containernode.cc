#include "document/containernode.h"

namespace ve {
namespace webplatform {

ContainerNode::ContainerNode(std::unique_ptr<Style> style)
    : DomNode(std::move(style)) {}

void ContainerNode::AddChild(std::unique_ptr<DomNode> child_node) {
  childs_.push_back(std::move(child_node));
}
} // namespace webplatform
} // namespace ve
