#include "rendering/layoutengine/layoutengine.h"
#include "document/imageelement.h"
#include "rendering/layoutengine/layoutbox.h"
#include "rendering/layoutengine/layoutimage.h"
#include "rendering/layoutengine/layouttext.h"

namespace ve::webplatform {

std::unique_ptr<LayoutNode> LayoutEngine::LayoutDOM(const DomNode &dom) {
  return LayoutDOMNode(dom);
}

std::unique_ptr<LayoutNode>
LayoutEngine::LayoutDOMNode(const DomNode &dom_node) {
  if (const auto *text_node = dynamic_cast<const TextElement *>(&dom_node)) {
    return LayoutTextElement(*text_node);
  } else if (const auto *img_node =
                 dynamic_cast<const ImageElement *>(&dom_node)) {
    return LayoutImageElement(*img_node);
  } else if (const auto *element_node =
                 dynamic_cast<const ElementNode *>(&dom_node)) {
    return LayoutElement(*element_node);
  }
  return nullptr;
}

std::unique_ptr<LayoutNode>
LayoutEngine::LayoutElement(const ElementNode &element_node) {
  std::unique_ptr<LayoutNode> layout_box =
      std::make_unique<LayoutBox>(element_node.GetStyle());
  for (const auto &elem_child : element_node.childs_) {
    std::unique_ptr<LayoutNode> child_node = LayoutDOMNode(*elem_child);
    if (child_node) {
      layout_box->AddChild(std::move(child_node));
    }
  }
  return layout_box;
}

std::unique_ptr<LayoutNode>
LayoutEngine::LayoutTextElement(const TextElement &text_node) {
  std::unique_ptr<LayoutNode> layout_text =
      std::make_unique<LayoutText>(text_node.data);
  return layout_text;
}

std::unique_ptr<LayoutNode>
LayoutEngine::LayoutImageElement(const ImageElement &img_node) {
  std::unique_ptr<LayoutNode> img = std::make_unique<LayoutImage>(
      img_node.widht_, img_node.height_, img_node.path_to_img_);
  return img;
}

} // namespace ve::webplatform
