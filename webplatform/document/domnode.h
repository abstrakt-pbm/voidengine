#pragma once

#include "document/style.h"

#include <memory>

namespace ve {
namespace webplatform {

class DomNode {
public:
  DomNode() = default;
  DomNode(std::unique_ptr<Style> style);
  virtual ~DomNode() = default;

  DomNode(const DomNode &) = delete;
  DomNode &operator=(const DomNode &) = delete;

  DomNode(DomNode &&) noexcept = default;
  DomNode &operator=(DomNode &&) noexcept = default;

  void SetStyle(std::unique_ptr<Style> style);
  const Style *GetStyle() const;
  // Не у всех нод есть стиль
  std::unique_ptr<Style> style_;
};
} // namespace webplatform
} // namespace ve
