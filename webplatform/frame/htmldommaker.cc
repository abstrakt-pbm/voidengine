#pragma once

#include "htmldommaker.h"

#include "frame/dom/htmlelement.h"
#include "frame/htmltoken.h"
#include <memory>

namespace ve {
namespace webplatform {

[[nodiscard]] constexpr bool IsVoidHTMLElement(HTMLTag tag) noexcept {
  switch (tag) {
  case HTMLTag::kArea:
  case HTMLTag::kBase:
  case HTMLTag::kBr:
  case HTMLTag::kCol:
  case HTMLTag::kEmbed:
  case HTMLTag::kHr:
  case HTMLTag::kImg:
  case HTMLTag::kInput:
  case HTMLTag::kLink:
  case HTMLTag::kMeta:
  case HTMLTag::kSource:
  case HTMLTag::kTrack:
  case HTMLTag::kWbr:
    return true;
  default:
    return false;
  }
}

std::unique_ptr<HTMLElement> MakeHTMLElement(const HTMLToken &token) {
  return std::make_unique<HTMLElement>();
}

void HTMLDOMMaker::InsertHTMLElement(const HTMLToken &token) {
  if (token.Type() == HTMLToken::TokenType::kStartTag) {
    if (!IsVoidHTMLElement(token.Tag())) {
      auto top_stack_element_ = open_elements_.Top();
      auto current_top_element = top_stack_element_->html_element_;

      auto element = MakeHTMLElement(token);
      auto element_ptr = element.get();
      current_top_element->AppendChild(std::move(element));

      auto stack_elem =
          std::make_unique<HTMLStackElement>(element_ptr, nullptr, token.Tag());
      open_elements_.PushElement(std::move(stack_elem));
    }
  }

  if (token.Type() == HTMLToken::TokenType::kEndTag) {
    if (open_elements_.contains(token.Tag())) {
      while (open_elements_.Top() && open_elements_.Top()->tag != token.Tag()) {
        open_elements_.PopElement();
      }
      if (open_elements_.Top()) {
        open_elements_.PopElement();
      }
    }
  }
}

} // namespace webplatform
} // namespace ve
