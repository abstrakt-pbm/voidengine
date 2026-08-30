#include "document/htmlelementnode.h"

#include "document/containernode.h"

#include <memory>

namespace ve::webplatform {
HtmlElementNode::HtmlElementNode()
    : ContainerNode(std::make_unique<Style>()) {};

} // namespace ve::webplatform
