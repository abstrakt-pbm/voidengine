#include "htmltokenizer.h"
#include "htmltoken.h"
#include <cctype>
#include <optional>

namespace {
static bool IsAsciiAlpha(char character) {
  return (character >= 'A' && character <= 'Z') ||
         (character >= 'a' && character <= 'z');
}
static bool IsHtmlSpace(char character) {
  return character == '\t' || character == '\n' || character == '\f' ||
         character == '\r' || character == ' ';
}

} // namespace

namespace ve {
namespace webplatform {

HTMLTokeniser::HTMLTokeniser()
    : current_state_(TokeniserState::kData), current_text_position_(0) {}

std::vector<HTMLToken> HTMLTokeniser::Tokenize(std::string_view html) {
  std::vector<HTMLToken> tokens;

  current_text_position_ = 0;
  current_state_ = TokeniserState::kData;
  HTMLToken current_html_token;

  while (current_state_ != TokeniserState::kEOF) {
    const std::optional<char> character =
        current_text_position_ < html.size()
            ? std::optional<char>{html[current_text_position_]}
            : std::nullopt;

    switch (current_state_) {
    case TokeniserState::kData:
      current_state_ = ExecuteDataState(character, current_html_token);
      break;

    case TokeniserState::kTagOpen:
      current_state_ = ExecuteTagOpenState(character, current_html_token);
      break;

    case TokeniserState::kEndTagOpen:
      current_state_ = ExecuteEndTagOpenState(character, current_html_token);
      break;

    case TokeniserState::kTagName:
      current_state_ = ExecuteTagNameState(character, current_html_token);
      break;

    case TokeniserState::kBeforeAttributeName:
      current_state_ =
          ExecuteBeforeAttributeNameState(character, current_html_token);
      break;

    case TokeniserState::kAttributeName:
      current_state_ = ExecuteAttributeNameState(character, current_html_token);
      break;

    case TokeniserState::kAfterAttributeName:
      current_state_ =
          ExecuteAfterAttributeNameState(character, current_html_token);
      break;

    case TokeniserState::kBeforeAttributeValue:
      current_state_ =
          ExecuteBeforeAttributeValueState(character, current_html_token);
      break;

    case TokeniserState::kAttributeValueUnQuoted:
      current_state_ =
          ExecuteAttributeValueUnQuotedState(character, current_html_token);
      break;

    case TokeniserState::kAttributeValueSingleQuoted:
      current_state_ =
          ExecuteAttributeValueSingleQuotedState(character, current_html_token);
      break;

    case TokeniserState::kAttributeValueDoubleQuoted:
      current_state_ =
          ExecuteAttributeValueDoubleQuotedState(character, current_html_token);
      break;

    case TokeniserState::kAfterAttributeValueQuoted:
      current_state_ =
          ExecuteAfterAttributeValueQuotedState(character, current_html_token);
      break;

    case TokeniserState::kSelfClosingStartTag:
      current_state_ =
          ExecuteSelfClosingStartTagState(character, current_html_token);
      break;
    case TokeniserState::kMarkupDeclOpen:
      current_state_ =
          ExecuteMarkupDeclOpenState(character, current_html_token);
      break;

    case TokeniserState::kEOF:
      break;

    default:
      std::terminate();
    }
    if (current_html_token.IsFinished()) {
      tokens.push_back(std::move(current_html_token));
      current_html_token = HTMLToken();
    }
  }

  return tokens;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteDataState(std::optional<char> character,
                                HTMLToken &token) {
  if (!character) {
    if (!token.IsUninitialized()) {
      token.SetFinished(true);
      return TokeniserState::kData;
    }
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (*character == '<') {
    ++current_text_position_;
    if (!token.IsUninitialized()) {
      token.SetFinished(true);
    }
    return TokeniserState::kTagOpen;
  }

  if (token.IsUninitialized()) {
    token.SetType(HTMLToken::TokenType::kCharacter);
  }
  token.AppendCharacter(*character);
  ++current_text_position_;
  return TokeniserState::kData;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteTagOpenState(std::optional<char> character,
                                   HTMLToken &token) {
  if (character) {
    if (*character == '/') {
      ++current_text_position_;
      return TokeniserState::kEndTagOpen;
    }

    if (*character == '!') {
      ++current_text_position_;
      return TokeniserState::kMarkupDeclOpen;
    }
    if (*character == '?') {
      ++current_text_position_;
      return TokeniserState::kProcessingInstructionOpenState;
    }

    if (IsAsciiAlpha(*character)) {
      token.SetType(HTMLToken::TokenType::kStartTag);
      return TokeniserState::kTagName;
    }
  }
  token.SetType(HTMLToken::TokenType::kCharacter);
  token.AppendCharacter('<');
  token.SetFinished(true);
  return TokeniserState::kData;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteTagNameState(std::optional<char> character,
                                   HTMLToken &token) {
  if (character) {
    if (IsHtmlSpace(*character)) {
      ++current_text_position_;
      return TokeniserState::kBeforeAttributeName;
    }
    if (*character == '>') {
      ++current_text_position_;
      token.SetFinished(true);
      return TokeniserState::kData;
    }
    if (*character == '/') {
      ++current_text_position_;
      return TokeniserState::kSelfClosingStartTag;
    }
    if (*character >= 'A' && *character <= 'Z') {
      token.AppendToTagName(static_cast<char>(*character + ('a' - 'A')));
      ++current_text_position_;
      return TokeniserState::kTagName;
    }
    if (*character == '\0') {
      token.AppendToTagName("\xEF\xBF\xBD");
      ++current_text_position_;
      return TokeniserState::kTagName;
    }
    token.AppendToTagName(*character);
    ++current_text_position_;
    return TokeniserState::kTagName;
  }
  token = HTMLToken();
  token.SetType(HTMLToken::TokenType::kEndOfFile);
  token.SetFinished(true);
  return TokeniserState::kEOF;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteBeforeAttributeNameState(std::optional<char> character,
                                               HTMLToken &token) {
  if (!character) {
    // Незавершённый tag token отбрасывается.
    token = HTMLToken{};
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (IsHtmlSpace(*character)) {
    ++current_text_position_;
    return TokeniserState::kBeforeAttributeName;
  }

  if (*character == '/') {
    ++current_text_position_;
    return TokeniserState::kSelfClosingStartTag;
  }

  if (*character == '>') {
    ++current_text_position_;
    token.SetFinished(true);
    return TokeniserState::kData;
  }

  if (*character == '=') {
    // Parse error, но HTML всё равно создаёт
    // атрибут с именем "=".
    token.BeginAttribute();
    token.AppendToAttributeName('=');
    ++current_text_position_;
    return TokeniserState::kAttributeName;
  }
  token.BeginAttribute();
  return TokeniserState::kAttributeName;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteSelfClosingStartTagState(std::optional<char> character,
                                               HTMLToken &token) {

  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (*character == '>') {
    ++current_text_position_;

    token.SetIsSelfClosing(true);
    token.SetFinished(true);
    return TokeniserState::kData;
  }

  // Parse error
  return TokeniserState::kBeforeAttributeName;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteAttributeNameState(std::optional<char> character,
                                         HTMLToken &token) {
  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (IsHtmlSpace(*character)) {
    ++current_text_position_;
    return TokeniserState::kAfterAttributeName;
  }

  if (*character == '=') {
    ++current_text_position_;
    return TokeniserState::kBeforeAttributeValue;
  }
  if (*character == '/') {
    ++current_text_position_;
    return TokeniserState::kSelfClosingStartTag;
  }
  if (*character == '>') {
    ++current_text_position_;
    token.SetFinished(true);
    return TokeniserState::kData;
  }
  if (*character >= 'A' && *character <= 'Z') {
    token.AppendToAttributeName(static_cast<char>(*character + ('a' - 'A')));
    ++current_text_position_;
    return TokeniserState::kAttributeName;
  }
  if (*character == '\0') {
    token.AppendToAttributeName("\xEF\xBF\xBD");
    ++current_text_position_;
    return TokeniserState::kAttributeName;
  }

  token.AppendToAttributeName(*character);
  ++current_text_position_;
  return TokeniserState::kAttributeName;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteAfterAttributeNameState(std::optional<char> character,
                                              HTMLToken &token) {
  if (!character) {
    token = HTMLToken{};
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);

    return TokeniserState::kEOF;
  }

  if (IsHtmlSpace(*character)) {
    ++current_text_position_;
    return TokeniserState::kAfterAttributeName;
  }

  if (*character == '/') {
    ++current_text_position_;
    return TokeniserState::kSelfClosingStartTag;
  }

  if (*character == '=') {
    ++current_text_position_;
    return TokeniserState::kBeforeAttributeValue;
  }

  if (*character == '>') {
    ++current_text_position_;
    token.SetFinished(true);

    return TokeniserState::kData;
  }

  // Начинается следующий атрибут.
  token.BeginAttribute();

  // Текущий символ не потребляем:
  // AttributeNameState обработает его повторно.
  return TokeniserState::kAttributeName;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteBeforeAttributeValueState(std::optional<char> character,
                                                HTMLToken &token) {
  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (IsHtmlSpace(*character)) {
    ++current_text_position_;
    return TokeniserState::kBeforeAttributeValue;
  }

  if (*character == '\'') {
    ++current_text_position_;
    return TokeniserState::kAttributeValueSingleQuoted;
  }
  if (*character == '>') {
    ++current_text_position_;
    token.SetFinished(true);
    return TokeniserState::kData;
  }
  if (*character == '"') {
    ++current_text_position_;
    return TokeniserState::kAttributeValueDoubleQuoted;
  }
  return TokeniserState::kAttributeValueUnQuoted;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteAttributeValueUnQuotedState(std::optional<char> character,
                                                  HTMLToken &token) {
  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }
  if (IsHtmlSpace(*character)) {
    ++current_text_position_;
    return TokeniserState::kBeforeAttributeName;
  }
  if (*character == '>') {
    ++current_text_position_;
    token.SetFinished(true);
    return TokeniserState::kData;
  }
  if (*character == '\0') {
    token.AppendToAttributeValue("\xEF\xBF\xBD");
    ++current_text_position_;
    return TokeniserState::kAttributeValueUnQuoted;
  }
  if (*character == '&') {
    // currently unimplemented
  }
  token.AppendToAttributeValue(*character);
  ++current_text_position_;
  return TokeniserState::kAttributeValueUnQuoted;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteAttributeValueSingleQuotedState(
    std::optional<char> character, HTMLToken &token) {
  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (*character == '\'') {
    ++current_text_position_;
    return TokeniserState::kAfterAttributeValueQuoted;
  }
  if (*character == '&') {
    // currently unimplemented
  }
  if (*character == '\0') {
    token.AppendToAttributeValue("\xEF\xBF\xBD");
    ++current_text_position_;
    return TokeniserState::kAttributeValueSingleQuoted;
  }
  token.AppendToAttributeValue(*character);
  ++current_text_position_;
  return TokeniserState::kAttributeValueSingleQuoted;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteAttributeValueDoubleQuotedState(
    std::optional<char> character, HTMLToken &token) {
  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (*character == '\"') {
    ++current_text_position_;
    return TokeniserState::kAfterAttributeValueQuoted;
  }
  if (*character == '&') {
    // currently unimplemented
  }
  if (*character == '\0') {
    token.AppendToAttributeValue("\xEF\xBF\xBD");
    ++current_text_position_;
    return TokeniserState::kAttributeValueDoubleQuoted;
  }
  token.AppendToAttributeValue(*character);
  ++current_text_position_;
  return TokeniserState::kAttributeValueDoubleQuoted;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteAfterAttributeValueQuotedState(
    std::optional<char> character, HTMLToken &token) {
  if (!character) {
    token = HTMLToken();
    token.SetType(HTMLToken::TokenType::kEndOfFile);
    token.SetFinished(true);
    return TokeniserState::kEOF;
  }

  if (IsHtmlSpace(*character)) {
    ++current_text_position_;
    return TokeniserState::kBeforeAttributeName;
  }
  if (*character == '/') {
    ++current_text_position_;
    return TokeniserState::kSelfClosingStartTag;
  }
  if (*character == '>') {
    ++current_text_position_;
    token.SetFinished(true);
    return TokeniserState::kData;
  }

  return TokeniserState::kBeforeAttributeName;
}

HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteEndTagOpenState(std::optional<char> character,
                                      HTMLToken &token) {
  if (!character) {
    token.SetType(HTMLToken::TokenType::kCharacter);
    token.AppendCharacter('<');
    token.AppendCharacter('/');
    token.SetFinished(true);

    return TokeniserState::kData;
  }

  if (IsAsciiAlpha(*character)) {
    token.SetType(HTMLToken::TokenType::kEndTag);
    return TokeniserState::kTagName;
  }

  if (*character == '>') {
    // Parse error: пустой закрывающий тег </>.
    ++current_text_position_;
    return TokeniserState::kData;
  }

  return TokeniserState::kBogusComment;
}

// Нужно переделать источник сырого теста на удобную работу с
HTMLTokeniser::TokeniserState
HTMLTokeniser::ExecuteMarkupDeclOpenState(std::optional<char> character,
                                          HTMLToken &token) {
  if (!character) {
    token.SetType(HTMLToken::TokenType::kCharacter);
    token.AppendCharacter('<');
    token.AppendCharacter('/');
    token.SetFinished(true);

    return TokeniserState::kData;
  }
  if (*character == ' ') {
  }
  return TokeniserState::kData;
}
} // namespace webplatform
} // namespace ve
