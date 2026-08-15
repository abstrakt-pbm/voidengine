#include "imageelement.h"

namespace ve {
namespace webplatform {

ImageElement::ImageElement(float widht, float height, std::string path_to_img)
    : path_to_img_(path_to_img), widht_(widht), height_(height) {}
} // namespace webplatform
} // namespace ve
