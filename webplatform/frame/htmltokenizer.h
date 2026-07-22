#pragma once

#include <optional>
#include <string_view>
#include <vector>

namespace ve {
namespace webplatform {

class HTMLToken;

class HTMLTokeniser {
public:
  enum class TokeniserState {
    kData, // Interpret text as raw text
    kTagOpen,
    kEndTagOpen,
    kTagName,
    kMarkupDeclOpen,
    kProcessingInstructionOpenState,
    kBeforeAttributeName,
    kAttributeName,
    kAfterAttributeName,
    kSelfClosingStartTag,
    kBeforeAttributeValue,
    kAttributeValueUnQuoted,
    kAttributeValueSingleQuoted,
    kAttributeValueDoubleQuoted,
    kAfterAttributeValueQuoted,
    kBogusComment,
    kEOF
  };
  HTMLTokeniser();
  std::vector<HTMLToken> Tokenize(std::string_view html);

private:
  TokeniserState current_state_;
  size_t current_text_position_;

  TokeniserState ExecuteDataState(std::optional<char> character,
                                  HTMLToken &token);
  TokeniserState ExecuteTagOpenState(std::optional<char> character,
                                     HTMLToken &token);
  TokeniserState ExecuteTagNameState(std::optional<char> character,
                                     HTMLToken &token);
  TokeniserState ExecuteBeforeAttributeNameState(std::optional<char> character,
                                                 HTMLToken &token);
  TokeniserState ExecuteSelfClosingStartTagState(std::optional<char> character,
                                                 HTMLToken &token);
  TokeniserState ExecuteAttributeNameState(std::optional<char> character,
                                           HTMLToken &token);
  TokeniserState ExecuteAfterAttributeNameState(std::optional<char> character,
                                                HTMLToken &token);
  TokeniserState ExecuteBeforeAttributeValueState(std::optional<char> character,
                                                  HTMLToken &token);
  TokeniserState
  ExecuteAttributeValueUnQuotedState(std::optional<char> character,
                                     HTMLToken &token);
  TokeniserState
  ExecuteAttributeValueSingleQuotedState(std::optional<char> character,
                                         HTMLToken &token);
  TokeniserState
  ExecuteAttributeValueDoubleQuotedState(std::optional<char> character,
                                         HTMLToken &token);
  TokeniserState
  ExecuteAfterAttributeValueQuotedState(std::optional<char> character,
                                        HTMLToken &token);
  TokeniserState ExecuteEndTagOpenState(std::optional<char> character,
                                        HTMLToken &token);
};

} // namespace webplatform
} // namespace ve
