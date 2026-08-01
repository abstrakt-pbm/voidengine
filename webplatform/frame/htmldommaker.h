#pragma once

#include "dom/htmlelement.h"
#include "frame/htmltoken.h"

namespace ve {
namespace webplatform {

class HTMLStackElement {
public:
  HTMLStackElement(HTMLElement *elem, HTMLStackElement *stack_elem,
                   HTMLTag tag);

  HTMLElement *html_element_ = nullptr;
  std::unique_ptr<HTMLStackElement> next_element_;
  HTMLTag tag = HTMLTag::kUnknown;
};

class HTMLElementStack {
public:
  void PushElement(std::unique_ptr<HTMLStackElement> stack_element);
  HTMLStackElement *PopElement();

  HTMLStackElement *Head();
  HTMLStackElement *Top();
  bool contains(HTMLTag tag);

private:
  std::unique_ptr<HTMLStackElement> top_;
};

class HTMLDOMMaker {
public:
  void InsertHTMLElement(const HTMLToken &token);

private:
  HTMLElementStack open_elements_;
};
} // namespace webplatform
} // namespace ve
