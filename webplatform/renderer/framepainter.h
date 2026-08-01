#pragma once

#include "frame/geometrytree.h"

#include <memory>

namespace ve {
namespace webplatform {
// Преобразует результаты LayoutCalculation в команды отрисовки
// Команды отрисовки передаются эмбедеру

class DisplayList;
class FramePainterDelegate {
public:
  void OnPainted(const DisplayList &list);
};

class FramePainter {
public:
  void Paint(const GeometryTreeNode &geometry_tree_node);
  DisplayList PaintInternal(const GeometryTreeNode &geometry_tree_node);
  DisplayList PaintNode(const GeometryTreeNode &geometry_tree_node);

private:
  std::unique_ptr<FramePainterDelegate> frame_painter_delegate_;
};
} // namespace webplatform
} // namespace ve
