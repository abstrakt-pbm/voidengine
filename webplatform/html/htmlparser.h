#pragma once

#include "document/domnode.h"
namespace ve {
namespace html {
std::unique_ptr<webplatform::DomNode> ParseHTML(std::string raw_html);
}
} // namespace ve
