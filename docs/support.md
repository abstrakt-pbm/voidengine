# Поддержка Web Platform в VoidEngine

Этот документ описывает функциональность Web Platform, которая намеренно
поддерживается текущей версией VoidEngine.

Используются следующие статусы:

- `SUPPORTED` — функциональность намеренно реализована и должна работать.
- `PARTIAL` — поддерживается только явно указанное подмножество функциональности.
- `UNSUPPORTED` — известная функциональность, которая пока не реализована.
- `UNTRIAGED` — функциональность существует в Web Platform, но её семантика
  ещё не была исследована применительно к VoidEngine.

Случайно работающее поведение за пределами явно описанной поддержки
не является частью контракта VoidEngine.

---

# Текущий milestone

## Базовый рендеринг статической страницы

Цель — уметь отображать простой статический документ из вложенных
блочных элементов с использованием минимальной модели CSS Box Model.

Целевые возможности:

- вложенные блочные элементы;
- фиксированные размеры;
- вертикальный normal flow;
- фон;
- padding;
- border;
- margin.

---

# Layout

##include <gtest/gtest.h>

#include <memory>

#include "document/div.h"
#include "document/documentpainter.h"
#include "document/style.h"

namespace ve {
namespace webplatform {

TEST(MarginTest, AppliesTopAndLeftMarginToChild) {
  Style root_style(100.0f, 100.0f, Style::Colour::RED);

  // left = 10, right = 0, top = 10, bottom = 0
  root_style.SetPadding(Padding(10.0f, 0.0f, 10.0f, 0.0f));
  root_style.border_width = 1.0f;

  Div root(root_style);

  Style child_style(20.0f, 20.0f, Style::Colour::GREEN);

  // left = 7, right = 0, top = 5, bottom = 0
  child_style.SetMargin(Margin(7.0f, 0.0f, 5.0f, 0.0f));

  root.AddChild(std::make_unique<Div>(child_style));

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(&root);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 1);

  const auto& child_fragment = fragment->child_fragments_[0];

  // x = border + padding-left + margin-left
  EXPECT_FLOAT_EQ(child_fragment->x_, 1.0f + 10.0f + 7.0f);

  // y = border + padding-top + margin-top
  EXPECT_FLOAT_EQ(child_fragment->y_, 1.0f + 10.0f + 5.0f);
}

TEST(MarginTest, VerticalMarginsAffectSiblingPosition) {
  Style root_style(100.0f, 200.0f, Style::Colour::RED);
  Div root(root_style);

  Style first_style(20.0f, 20.0f, Style::Colour::GREEN);

  // top = 5, bottom = 7
  first_style.SetMargin(Margin(0.0f, 0.0f, 5.0f, 7.0f));

  Style second_style(20.0f, 30.0f, Style::Colour::BLUE);

  // top = 3
  second_style.SetMargin(Margin(0.0f, 0.0f, 3.0f, 0.0f));

  root.AddChild(std::make_unique<Div>(first_style));
  root.AddChild(std::make_unique<Div>(second_style));

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(&root);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 2);

  const auto& first = fragment->child_fragments_[0];
  const auto& second = fragment->child_fragments_[1];

  EXPECT_FLOAT_EQ(first->y_, 5.0f);

  // first:
  // margin-top    = 5
  // height        = 20
  // margin-bottom = 7
  //
  // cursor = 5 + 20 + 7 = 32
  //
  // second margin-top = 3
  //
  // second.y = 32 + 3 = 35
  EXPECT_FLOAT_EQ(second->y_, 35.0f);
}

TEST(MarginTest, NestedMarginIsRelativeToParentContentArea) {
  Style root_style(100.0f, 100.0f, Style::Colour::RED);

  // left = 10, right = 0, top = 10, bottom = 0
  root_style.SetPadding(Padding(10.0f, 0.0f, 10.0f, 0.0f));
  root_style.border_width = 1.0f;

  Div root(root_style);

  Style child_style(50.0f, 50.0f, Style::Colour::GREEN);

  // left = 6, right = 0, top = 4, bottom = 0
  child_style.SetMargin(Margin(6.0f, 0.0f, 4.0f, 0.0f));

  root.AddChild(std::make_unique<Div>(child_style));

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(&root);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 1);

  const auto& child = fragment->child_fragments_[0];

  EXPECT_FLOAT_EQ(child->x_, 1.0f + 10.0f + 6.0f);
  EXPECT_FLOAT_EQ(child->y_, 1.0f + 10.0f + 4.0f);
}

} // namespace webplatform
} // namespace ve# Block Layout

