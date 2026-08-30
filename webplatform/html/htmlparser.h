#pragma once

#include "document/htmlelementnode.h"

namespace ve {
namespace html {
std::unique_ptr<webplatform::HtmlElementNode> ParseHTML(std::string raw_html);
}
} // namespace ve
