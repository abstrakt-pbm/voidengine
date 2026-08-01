#pragma once

#include "node.h"

#include <memory>

namespace ve {
namespace webplatform {
class ContainerNode : public Node {
public:
  void AppendChild(std::unique_ptr<Node> container_node);

private:
};
} // namespace webplatform
} // namespace ve
