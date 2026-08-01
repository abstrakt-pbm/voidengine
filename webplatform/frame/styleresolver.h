#pragma once

#include "document.h"

namespace ve {
namespace webplatform {
class LayoutTree;

class StyleResolver {
public:
  void ResolveStyles(const Document &doc);

private:
};
} // namespace webplatform
} // namespace ve
