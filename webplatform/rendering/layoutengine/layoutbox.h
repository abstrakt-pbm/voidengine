#pragma once
#include "document/style.h"
#include "rendering/layoutengine/layoutnode.h"
namespace ve::webplatform {
class LayoutBox : public LayoutNode {
public:
  LayoutBox(const Style *style);
  const Style *GetStyle() const;

  // может быть LayoutContext
  const Style *style_;
};
} // namespace ve::webplatform
