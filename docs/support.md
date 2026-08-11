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

Текущее поддерживаемое подмножество включает:

- вложенные блочные элементы;
- вертикальный normal flow;
- фиксированные размеры;
- автоматическую ширину block-элементов;
- автоматическую высоту block-элементов;
- background;
- padding;
- border;
- margin;
- `overflow: visible`;
- `overflow: hidden`;
- вложенный rectangular clipping.

---

# Layout

Статус: `PARTIAL`

## Поддерживается

- Вложенные блочные элементы.
- Дети располагаются вертикально сверху вниз.
- Координаты дочерних элементов задаются относительно родителя.
- Вложенный block layout.
- Фиксированная ширина элемента.
- Фиксированная высота элемента.
- Автоматическая ширина block-элемента в текущей упрощённой модели.
- Автоматическая высота block-элемента в текущей упрощённой модели.
- Передача ограничений доступной ширины от родителя к ребёнку.
- Вычисление размеров вложенных `height: auto` элементов снизу вверх.
- Учёт padding, border и margin при вычислении базовой геометрии.

## Модель ограничений

Для вычисления геометрии layout получает ограничения от containing block.

В текущей реализации поддерживается ограничение доступной ширины.

Для корневого элемента доступная ширина определяется viewport.

Для вложенного элемента доступная ширина определяется шириной
content box родителя.

## Не поддерживается

- `min-width` / `max-width`.
- `min-height` / `max-height`.
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
- `width: auto` для обычного block layout.
- `height: auto` для обычного block layout.
- Width и height сейчас описывают размер `border-box`.

### `width: auto`

В текущей модели автоматическая ширина block-элемента вычисляется
из доступной ширины containing block.

Для корневого элемента доступная ширина задаётся viewport.

Для вложенного элемента используется content box родителя.

Горизонтальные margin уменьшают доступную ширину border box элемента.

Не поддерживаются:

- `auto` margins;
- intrinsic sizing;
- shrink-to-fit;
- percentage width;
- min/max width.

### `height: auto`

В текущей модели автоматическая высота block-контейнера определяется
его normal-flow детьми.

Учитываются:

- высоты дочерних элементов;
- вертикальные margin дочерних элементов;
- padding родителя;
- border родителя.

Поддерживаются вложенные `height: auto` контейнеры.

Пустой `height: auto` контейнер сохраняет высоту, создаваемую его
padding и border.

Не поддерживаются:

- margin collapsing;
- intrinsic text sizing;
- floats;
- positioned descendants;
- min/max height;
- percentage height.

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
- Padding учитывается при вычислении content box.
- Вложенный padding.
- Padding участвует в вычислении `height: auto`.

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
- Border учитывается при вычислении content box.
- Border участвует в вычислении `height: auto`.
- Border рисуется вокруг `border-box` элемента.

### Не поддерживается

- Разная ширина border для отдельных сторон.
- Border styles.
- Настраиваемые цвета border.
- Border radius.
- Logical border properties.

---

## Margin

Статус: `PARTIAL`

### Поддерживается

- Четыре физические стороны.
- Неотрицательные числовые margin.
- `margin-left` влияет на горизонтальное положение элемента.
- `margin-top` влияет на вертикальное положение элемента.
- `margin-bottom` влияет на положение следующего block sibling.
- `margin-left` и `margin-right` учитываются при вычислении `width: auto`.
- Margin вложенных элементов вычисляется относительно области родителя.
- Вертикальные margin соседних block-элементов складываются без collapsing.

### Не поддерживается

- Margin collapsing.
- Отрицательные margin как гарантированная часть контракта.
- `auto` margin.
- Процентные margin.
- Logical margin properties.

---

# Overflow

Статус: `PARTIAL`

## Поддерживается

- `overflow: visible`.
- `overflow: hidden`.
- Прямоугольный clipping потомков.
- Вложенные `overflow: hidden`.
- Пересечение вложенных clip-областей.
- Восстановление родительского clip после завершения вложенного clip.
- Сброс clipping после выхода из самого внешнего clip-контекста.

`overflow: hidden` не изменяет геометрию дочернего
`PhysicalFragment`.

Layout продолжает хранить полный фактический размер элемента.

Ограничение видимой области применяется на этапе painting/rasterization.

## Не поддерживается

- `overflow: auto`.
- `overflow: scroll`.
- Scrollbars.
- Независимые `overflow-x` и `overflow-y`.
- Scrolling.
- Scroll containers.
- Rounded clipping.
- Clipping с transforms.
- Сложные clip paths.

---

# Painting

Статус: `PARTIAL`

## Поддерживается

- Отрисовка фона элемента.
- Отрисовка сплошного прямоугольного border.
- Родитель рисуется до своих детей.
- Отрисовка вложенных элементов.
- Display list из типизированных rendering commands.
- Прямоугольные clip-команды.
- Восстановление предыдущего clip-состояния.
- Независимость Web Platform слоя от конкретного graphics backend.

Фактическое применение clip выполняется graphics backend.

Движок определяет:

- необходимость clipping;
- геометрию clip;
- вложенность clip-состояний;
- момент восстановления предыдущего clip.

Backend выполняет уже вычисленные команды отрисовки и clipping.

## Не поддерживается

- Текст.
- Изображения.
- Градиенты.
- Тени.
- Border radius.
- Opacity.
- Transforms.
- Filters.
- Clip paths.
- Непрямоугольный clipping.

---

# Geometry

Статус: `PARTIAL`

Для результата layout используется дерево `PhysicalFragment`.

### Поддерживается

- Локальная геометрия элемента относительно родителя.
- `x`.
- `y`.
- `width`.
- `height`.
- Вложенные physical fragments.
- Фактические размеры после применения `AUTO` sizing.
- Передача внешних geometry constraints при layout.
- Использование geometry tree при построении display list.

Глобальные координаты для painting могут быть получены накоплением
смещений при рекурсивном обходе дерева.

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

Style задаётся внутренними структурами движка.

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

## Inline Formatting

Статус: `UNSUPPORTED`

---

# Тестирование

Поддерживаемая семантика Box Model и overflow фиксируется
conformance-тестами VoidEngine.

На текущем этапе покрыты:

- margin;
- fixed height;
- auto height;
- empty auto-height containers;
- fixed width;
- auto width;
- auto width относительно content box родителя;
- `overflow: visible`;
- `overflow: hidden`;
- восстановление родительского clip при вложенном clipping.

Тесты проверяют семантику движка на уровне geometry и display list,
не поведение конкретного graphics backend.

---

# Текущий frontier

Базовый block box layout текущего milestone уже включает:

- fixed/auto width;
- fixed/auto height;
- padding;
- border;
- margin;
- nested block flow;
- rectangular overflow clipping.

Следующее направление разработки должно выбираться после короткого
исследования соседних возможностей Web Platform.

Одним из основных следующих направлений является базовая поддержка текста:

- текстовые узлы;
- font metrics;
- формирование простейших line boxes;
- отрисовка текста.

Это направление является ориентировочным и может быть уточнено
после исследования минимальной модели text/inline layout.

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
