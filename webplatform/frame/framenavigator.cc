#include "framenavigator.h"

#include "document.h"
#include "frame.h"

#include <iostream>

namespace ve {
namespace webplatform {

FrameNavigator::FrameNavigator(Frame *frame) : frame_(frame) {}

void FrameNavigator::StartNavigation(
    const FrameNavigationRequest &navigation_request) {
  const ResourceRequest resource_request(navigation_request.Url());
  ResourceResponse res = resource_loader_.FetchResource(resource_request);
  std::string decoded_html = text_decoder_.Decode(res.Payload());
  Document document = document_parcer_.ParseDocument(decoded_html);
  // auto dom_window = frame_->DomWindow();
  // dom_window->InstallDocument(&document);
}

FrameNavigationRequest::FrameNavigationRequest(std::string url) : url_(url) {}
std::string FrameNavigationRequest::Url() const { return url_; }

} // namespace webplatform
} // namespace ve
