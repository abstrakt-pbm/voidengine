#include "frame.h"
#include "frame/document.h"
#include "frame/framenavigator.h"

#include "layouttreebuilder.h"

namespace ve {
namespace webplatform {
class StyleResolver;
Frame::Frame() : frame_navigator_(this) {}

void Frame::Navigate(const FrameNavigationRequest &request) {
  frame_navigator_.StartNavigation(request);
  const Document &doc = DomWindow()->Document();
  style_resolver_.ResolveStyles(doc);
  auto layout_tree = layout_tree_builder_.MakeLayoutTree(doc);
  // Завтра делать FramePainter::Paint
  // Делать начиная с того какие требования к командам отрисовки
  frame_painter_.Paint(layout_tree);
}

DocumentWindow *Frame::DomWindow() { return &doc_window_; }

} // namespace webplatform
} // namespace ve
