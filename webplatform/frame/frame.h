#pragma once

#include "framenavigator.h"

namespace ve {
namespace webplatform {
class JSContext;
class Document;

// Container for particular document
class DocumentWindow {
public:
  void InstallDocument(Document *document);

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
};

} // namespace webplatform
} // namespace ve
