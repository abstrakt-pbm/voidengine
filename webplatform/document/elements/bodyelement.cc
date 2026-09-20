#include "document/elements/bodyelement.h"
#include "document/elementnode.h"

namespace ve::webplatform {

BodyElementNode::BodyElementNode(std::unique_ptr<Style> style)
    : ElementNode(std::move(style)) {}
} // namespace ve::webplatform
