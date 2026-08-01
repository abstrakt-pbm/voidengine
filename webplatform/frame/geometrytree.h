#pragma once

#include <string>

namespace ve {
namespace webplatform {

// Containce calculated Document geometry

struct Colour {};
struct Font {};

struct Point {
  float x;
  float y;
};

struct Size {
  float height;
  float width;
};

struct Rect {
  Point point;
  Size size;
};

struct BoxGeometry {
  Rect rect;
  Colour colour;
};

struct TextGeometry {
  Rect bounds;
  Point baseline;
  Font font;
  Colour colour;
  std::string text;
};

class GeometryTreeNode {
public:
private:
};

class GeometryTree {
public:
private:
};
} // namespace webplatform
} // namespace ve
