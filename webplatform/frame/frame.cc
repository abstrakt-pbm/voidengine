#include "frame.h"
#include "frame/framenavigator.h"

namespace ve {
namespace webplatform {

Frame::Frame() : frame_navigator_(this) {}

void Frame::Navigate(const FrameNavigationRequest &request) {
  frame_navigator_.StartNavigation(request);
}

DocumentWindow *Frame::DomWindow() { return &doc_window_; }

} // namespace webplatform
} // namespace ve
