#pragma once

#include "frame/layouttreebuilder.h"
#include "frame/styleresolver.h"
#include "framenavigator.h"
#include "renderer/framepainter.h"

namespace ve {
namespace webplatform {
class JSContext;
class Document;

// Container for particular document
class DocumentWindow {
public:
  void InstallDocument(Document *document);

  const Document &Document();

private:
  // in future DocumentWindow will own JSContext

  Document *doc_;
};

// One particular Document environment
class Frame {
public:
  Frame();
  void Navigate(const FrameNavigationRequest &request);
  DocumentWindow *DomWindow();

private:
  DocumentWindow doc_window_;
  FrameNavigator frame_navigator_;

  FramePainter frame_painter_;
  StyleResolver style_resolver_;
  LayoutTreeBuilder layout_tree_builder_;
};

} // namespace webplatform
} // namespace ve
