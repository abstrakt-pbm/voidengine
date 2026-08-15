#pragma once

#include "document/domnode.h"
#include "html/htmltoken.h"

#include <stack>

namespace ve {
namespace html {
class HTMLTreeBuilder {
public:
  enum class InsertionMode {
    kInitialMode,
  };

  HTMLTreeBuilder();

  void ProcessToken(const HTMLToken &html_token);
  std::unique_ptr<webplatform::DomNode> TakeTree();

private:
  InsertionMode insertion_mode_ = InsertionMode::kInitialMode;
  std::unique_ptr<webplatform::DomNode> tree_root_;
  std::stack<webplatform::DomNode *> open_elements_;
};
} // namespace html
} // namespace ve
