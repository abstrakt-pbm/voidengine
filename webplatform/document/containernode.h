#pragma once
#include "document/domnode.h"

#include <memory>
#include <vector>

namespace ve::webplatform {
class ContainerNode : public DomNode {
public:
  void AddChild(std::unique_ptr<DomNode> child_element);
  std::vector<std::unique_ptr<DomNode>> childs_;
};
} // namespace ve::webplatform
