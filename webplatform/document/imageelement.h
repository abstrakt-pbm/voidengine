#pragma once
#include "document/elementnode.h"

#include <string>

namespace ve {
namespace webplatform {
class ImageElement : public ElementNode {
public:
  ImageElement(float widht, float height, std::string path_to_img);

  std::string path_to_img_;
  float widht_;
  float height_;
};
} // namespace webplatform
} // namespace ve
