#include "physicalfragment.h"

#include <sstream>

namespace {

void AppendFragment(const ve::webplatform::PhysicalFragment &fragment,
                    std::ostringstream &stream, size_t depth) {
  const std::string indent(depth * 2, ' ');

  stream << indent << "PhysicalFragment("
         << "x=" << fragment.x_ << ", y=" << fragment.y_
         << ", width=" << fragment.width_ << ", height=" << fragment.height_
         << ")";

  if (fragment.child_fragments_.empty()) {
    stream << '\n';
    return;
  }

  stream << " {\n";

  for (const auto &child : fragment.child_fragments_) {
    AppendFragment(*child, stream, depth + 1);
  }

  stream << indent << "}\n";
}

} // namespace
namespace ve {
namespace webplatform {

PhysicalFragment::PhysicalFragment(float x, float y, float height, float width)
    : x_(x), y_(y), height_(height), width_(width) {}

void PhysicalFragment::AddChild(
    std::unique_ptr<PhysicalFragment> child_fragment) {
  child_fragments_.push_back(std::move(child_fragment));
}

std::string PhysicalFragment::ToString() const {
  std::ostringstream stream;
  AppendFragment(*this, stream, 0);
  return stream.str();
}

BoxPhysicalFragment::BoxPhysicalFragment(float x, float y, float height,
                                         float width, const Div *owner)
    : PhysicalFragment(x, y, height, width), owner_(owner) {}

TextPhysicalFragment::TextPhysicalFragment(float x, float y, float height,
                                           float width, float baseline,
                                           const TextElement *owner)
    : PhysicalFragment(x, y, height, width), baseline_(baseline),
      owner_(owner) {}

TextLineFragment::TextLineFragment(float x, float y, float height, float width,
                                   std::string payload)
    : PhysicalFragment(x, y, height, width), payload_(payload) {}

ImagePhysicalFragment::ImagePhysicalFragment(float x, float y, float height,
                                             float width,
                                             std::string path_to_img)
    : PhysicalFragment(x, y, height, width), path_to_img_(path_to_img) {}

} // namespace webplatform
} // namespace ve
