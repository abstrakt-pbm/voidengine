#pragma once

#include "document/containernode.h"
#include "document/div.h"
#include "document/elementnode.h"
#include "document/htmlelementnode.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/textelement.h"

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
  // вводим алгоритм геометрии контейнерной ноды

  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(const HtmlElementNode &htmlelementnode);

  std::unique_ptr<PhysicalFragment>
  CalculateElementGeometry(const DomNode &element_node,
                           const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateTextGeometry(const TextElement &text_element,
                        const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateDivGeometry(const Div &div, const GeometryConstraints &constrains);

  std::unique_ptr<PhysicalFragment>
  CalculateImageGeometry(const ImageElement &img,
                         const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateContainerNodeGeometry(const ElementNode &container_node,
                                 const GeometryConstraints &constrains);

  float viewport_width = 1280.0f;
};
} // namespace ve::webplatform
