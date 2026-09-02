#pragma once

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
};
} // namespace webplatform
} // namespace ve
