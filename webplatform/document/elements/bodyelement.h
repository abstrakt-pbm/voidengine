#pragma once

#include "document/elementnode.h"

namespace ve::webplatform {
class BodyElementNode : public ElementNode {
public:
  BodyElementNode(std::unique_ptr<Style> style);
};

} // namespace ve::webplatform