Статус: `PARTIAL`

### Поддерживается

- Вложенные блочные элементы.
- Дети располагаются вертикально сверху вниз.
- Координаты дочерних элементов задаются относительно родителя.
- Вложенный block layout.
- Фиксированная ширина элемента.
- Фиксированная высота элемента.

### Не поддерживается

- Автоматическая ширина.
- Автоматическая высота.
- Min/max sizing.
- Intrinsic sizing.
- Процентные размеры.
- Absolute positioning.
- Relative positioning.
- Fixed positioning.
- Floats.
- Несколько formatting contexts.
- Fragmentation.
- Writing modes.

---

# CSS Box Model

## Width / Height

Статус: `PARTIAL`

### Поддерживается

- Фиксированная числовая ширина.
- Фиксированная числовая высота.
- Width и height сейчас задают размер `border-box`.

### Не поддерживается

- `auto`.
- Процентные значения.
- `min-width`.
- `max-width`.
- `min-height`.
- `max-height`.
- Intrinsic sizing keywords.

---

## Padding

Статус: `SUPPORTED`

### Поддерживается

- Четыре физические стороны:
  - top;
  - right;
  - bottom;
  - left.
- Padding влияет на внутреннее положение дочерних элементов.
- Вложенный padding.

### Не поддерживается

- Процентный padding.
- Logical properties, например `padding-inline`.
- Парсинг CSS-значений.

---

## Border

Статус: `PARTIAL`

### Поддерживается

- Одинаковая ширина border со всех сторон.
- Border учитывается при вычислении положения дочерних элементов.
- Border рисуется вокруг `border-box` элемента.

### Не поддерживается

- Разная ширина border для отдельных сторон.
- Border styles.
- Настраиваемые цвета border.
- Border radius.
- Logical border properties.

---

## Margin

Статус: `PARTIAL/SUPPORTED`

Планируется как следующее расширение базовой Box Model.

### Первое планируемое подмножество

- Неотрицательные margin.
- Четыре физические стороны.
- Margin между вертикально расположенными block-элементами.

### Не планируется в первой реализации

- Margin collapsing.
- Отрицательные margin.
- `auto` margin.
- Процентные margin.
- Logical margin properties.

---

# Painting

Статус: `PARTIAL`

### Поддерживается

- Отрисовка фона элемента.
- Отрисовка сплошного прямоугольного border.
- Родитель рисуется до своих детей.
- Отрисовка вложенных элементов.

### Не поддерживается

- Текст.
- Изображения.
- Градиенты.
- Тени.
- Border radius.
- Opacity.
- Transforms.
- Filters.
- Сложный clipping.

---

# Text

Статус: `UNSUPPORTED`

### Не поддерживается

- Layout текстовых узлов.
- Выбор шрифта.
- Font metrics.
- Glyph shaping.
- Line boxes.
- Inline formatting.
- Отрисовка текста.

---

# CSS

Статус: `PARTIAL`

VoidEngine сейчас реализует только ту семантику стилей,
которая требуется текущему milestone рендеринга.

### Не поддерживается

- CSS parser.
- Cascade.
- Specificity.
- Inheritance.
- Stylesheets.
- Selectors.
- Media queries.
- Custom properties.
- Animations.
- Transitions.

---

# Продвинутый Layout

## Flexbox

Статус: `UNSUPPORTED`

## Grid

Статус: `UNSUPPORTED`

## Positioned Layout

Статус: `UNSUPPORTED`

## Floats

Статус: `UNSUPPORTED`

---

# Текущий frontier

Ближайшие направления разработки:

1. Базовая поддержка margin.
2. Автоматическое вычисление размеров block-элементов.
3. Базовый layout и отрисовка текста.

Текущей задачей считается только первый пункт.

Последующие пункты являются ориентировочными и могут изменяться
по мере развития реализации и получения новой информации о домене.

---

# Политика поддержки

Функциональность считается поддерживаемой только если:

1. её поддерживаемая семантика явно определена;
2. реализация обеспечивает эту семантику;
3. поведение покрыто тестами.

Поведение, которое случайно работает, но не описано и не покрыто тестами,
не считается поддерживаемым.

Отсутствие неподдерживаемой функциональности не является багом.

Багом считается поведение, которое нарушает явно заявленную
поддерживаемую семантику.
