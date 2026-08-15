#include "htmltoken.h"

namespace {
std::string EscapeForDebug(std::string_view value) {
  std::string result;
  result.reserve(value.size());

  for (char character : value) {
    switch (character) {
    case '\n':
      result += "\\n";
      break;

    case '\r':
      result += "\\r";
      break;

    case '\t':
      result += "\\t";
      break;

    case '\0':
      result += "\\0";
      break;

    case '\\':
      result += "\\\\";
      break;

    case '"':
      result += "\\\"";
      break;

    default:
      result.push_back(character);
      break;
    }
  }

  return result;
}

} // namespace

namespace ve {
namespace html {

[[nodiscard]] constexpr HTMLTag
HTMLTagFromString(std::string_view name) noexcept {
  for (const HTMLTagEntry &entry : kHTMLTagEntries) {
    if (entry.name == name) {
      return entry.tag;
    }
  }

  return HTMLTag::kUnknown;
}

void HTMLToken::SetType(HTMLToken::TokenType type) { type_ = type; }

void HTMLToken::SetFinished(bool is_finished) {
  is_finished_ = is_finished;
  if (is_finished == true) {
    tag_ = HTMLTagFromString(name_);
  }
}
void HTMLToken::SetIsSelfClosing(bool is_self_closing) {
  is_self_closing_ = is_self_closing;
}

void HTMLToken::AppendCharacter(char character) { text_ += character; }

void HTMLToken::AppendToTagName(char character) { name_ += character; }
void HTMLToken::AppendToTagName(std::string_view character) {
  name_ += character;
}

void HTMLToken::AppendToAttributeName(char character) {
  if (!attributes_.empty()) {
    attributes_.back().name.push_back(character);
  }
}
void HTMLToken::AppendToAttributeName(std::string_view character) {
  if (!attributes_.empty()) {
    attributes_.back().name += character;
  }
}

void HTMLToken::AppendToAttributeValue(char character) {
  if (!attributes_.empty()) {
    attributes_.back().value += character;
  }
}
void HTMLToken::AppendToAttributeValue(std::string_view character) {
  if (!attributes_.empty()) {
    attributes_.back().value += character;
  }
}

void HTMLToken::BeginAttribute() { attributes_.emplace_back(); }

HTMLToken::TokenType HTMLToken::Type() const { return type_; };

bool HTMLToken::IsUninitialized() const {
  return type_ == TokenType::kUninitialized;
}
bool HTMLToken::IsFinished() const { return is_finished_; }

std::string HTMLAttribute::ToString() const {
  return name + "=\"" + EscapeForDebug(value) + "\"";
}

std::string HTMLToken::ToString() const {
  switch (type_) {
  case TokenType::kUninitialized:
    return "Uninitialized";

  case TokenType::kDOCTYPE:
    return "DOCTYPE(name=\"" + EscapeForDebug(name_) + "\")";

  case TokenType::kStartTag: {
    std::string result = "StartTag(name=\"" + EscapeForDebug(name_) + "\"";

    if (!attributes_.empty()) {
      result += ", attributes=[";

      for (std::size_t index = 0; index < attributes_.size(); ++index) {
        if (index != 0) {
          result += ", ";
        }

        result += attributes_[index].ToString();
      }

      result += "]";
    }

    result += ", self_closing=";
    result += is_self_closing_ ? "true" : "false";
    result += ")";

    return result;
  }

  case TokenType::kEndTag:
    return "EndTag(name=\"" + EscapeForDebug(name_) + "\")";

  case TokenType::kComment:
    return "Comment(data=\"" + EscapeForDebug(text_) + "\")";

  case TokenType::kCharacter:
    return "Character(data=\"" + EscapeForDebug(text_) + "\")";

  case TokenType::kEndOfFile:
    return "EOF";

  case TokenType::kProcessingInstruction:
    return "ProcessingInstruction(target=\"" + EscapeForDebug(name_) +
           "\", data=\"" + EscapeForDebug(text_) + "\")";
  }

  return "Unknown";
}

HTMLTag HTMLToken::Tag() const { return tag_; }
} // namespace html
} // namespace ve
