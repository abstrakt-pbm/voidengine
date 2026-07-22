#pragma once

#include <span>
#include <string_view>

#include "document.h"
#include "htmltokenizer.h"

namespace ve {
namespace webplatform {

class TextDecoder {
public:
  std::string Decode(std::span<const std::byte> bytes);

private:
};

class DocumentParcer {
public:
  Document ParseDocument(std::string_view html_string);

private:
  HTMLTokeniser tokiniser_;
};

} // namespace webplatform
} // namespace ve
