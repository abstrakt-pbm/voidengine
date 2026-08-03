#pragma once

#include "geometrytree.h"
#include "layouttree.h"

namespace ve {
namespace webplatform {

class ViewPort {
public:
private:
  float width_;
};

struct Constrains {
  float width;
};

class LayoutObject {
public:
private:
  float x;
  float y;

  float heigth;
  float width;
};

class LayoutBox : public LayoutObject {
public:
  // на основе
  void AppendChild(LayoutObject layout_object);

private:
};

class LayoutText {
public:
private:
};

class LayoutEngine {
public:
  GeometryTree MakeGeometryTree(const LayoutTree &layout_tree);
  LayoutObject Layout(const LayoutElement &layout_element,
                      const ComputedStyle &style);

private:
};
} // namespace webplatform
} // namespace ve
