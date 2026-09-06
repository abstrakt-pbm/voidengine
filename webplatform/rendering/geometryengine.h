#pragma once

#include "document/containernode.h"
#include "document/div.h"
#include "document/elementnode.h"
#include "document/htmlelementnode.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/textelement.h"

#include "rendering/layoutengine/layoutbox.h"
#include "rendering/layoutengine/layoutimage.h"
#include "rendering/layoutengine/layoutnode.h"
#include "rendering/layoutengine/layouttext.h"

namespace ve::webplatform {

struct GeometryConstraints {
  float max_width = 0.0f;
};

// Оределение геометрии элемента и расположнение детей
// Чтобы движок геометрии мыслил только тем что реально будет отрисовано нужно
// выделить отдельный LayoutTree который будет формироваться из DOM и будет
// формироваться по правилам того что должно быть отрисовано например учёт
// display:none
class GeometryEngine {
public:
  // Layout
  std::unique_ptr<PhysicalFragment>
  CalculateLayoutNodeGeometry(const LayoutNode &layout_node,
                              const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateLayoutBoxGeometry(const LayoutBox &layout_root,
                             const GeometryConstraints &constrains);

  std::unique_ptr<PhysicalFragment>
  CalculateLayoutTextGeometry(const LayoutText &layout_text,
                              const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateLayoutImageGeometry(const LayoutImage &layout_text,
                               const GeometryConstraints &constrains);

  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(const LayoutNode &root_box);

  float viewport_width = 1280.0f;
};
} // namespace ve::webplatform
