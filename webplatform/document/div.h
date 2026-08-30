#pragma once

#include "document/containernode.h"
#include "document/style.h"

namespace ve {
namespace webplatform {
class Div : public ContainerNode {
public:
  Div(std::unique_ptr<Style> style);
};
} // namespace webplatform
} // namespace ve
