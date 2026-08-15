#pragma once
#include "domnode.h"

namespace ve {
namespace webplatform {
class ImageElement : public DomNode {
public:
  ImageElement(float widht, float height, std::string path_to_img);

  std::string path_to_img_;
  float widht_;
  float height_;
};
} // namespace webplatform
} // namespace ve
