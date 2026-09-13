#pragma once

#include "document/physicalfragment.h"
#include "document/style.h"

namespace ve::webplatform {
class LayoutAlgo {
public:
  virtual ~LayoutAlgo() = default;
  virtual void LayoutChild(const Style *child_style,
                           PhysicalFragment &child_fragment) = 0;
  virtual float OccupiedBlockSize() const = 0;
};

class BlockLayoutAlgo : public LayoutAlgo {
public:
  BlockLayoutAlgo(const Style &style);
  ~BlockLayoutAlgo() = default;

  void LayoutChild(const Style *child_style,
                   PhysicalFragment &child_fragment) override;
  float OccupiedBlockSize() const override;

private:
  float content_box_top_ = 0.f;
  float cursor_x_ = 0.f;
  float cursor_y_ = 0.f;
};

class LayoutContext {
public:
  LayoutContext(const Style style);
  void LayoutChild(const Style *child_style, PhysicalFragment &child_fragment);

  float OccupiedBlockSize() const;

private:
  std::unique_ptr<LayoutAlgo> layout_algo_;
};
}; // namespace ve::webplatform
