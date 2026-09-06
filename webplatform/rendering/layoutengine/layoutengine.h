#pragma once

#include "document/domnode.h"
#include "document/elementnode.h"
#include "document/imageelement.h"
#include "document/textelement.h"
#include "rendering/layoutengine/layoutnode.h"

namespace ve::webplatform {
class LayoutEngine {
public:
  std::unique_ptr<LayoutNode> LayoutDOM(const DomNode &dom);

private:
  std::unique_ptr<LayoutNode> LayoutDOMNode(const DomNode &dom_node);
  std::unique_ptr<LayoutNode> LayoutElement(const ElementNode &element_node);
  std::unique_ptr<LayoutNode> LayoutTextElement(const TextElement &text_node);
  std::unique_ptr<LayoutNode> LayoutImageElement(const ImageElement &img_node);
};
} // namespace ve::webplatform
