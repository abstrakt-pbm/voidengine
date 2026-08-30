#pragma once

#include "document/domnode.h"
#include "document/physicalfragment.h"
#include "document/style.h"

namespace ve::webplatform {
// Сущность используется чтобы расположить готовую геометрию ребёнка
// на вход подаётся стиль родителя по которому определяется тип layout-а
// получает на вход геометрию, энжн заполняет поля в структуре на позицию

class LayoutAlgo {
public:
  virtual void LayoutChild(const Style *child_style,
                           PhysicalFragment &child_fragment) = 0;
};

class BlockLayoutAlgo : public LayoutAlgo {
public:
  BlockLayoutAlgo(const Style &style);
  void LayoutChild(const Style *child_style,
                   PhysicalFragment &child_fragment) override;

private:
  float cursor_x = 0.0f;
  float cursor_y = 0.0f;
  const Style &style_;
};

class LayoutContext {
public:
  LayoutContext(const Style style);
  void LayoutChild(const Style *child_style, PhysicalFragment &child_fragment);

private:
  Style style_;
  std::unique_ptr<LayoutAlgo> layout_algo_;
};
}; // namespace ve::webplatform
