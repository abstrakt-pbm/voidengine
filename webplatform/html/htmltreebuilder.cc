#include "htmltreebuilder.h"

#include "document/div.h"
#include "document/domnode.h"
#include "document/imageelement.h"
#include "document/style.h"
#include "document/textelement.h"
#include "htmltoken.h"

#include <iostream>
#include <memory>

namespace ve {
namespace html {

namespace {

webplatform::Style CreateDefaultDivStyle() {
  webplatform::Style style(0.0f, 0.0f, webplatform::Style::Colour::RED);

  style.width_mode_ = webplatform::Style::WidthMode::AUTO;

  style.height_mode_ = webplatform::Style::HeightMode::AUTO;

  style.overflow_ = webplatform::Style::Overflow::VISIBLE;

  return style;
}

} // namespace

HTMLTreeBuilder::HTMLTreeBuilder()
    : tree_root_(std::make_unique<webplatform::DomNode>()) {
  //
  // Синтетический root всегда находится внизу
  // stack of open elements.
  //
  open_elements_.push(tree_root_.get());
}

void HTMLTreeBuilder::ProcessToken(const HTMLToken &html_token) {
  //
  // StartTag.
  //
  if (html_token.Type() == HTMLToken::TokenType::kStartTag) {
    std::unique_ptr<webplatform::DomNode> new_element;
    //
    // Пока поддерживаем только <div>.
    //
    if (html_token.Tag() == HTMLTag::kDiv) {
      new_element = std::make_unique<webplatform::Div>(CreateDefaultDivStyle());
    } else if (html_token.Tag() == HTMLTag::kImg) {
      // проще перейти с вектора атрибутов на мапу
      std::string img_source = html_token.GetAttributeValue("src");
      std::string width_str = html_token.GetAttributeValue("width");
      std::string height_str = html_token.GetAttributeValue("height");
      new_element = std::make_unique<webplatform::ImageElement>(
          std::stoi(width_str), std::stoi(height_str), img_source);

      // image has not closing tag
      open_elements_.top()->AddChild(std::move(new_element));
      return;
    }
    //
    // Unsupported element.
    //
    if (!new_element) {
      return;
    }
    //
    // Сохраняем non-owning pointer до передачи
    // ownership в DOM tree.
    //
    webplatform::DomNode *new_element_ptr = new_element.get();
    //
    // Текущий открытый элемент становится parent.
    //
    open_elements_.top()->AddChild(std::move(new_element));
    //
    // Новый element становится текущим открытым.
    //
    open_elements_.push(new_element_ptr);
    return;
  }

  //
  // Character.
  //
  if (html_token.Type() == HTMLToken::TokenType::kCharacter) {
    //
    // Не создаём TextElement непосредственно
    // под synthetic root.
    //
    // Например '\n' до первого <div> или после </div>.
    //

    if (open_elements_.size() <= 1) {
      return;
    }

    auto new_text_element = std::make_unique<webplatform::TextElement>();

    new_text_element->data = html_token.text_;

    open_elements_.top()->AddChild(std::move(new_text_element));

    return;
  }

  //
  // EndTag.
  //
  if (html_token.Type() == HTMLToken::TokenType::kEndTag) {
    //
    // Пока предполагаем гарантированно корректный HTML:
    //
    // <div>
    //   <div></div>
    // </div>
    //
    // Синтетический root никогда не снимаем со стека.
    //
    if (open_elements_.size() > 1) {
      open_elements_.pop();
    }

    return;
  }
}

std::unique_ptr<webplatform::DomNode> HTMLTreeBuilder::TakeTree() {
  if (!tree_root_) {
    return nullptr;
  }

  //
  // Ничего не было построено.
  //
  if (tree_root_->childs_.empty()) {
    return nullptr;
  }

  //
  // Пока VoidEngine ожидает один настоящий root element.
  //
  // Синтетический DomNode наружу не отдаём.
  //
  return std::move(tree_root_->childs_.front());
}

} // namespace html
} // namespace ve
