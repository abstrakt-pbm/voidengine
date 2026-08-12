#include "domnode.h"

namespace ve {
namespace webplatform {
void DomNode::AddChild(std::unique_ptr<DomNode> child_node) {
  childs_.push_back(std::move(child_node));
}
} // namespace webplatform
} // namespace ve
