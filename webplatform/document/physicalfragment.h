#pragma once

#include <memory>
#include <vector>

namespace ve {
namespace webplatform {

class Div;
class TextElement;

// Координаты каждого дочернего элемента относительно родителя
class PhysicalFragment {
public:
  PhysicalFragment(float x, float y, float height, float width,
                   const Div *owner);
  PhysicalFragment(float x, float y, float height, float width);
  virtual ~PhysicalFragment() = default;
  void AddChild(std::unique_ptr<PhysicalFragment> child_fragment);

  float x_ = 0.0f;
  float y_ = 0.0f;
  float height_ = 0.0f;
  float width_ = 0.0f;

  const Div *owner_ = nullptr;

  std::vector<std::unique_ptr<PhysicalFragment>> child_fragments_;

  std::string ToString() const;
};

class GlyphPhysicalFragment : public PhysicalFragment {
public:
  GlyphPhysicalFragment(float x, float y, float height, float width,
                        int glyph_id);
};

class TextPhysicalFragment : public PhysicalFragment {
public:
  TextPhysicalFragment(float x, float y, float height, float width,
                       float baseline, const TextElement *owner);

  // offset from begining of fragment y
  // формируется из харрактеристик шрифта
  float baseline_ = 0.0F;
  const TextElement *owner_ = nullptr;

private:
};

} // namespace webplatform
} // namespace ve
