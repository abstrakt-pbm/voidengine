#pragma once

#include "element.h"

#include "../htmltoken.h"

namespace ve {
namespace webplatform {
class HTMLElement : public Element {
public:
private:
  HTMLTag tag_ = HTMLTag::kUnknown;
};

} // namespace webplatform
} // namespace ve
