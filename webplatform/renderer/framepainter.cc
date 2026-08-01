#include "framepainter.h"
#include "renderer/displaylist.h"

namespace ve {
namespace webplatform {

void FramePainter::Paint(const GeometryTreeNode &geometry_tree_node) {
  DisplayList display_list;
  PaintInternal(geometry_tree_node, display_list);
  frame_painter_delegate_->OnPainted(std::move(display_list));
}

void FramePainter::PaintInternal(const GeometryTreeNode &geometry_tree_node,
                                 DisplayList &display_list) {
  display_list.AddCommand(PaintNode(geometry_tree_node));

  for (const auto &child : geometry_tree_node.GetChildren()) {
    PaintInternal(child, display_list);
  }
}

DrawCommand
FramePainter::PaintNode(const GeometryTreeNode &geometry_tree_node) {
  //
}

} // namespace webplatform
} // namespace ve
