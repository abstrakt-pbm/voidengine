#pragma once
#include "rendering/layoutengine/layoutnode.h"

#include <string>

namespace ve::webplatform {
class LayoutText : public LayoutNode {
public:
  LayoutText(std::string data);
  std::string data_;
};
} // namespace ve::webplatform
