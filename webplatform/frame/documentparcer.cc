#include "documentparcer.h"

#include "frame/document.h"
#include "htmltoken.h"

#include <iostream>
#include <string>

namespace {

bool IsContinuationByte(unsigned char byte) {
  return byte >= 0x80 && byte <= 0xBF;
}

bool IsValidUtf8(std::span<const std::byte> input) {
  const auto *bytes = reinterpret_cast<const unsigned char *>(input.data());

  std::size_t i = 0;

  while (i < input.size()) {
    const unsigned char first = bytes[i];

    if (first <= 0x7F) {
      ++i;
      continue;
    }

    // U+0080..U+07FF
    if (first >= 0xC2 && first <= 0xDF) {
      if (i + 1 >= input.size() || !IsContinuationByte(bytes[i + 1])) {
        return false;
      }

      i += 2;
      continue;
    }

    // U+0800..U+0FFF, excluding overlong encodings.
    if (first == 0xE0) {
      if (i + 2 >= input.size() || bytes[i + 1] < 0xA0 || bytes[i + 1] > 0xBF ||
          !IsContinuationByte(bytes[i + 2])) {
        return false;
      }

      i += 3;
      continue;
    }

    // Most three-byte sequences.
    if ((first >= 0xE1 && first <= 0xEC) || (first >= 0xEE && first <= 0xEF)) {
      if (i + 2 >= input.size() || !IsContinuationByte(bytes[i + 1]) ||
          !IsContinuationByte(bytes[i + 2])) {
        return false;
      }

      i += 3;
      continue;
    }

    // Exclude UTF-16 surrogate range U+D800..U+DFFF.
    if (first == 0xED) {
      if (i + 2 >= input.size() || bytes[i + 1] < 0x80 || bytes[i + 1] > 0x9F ||
          !IsContinuationByte(bytes[i + 2])) {
        return false;
      }

      i += 3;
      continue;
    }

    // U+10000..U+3FFFF, excluding overlong encodings.
    if (first == 0xF0) {
      if (i + 3 >= input.size() || bytes[i + 1] < 0x90 || bytes[i + 1] > 0xBF ||
          !IsContinuationByte(bytes[i + 2]) ||
          !IsContinuationByte(bytes[i + 3])) {
        return false;
      }

      i += 4;
      continue;
    }

    // U+40000..U+FFFFF
    if (first >= 0xF1 && first <= 0xF3) {
      if (i + 3 >= input.size() || !IsContinuationByte(bytes[i + 1]) ||
          !IsContinuationByte(bytes[i + 2]) ||
          !IsContinuationByte(bytes[i + 3])) {
        return false;
      }

      i += 4;
      continue;
    }

    // U+100000..U+10FFFF
    if (first == 0xF4) {
      if (i + 3 >= input.size() || bytes[i + 1] < 0x80 || bytes[i + 1] > 0x8F ||
          !IsContinuationByte(bytes[i + 2]) ||
          !IsContinuationByte(bytes[i + 3])) {
        return false;
      }

      i += 4;
      continue;
    }

    return false;
  }

  return true;
}

bool HasUtf8Bom(std::span<const std::byte> input) {
  return input.size() >= 3 && input[0] == std::byte{0xEF} &&
         input[1] == std::byte{0xBB} && input[2] == std::byte{0xBF};
}

} // namespace

namespace ve {
namespace webplatform {

std::string TextDecoder::Decode(std::span<const std::byte> bytes) {
  // Currently supported only utf-8 text
  if (HasUtf8Bom(bytes)) {
    bytes = bytes.subspan(3);
  }

  if (!IsValidUtf8(bytes)) {
    return "";
  }

  return std::string{reinterpret_cast<const char *>(bytes.data()),
                     bytes.size()};
}

Document DocumentParcer::ParseDocument(std::string_view html_string) {
  // Тут нужно сделать HTMLTokinizer который работает как стейт машина для
  auto tokens = tokiniser_.Tokenize(html_string);
  for (auto token : tokens) {
    std::cout << token.ToString() << std::endl;
  }
  return Document();
}

} // namespace webplatform

} // namespace ve
