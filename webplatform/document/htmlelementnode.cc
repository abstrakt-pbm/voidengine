#include "document/htmlelementnode.h"

#include <memory>

namespace ve::webplatform {
HtmlElementNode::HtmlElementNode(std::unique_ptr<Style> style)
    : ElementNode(std::move(style)) {};

} // namespace ve::webplatform
