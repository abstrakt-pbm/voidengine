#include "htmlparser.h"

#include "html/htmltreebuilder.h"
#include "htmltoken.h"
#include "htmltokenizer.h"

#include <iostream>

namespace ve {
namespace html {
std::unique_ptr<webplatform::DomNode> ParseHTML(std::string raw_html) {
  if (raw_html.size() == 0) {
    std::cout << "Raw HTML is empty!" << std::endl;
  }
  HTMLTokeniser tokenizer;
  HTMLTreeBuilder treebuilder;
  std::vector<HTMLToken> html_tokens = tokenizer.Tokenize(raw_html);
  for (auto &html_token : html_tokens) {
    std::cout << html_token.ToString() << std::endl;
    if (html_token.name_ == "div") {
      html_token.tag_ = HTMLTag::kDiv;
    }
    treebuilder.ProcessToken(html_token);
  }
  return treebuilder.TakeTree();
}

} // namespace html
} // namespace ve
