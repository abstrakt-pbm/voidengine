#pragma once
#include "rendering/layoutengine/layoutnode.h"
#include <string>

namespace ve::webplatform {
class LayoutImage : public LayoutNode {
public:
  LayoutImage(float width, float height, std::string path)
      : width_(width), height_(height), path_(std::move(path)) {}

  float width_;
  float height_;
  std::string path_;
};
} // namespace ve::webplatform
