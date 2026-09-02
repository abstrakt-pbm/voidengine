#include "document/containernode.h"

namespace ve {
namespace webplatform {

void ContainerNode::AddChild(std::unique_ptr<DomNode> child_node) {
  childs_.push_back(std::move(child_node));
}
} // namespace webplatform
} // namespace ve
