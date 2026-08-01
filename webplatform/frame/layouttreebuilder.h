#pragma once

#include "frame/document.h"
#include "frame/styleresolver.h"

#include "layouttree.h"

namespace ve {
namespace webplatform {
class LayoutTreeBuilder {
public:
  LayoutTree MakeLayoutTree(const Document &document);

private:
};
} // namespace webplatform
} // namespace ve
