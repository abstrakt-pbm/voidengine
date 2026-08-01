#pragma once

#include "frame/document.h"
#include "frame/htmltoken.h"

namespace ve {
namespace webplatform {

class HTMLTreeBuilder {
public:
  enum class InsertionMode {
    kInitialMode,
  };

  HTMLTreeBuilder(Document &document);

  void ProcessToken(const HTMLToken &token);

private:
  InsertionMode insertion_mode_;
  Document &document_;
};
} // namespace webplatform
} // namespace ve
