#pragma once

#include <memory>
#include <vector>

namespace ve::webplatform {
//
// представить в одном дереве только то что реально будет отображенор
// Dom проинтерпретированый с точки зрения Layout
// Каждый layout узел несёт в себе достаточно информации для того чтобы движок
// геометрии без напряга дал ему размер и положение
class LayoutNode {
public:
  virtual ~LayoutNode() = default;

  void AddChild(std::unique_ptr<LayoutNode> layout_node);
  const std::vector<std::unique_ptr<LayoutNode>> &Children() const;

private:
  std::vector<std::unique_ptr<LayoutNode>> children_;
};
} // namespace ve::webplatform
