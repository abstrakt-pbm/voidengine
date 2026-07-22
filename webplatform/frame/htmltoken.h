#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace ve {
namespace webplatform {

struct HTMLAttribute {
  std::string name;
  std::string value;

  std::string ToString() const;
};

class HTMLToken {
public:
  enum class TokenType {
    kUninitialized,
    kDOCTYPE,
    kStartTag,
    kEndTag,
    kComment,
    kCharacter,
    kEndOfFile,
    kProcessingInstruction
  };

  void SetType(TokenType type);
  void SetFinished(bool is_finished);
  void SetIsSelfClosing(bool is_self_closing);

  void AppendCharacter(char character);

  void AppendToTagName(char character);
  void AppendToTagName(std::string_view character);

  void AppendToAttributeName(char character);
  void AppendToAttributeName(std::string_view character);

  void AppendToAttributeValue(char character);
  void AppendToAttributeValue(std::string_view character);

  void BeginAttribute();

  TokenType Type() const;

  bool IsUninitialized() const;
  bool IsFinished() const;

  std::string ToString() const;

private:
  std::string name_;
  std::string text_;

  std::vector<HTMLAttribute> attributes_;

  TokenType type_ = TokenType::kUninitialized;
  bool is_finished_ = false;
  bool is_self_closing_ = false;
};

} // namespace webplatform
} // namespace ve
