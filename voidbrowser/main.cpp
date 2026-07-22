#include "frame/framenavigator.h"
#include <frame/frame.h>
#include <iostream>

// voidbrowser
int main() {
  ve::webplatform::FrameNavigationRequest nav_req(
      "/home/pablo/devel/voidengine/voidbrowser/testres/index.html");
  ve::webplatform::Frame frame;
  frame.Navigate(nav_req);
  return 0;
}
