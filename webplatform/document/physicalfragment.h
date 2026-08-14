#pragma once

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {

class Div;
class TextElement;

// Представить Dom дерево в наиболее удобном для Paint-ера виде
class PhysicalFragment {
public:
  PhysicalFragment(float x, float y, float height, float width);
  PhysicalFragment(const PhysicalFragment &) = delete;
  PhysicalFragment(PhysicalFragment &&) noexcept = default;

  virtual ~PhysicalFragment() = default;

  PhysicalFragment &operator=(const PhysicalFragment &) = delete;
  PhysicalFragment &operator=(PhysicalFragment &&) noexcept = default;

  void AddChild(std::unique_ptr<PhysicalFragment> child_fragment);

  float x_ = 0.0f;
  float y_ = 0.0f;
  float height_ = 0.0f;
  float width_ = 0.0f;

  std::vector<std::unique_ptr<PhysicalFragment>> child_fragments_;

  std::string ToString() const;
};

class TextPhysicalFragment : public PhysicalFragment {
public:
  TextPhysicalFragment(float x, float y, float height, float width,
                       float baseline, const TextElement *owner);

  // offset from begining of fragment y
  // формируется из харрактеристик шрифта
  float baseline_ = 0.0F;
  const TextElement *owner_ = nullptr;
};

class BoxPhysicalFragment : public PhysicalFragment {
public:
  BoxPhysicalFragment(float x, float y, float height, float width,
                      const Div *owner);
  const Div *owner_ = nullptr;
};

} // namespace webplatform
} // namespace ve
