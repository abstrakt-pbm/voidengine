#pragma once

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {

class Div;

// Координаты каждого дочернего элемента относительно родителя
class PhysicalFragment {
public:
  PhysicalFragment(float x, float y, float height, float width,
                   const Div *owner);

  void AddChild(std::unique_ptr<PhysicalFragment> child_fragment);

  float x_ = 0.0f;
  float y_ = 0.0f;
  float height_ = 0.0f;
  float width_ = 0.0f;

  const Div *owner_ = nullptr;

  std::vector<std::unique_ptr<PhysicalFragment>> child_fragments_;

  std::string ToString() const;
};
} // namespace webplatform
} // namespace ve
