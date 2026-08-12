#pragma once

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {
class DomNode {
public:
  DomNode() = default;
  virtual ~DomNode() = default;

  DomNode(const DomNode &) = delete;
  DomNode &operator=(const DomNode &) = delete;

  DomNode(DomNode &&) noexcept = default;
  DomNode &operator=(DomNode &&) noexcept = default;

  // Проблема в том что у текста нет детей
  void AddChild(std::unique_ptr<DomNode> child_element);
  std::vector<std::unique_ptr<DomNode>> childs_;
};
} // namespace webplatform
} // namespace ve
