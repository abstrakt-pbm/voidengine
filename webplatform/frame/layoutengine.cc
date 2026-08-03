#include "layoutengine.h"

#include "frame/computed_style.h"
#include "frame/geometrytree.h"

namespace ve {
namespace webplatform {

GeometryTree LayoutEngine::MakeGeometryTree(const LayoutTree &layout_tree) {
  GeometryTree geometry_tree;

  Layout(layout_tree.root, );

  // Пусть есть LayoutBlockBox(контейнер располагает в себе детей)
  // LayoutBlockBox(ребёнок), нужно их расположить
  //
  // В начале родитель имеет свою позицию, ширину/высоту
  // Родитель знает позиции своих детей и выбирает положнение нового ребёнка
  // ниже последнего добавленнного ребёнка передаёт эту информацию ребёнку,
  // ребёнок возвращает фактический размер

  // Рассматриваем кейс блочного контейнера
  // Мы создаём блочный контейнер (дети располагаются сверху вниз)
  // у контейнера есть

  //

  return geometry_tree;
}

// Вычислить геометрию одного элемента в его относительных координатах
LayoutObject LayoutEngine::Layout(const LayoutElement &layout_element,
                                  const ComputedStyle &style) {
  // Получить ограничения от родителя
  // Какие стили влияют на размер элементов
  //
}

void LayoutBox::AppendChild(LayoutObject layout_object) {
  // определяем координаты объекта в своей собственной системе координат
  //
  // y = y самый нижний элемент из детей + его высота и pading/margin (либо
  // просто курсор по y)
  //
  // x = определяется падингом по ограничениям и стилям
  // дочернего элемента определяем его высоту и ширину, возвращаем родителю
  //
  //
  // Обязаность ребёнка сказать свою ширину и высоту
  // Обязаность родителя определить x,y ребёнка в локальной системе координат
}

} // namespace webplatform
} // namespace ve
