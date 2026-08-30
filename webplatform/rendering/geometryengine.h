#pragma once

#include "document/div.h"
#include "document/htmlelementnode.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/textelement.h"

namespace ve::webplatform {

struct GeometryConstraints {
  float max_width = 0.0f;
};

// Оределение геометрии элемента и расположнение детей
class GeometryEngine {
public:
  std::unique_ptr<PhysicalFragment>
  CalculateDocumentGeometry(const HtmlElementNode &htmlelementnode);

  std::unique_ptr<PhysicalFragment>
  CalculateElementGeometry(const DomNode &dom_node,
                           const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateTextGeometry(const TextElement &text_element,
                        const GeometryConstraints &constrains);
  std::unique_ptr<PhysicalFragment>
  CalculateDivGeometry(const Div &div, const GeometryConstraints &constrains);

  std::unique_ptr<PhysicalFragment>
  CalculateImageGeometry(const ImageElement &img,
                         const GeometryConstraints &constrains);

  float viewport_width = 1280.0f;
};
} // namespace ve::webplatform
