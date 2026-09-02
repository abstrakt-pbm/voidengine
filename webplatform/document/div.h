#pragma once

#include "document/elementnode.h"
#include "document/style.h"

namespace ve {
namespace webplatform {
class Div : public ElementNode {
public:
  Div(std::unique_ptr<Style> style);
};
} // namespace webplatform
} // namespace ve
