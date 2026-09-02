#include "document/htmlelementnode.h"

#include <memory>

namespace ve::webplatform {
HtmlElementNode::HtmlElementNode() : ElementNode(std::make_unique<Style>()) {};

} // namespace ve::webplatform
