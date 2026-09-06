#pragma once

#include "document/containernode.h"
#include "document/style.h"

#include <memory>

namespace ve::webplatform {
class ElementNode : public ContainerNode {
public:
  ElementNode(std::unique_ptr<Style> style);

  void SetStyle(std::unique_ptr<Style> style);
  const Style *GetStyle() const;

private:
  std::unique_ptr<Style> style_;
};
} // namespace ve::webplatform
