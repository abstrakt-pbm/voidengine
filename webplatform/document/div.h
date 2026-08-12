#pragma once

#include "document/domnode.h"
#include "document/style.h"

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {
class Div : public DomNode {
public:
  Div(Style style);

  const Style &GetStyle() const;

private:
  Style style_;
};
} // namespace webplatform
} // namespace ve
