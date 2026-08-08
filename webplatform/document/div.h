#pragma once

#include "document/style.h"

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {
class Div {
public:
  Div(Style style);

  void AddChild(std::unique_ptr<Div> child_div);
  const Style &GetStyle() const;

  std::vector<std::unique_ptr<Div>> childs_;

private:
  Style style_;
};
} // namespace webplatform
} // namespace ve
