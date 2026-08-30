#pragma once

#include "document/containernode.h"
#include "document/htmlelementnode.h"
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
  std::unique_ptr<webplatform::HtmlElementNode> TakeTree();

private:
  InsertionMode insertion_mode_ = InsertionMode::kInitialMode;

  // <html>
  std::unique_ptr<webplatform::HtmlElementNode> tree_root_;
  std::stack<webplatform::ContainerNode *> open_elements_;
};
} // namespace html
} // namespace ve
