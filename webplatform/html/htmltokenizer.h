#pragma once

#include <optional>
#include <string_view>
#include <vector>

namespace ve {
namespace html {
class HTMLToken;

class HTMLTokeniser {
public:
  enum class TokeniserState {
    kData,
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
  TokeniserState ExecuteMarkupDeclOpenState(std::optional<char> character,
                                            HTMLToken &token);
  TokeniserState current_state_ = TokeniserState::kData;
  size_t current_text_position_ = 0;
};
} // namespace html
} // namespace ve
