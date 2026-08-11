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

  void AddChild(std::unique_ptr<DomNode> child_element);
  const Style &GetStyle() const;

  std::vector<std::unique_ptr<DomNode>> childs_;

private:
  Style style_;
};
} // namespace webplatform
} // namespace ve
