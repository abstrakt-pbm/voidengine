#include "div.h"
#include "document/containernode.h"

namespace ve {
namespace webplatform {
Div::Div(std::unique_ptr<Style> style) : ContainerNode(std::move(style)) {}

} // namespace webplatform
} // namespace ve
