#pragma once

#include "document/physicalfragment.h"
#include "document/style.h"
#include "rendering/geometryengine.h"
#include "rendering/layoutcontext.h"

#include <memory>

namespace ve::webplatform {
class Box {
public:
  Box(float width, float height);
  float Width() const;
  float Height() const;

private:
  float width_ = 0.f;
  float height_ = 0.f;
};

class FragmentBuilder {
public:
  FragmentBuilder(const Style &style,
                  const GeometryConstraints &geometry_constrains);
  std::unique_ptr<PhysicalFragment> Build();

  float FutureFragmentContentBoxWidth();
  void AddFragmentChild(const Style *child_style,
                        std::unique_ptr<PhysicalFragment> child_fragment);

  Box ContentBoxSnapshot();

private:
  const Style &style_;
  const GeometryConstraints &geometry_constrains_;
  LayoutContext layout_context_;
  std::unique_ptr<PhysicalFragment> fragment_;
};
} // namespace ve::webplatform
