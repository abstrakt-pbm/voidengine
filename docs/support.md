# Поддержка Web Platform в VoidEngine

Этот документ описывает функциональность Web Platform, которая намеренно
поддерживается текущей версией VoidEngine.

Документ описывает именно контракт движка, а не всё поведение, которое
случайно работает в текущей реализации.

Используются следующие статусы:

- `SUPPORTED` — функциональность намеренно реализована и должна работать.
- `PARTIAL` — поддерживается только явно указанное подмножество функциональности.
- `UNSUPPORTED` — известная функциональность, которая пока не реализована.
- `UNTRIAGED` — функциональность существует в Web Platform, но её семантика
  ещё не была исследована применительно к VoidEngine.

Поведение за пределами явно описанной поддержки не является частью
контракта VoidEngine.

---

# Текущий milestone

## Базовый рендеринг статического HTML-документа

Текущий вертикальный срез движка:

```text
HTML
  ↓
HTMLTokeniser
  ↓
HTMLTreeBuilder
  ↓
DOM-like tree
  ↓
GeometryEngine
  ↓
PhysicalFragment tree
  ↓
PainterEngine
  ↓
DisplayList
  ↓
graphics backend
```

Текущее поддерживаемое подмножество позволяет:

- разобрать простой HTML;
- построить вложенное дерево `<div>`;
- создать текстовые узлы;
- создать `<img>`;
- выполнить упрощённый block layout;
- вычислить geometry tree;
- сформировать display list;
- нарисовать:
  - background;
  - border;
  - текст;
  - изображения;
- применять прямоугольный `overflow: hidden`;
- выполнять nested clipping.

VoidEngine пока не является полноценной реализацией HTML/CSS и не пытается
реализовывать всё поведение браузера.

---

# HTML

Статус: `PARTIAL`

HTML pipeline состоит из:

```text
raw HTML
  ↓
HTMLTokeniser
  ↓
HTMLToken[]
  ↓
HTMLTreeBuilder
  ↓
DomNode tree
```

## Tokenizer

### Поддерживается

Поддерживаются токены:

- StartTag;
- EndTag;
- Character;
- EOF.

Поддерживается базовый parsing:

```html
<div>Hello</div>
```

Поддерживаются атрибуты:

```html
<img src="image.png" width="160" height="100">
```

Поддерживаются:

- атрибуты в двойных кавычках;
- атрибуты в одинарных кавычках;
- unquoted attribute values;
- несколько атрибутов;
- HTML whitespace между атрибутами;
- перевод имён тегов в lowercase;
- перевод имён атрибутов в lowercase;
- сохранение регистра attribute values;
- определение self-closing syntax;
- получение значения атрибута по имени.

Например:

```html
<IMG SRC="Images/Cat.PNG" WIDTH="160">
```

tokenizer приводит имена к:

```text
img
src
width
```

но значение:

```text
Images/Cat.PNG
```

сохраняется без изменения.

Порядок HTML-атрибутов не является частью семантики пользователя
`HTMLToken`.

Внутренний код может получать атрибут по имени независимо от его позиции
в исходном HTML.

### Текущий контракт отсутствующего атрибута

Если атрибут не найден:

```cpp
GetAttributeValue("name")
```

возвращает пустую строку.

Это временный API-контракт текущей реализации.

### Не поддерживается

Не являются частью текущего контракта:

- полный WHATWG HTML tokenizer;
- character references;
- named entities;
- numeric entities;
- comments;
- DOCTYPE;
- CDATA;
- script data states;
- RAWTEXT;
- RCDATA;
- полноценный error recovery;
- parse errors по HTML Standard;
- корректное поведение для произвольного malformed HTML.

---

# HTML Tree Builder

Статус: `PARTIAL`

## Поддерживается

Tree Builder поддерживает:

- `<div>`;
- `<img>`;
- Character tokens внутри поддерживаемого дерева;
- вложенные `<div>`;
- `TextElement`;
- `ImageElement`;
- stack of open elements;
- `<img>` как void element.

Например:

```html
<div>
  A
  <div>
    B
    <div>C</div>
  </div>
  D
</div>
```

строит структуру:

```text
Div
├── Text("A")
├── Div
│   ├── Text("B")
│   └── Div
│       └── Text("C")
└── Text("D")
```

`<img>` не помещается в stack of open elements.

Поэтому:

```html
<div>
  <img src="image.png" width="160" height="100">
  <div></div>
</div>
```

строится как:

```text
Div
├── Image
└── Div
```

а не:

```text
Div
└── Image
    └── Div
```

## Ограничения текущего Tree Builder

Сейчас предполагается корректный HTML в рамках поддерживаемого subset.

В частности предполагается:

- корректная вложенность `<div>`;
- соответствующие end tags;
- один настоящий root element;
- root element, пригодный для текущего layout pipeline.

Character tokens непосредственно под synthetic root не создают
`TextElement`.

HTML error recovery пока отсутствует.

## Не поддерживается

- `<html>`;
- `<head>`;
- `<body>`;
- `<p>`;
- headings;
- lists;
- tables;
- forms;
- links;
- semantic elements;
- HTML insertion modes;
- implied elements;
- foster parenting;
- adoption agency algorithm;
- scope algorithms;
- автоматическое исправление malformed nesting;
- полноценная DOM construction semantics HTML Standard.

---

# DOM

Статус: `PARTIAL`

Текущая DOM-like модель содержит базовый:

```text
DomNode
```

с owning children.

Специализированные типы:

```text
Div
TextElement
ImageElement
```

## Поддерживается

- дерево owning child nodes;
- произвольная глубина вложенности поддерживаемых node types;
- `Div` как block container;
- `TextElement` как text leaf;
- `ImageElement` как image leaf.

## Не поддерживается

- Web IDL DOM API;
- `Document`;
- `Element`;
- `HTMLElement` hierarchy Web Platform;
- attributes как часть DOM Element API;
- mutation observers;
- DOM events;
- scripting;
- query selectors;
- node adoption;
- shadow DOM;
- custom elements.

Текущие классы являются внутренней моделью VoidEngine и не заявляются
как полноценная реализация DOM Standard.

---

# Layout

Статус: `PARTIAL`

## Поддерживается

- Вложенные блочные элементы.
- Вертикальный normal flow.
- Дети располагаются сверху вниз.
- Локальные координаты ребёнка относительно родителя.
- Вложенный block layout.
- Фиксированная ширина.
- Фиксированная высота.
- Автоматическая ширина block-элемента.
- Автоматическая высота block-элемента.
- Передача ограничения доступной ширины родитель → ребёнок.
- Вычисление nested `height: auto` снизу вверх.
- Padding.
- Border.
- Margin.
- Text leaf fragments.
- Image leaf fragments.
- Участие текста в normal vertical flow.
- Участие изображения в normal vertical flow.
- Участие текста и изображения в `height: auto` родителя.

---

# Geometry constraints

Layout получает ограничения от containing block.

Текущая структура ограничений содержит доступную ширину:

```text
max_width
```

Для document root доступная ширина определяется viewport.

Для вложенного элемента доступная ширина определяется content area
родителя.

Сейчас constraint propagation ориентирован прежде всего на block flow
и text wrapping.

Не поддерживаются полноценные min/max constraint systems и intrinsic
sizing Web Platform.

---

# CSS Box Model

Статус: `PARTIAL`

## Width / Height

Статус: `PARTIAL`

### Поддерживается

- фиксированная числовая ширина;
- фиксированная числовая высота;
- `width: auto`;
- `height: auto`.

В текущей модели значения `width` и `height` описывают размер:

```text
border-box
```

### `width: auto`

Автоматическая ширина block-элемента вычисляется из доступной ширины
containing block.

Для root используется viewport width.

Для вложенного элемента используется ширина content area родителя.

Горизонтальные margin уменьшают доступную ширину border box.

Не поддерживаются:

- `auto` margins;
- intrinsic sizing;
- shrink-to-fit;
- percentage width;
- min/max width.

### `height: auto`

Автоматическая высота block container определяется normal-flow детьми.

Учитываются:

- высота дочерних `Div`;
- высота `TextElement`;
- высота `ImageElement`;
- вертикальные margin дочерних `Div`;
- padding родителя;
- border родителя.

Поддерживаются вложенные `height: auto` containers.

Пустой `height: auto` container сохраняет высоту, создаваемую padding
и border.

Не поддерживаются:

- margin collapsing;
- positioned descendants;
- floats;
- min/max height;
- percentage height.

---

# Padding

Статус: `SUPPORTED`

## Поддерживается

Четыре физические стороны:

- left;
- right;
- top;
- bottom.

Padding:

- влияет на положение children;
- определяет content area;
- учитывается при вычислении доступной ширины children;
- поддерживается во вложенных элементах;
- участвует в `height: auto`.

## Не поддерживается

- percentage padding;
- logical properties;
- CSS parsing значений.

---

# Border

Статус: `PARTIAL`

## Поддерживается

- одинаковая ширина border со всех четырёх сторон;
- border входит в border-box;
- border влияет на origin content area;
- border уменьшает доступную content width;
- border участвует в `height: auto`;
- border рисуется вокруг элемента;
- `overflow: hidden` clip начинается после border.

## Painting

Текущий border:

- прямоугольный;
- сплошной;
- одной ширины со всех сторон;
- отрисовывается фиксированным цветом backend-а.

## Не поддерживается

- разные width для отдельных сторон;
- border style;
- произвольный border colour через CSS;
- border radius;
- logical border properties.

---

# Margin

Статус: `PARTIAL`

## Поддерживается

Для `Div` поддерживаются четыре физических margin.

В частности:

- `margin-left` влияет на horizontal placement;
- `margin-top` влияет на vertical placement;
- `margin-bottom` влияет на следующий sibling;
- `margin-left` и `margin-right` участвуют в `width: auto`;
- nested margin применяется относительно content area parent;
- соседние vertical margins сейчас складываются.

## Не поддерживается

- margin collapsing;
- отрицательные margins как гарантированный контракт;
- `auto` margins;
- percentage margins;
- logical margin properties;
- margins для всех возможных node types.

---

# Text

Статус: `PARTIAL`

VoidEngine имеет базовый text layout и text painting.

## Text nodes

HTML Character tokens внутри поддерживаемого DOM tree создают:

```text
TextElement
```

TextElement участвует в layout как leaf node.

## Font metrics

Текущий Web Platform слой получает заранее подготовленные:

- font size;
- ascent;
- descent;
- glyph advance.

В текущем `voidbrowser` эти значения временно подготавливаются через
SDL_ttf перед layout.

Это не является окончательной font architecture VoidEngine.

## Текущая модель layout

Text layout предполагает единый glyph advance:

```text
glyph_advance
```

Доступное количество символов в строке определяется приблизительно как:

```text
available_width / glyph_advance
```

После чего строка разбивается на `TextLineFragment`.

Поддерживается:

- несколько text lines;
- перенос по доступной ширине;
- line height через ascent + descent;
- baseline;
- text fragment height;
- участие текста в vertical flow;
- участие текста в auto-height parent;
- глобальные painting coordinates для вложенного текста.

## Painting

Painter создаёт:

```text
DrawTextCommand
```

с:

- x;
- baseline_y;
- font_size;
- text payload.

Backend выполняет rasterization текста.

## Ограничения

Текущий text layout является сильно упрощённым.

Не поддерживаются:

- полноценный inline formatting context;
- word wrapping по правилам CSS;
- Unicode line breaking;
- shaping;
- HarfBuzz-like shaping;
- kerning-aware layout;
- variable glyph advances в layout;
- bidi;
- complex scripts;
- font fallback;
- font-family;
- font-weight;
- font-style;
- CSS font selection;
- inline elements;
- text-align;
- whitespace processing по HTML/CSS;
- text decoration;
- letter spacing;
- word spacing.

Текущая модель ориентирована на простой monospace-like prototype.

---

# Images

Статус: `PARTIAL`

## HTML

Поддерживается:

```html
<img src="image.png" width="160" height="100">
```

`<img>` рассматривается как void element.

Атрибуты читаются по имени, поэтому их порядок не имеет значения.

Например:

```html
<img height="100" src="image.png" width="160">
```

эквивалентен текущей модели.

## Layout

Image является leaf fragment.

Размер изображения в текущей модели задаётся явно:

```text
width
height
```

Image:

- участвует в vertical flow;
- сдвигает следующий sibling;
- участвует в `height: auto` parent;
- получает локальные координаты от GeometryEngine.

## Painting

Painter создаёт:

```text
DrawImageCommand
```

с:

- x;
- y;
- width;
- height;
- path к изображению.

Глобальные координаты изображения вычисляются накоплением local offsets
вложенных fragments.

## Backend

Текущий SDL backend загружает image texture через SDL_image.

Texture cache пока отсутствует.

Изображение может загружаться заново при каждом render frame.

## Ограничения

Не поддерживаются как часть Web Platform contract:

- intrinsic image dimensions;
- automatic sizing из самого файла;
- aspect-ratio;
- responsive images;
- `srcset`;
- `<picture>`;
- object-fit;
- object-position;
- network image loading;
- resource loader;
- caching;
- asynchronous decoding;
- loading states;
- alt rendering;
- broken-image UI.

Текущий `<img>` ожидает корректные числовые `width` и `height`.

---

# Overflow

Статус: `PARTIAL`

## Поддерживается

- `overflow: visible`;
- `overflow: hidden`;
- rectangular clipping;
- nested `overflow: hidden`;
- пересечение nested clip rectangles;
- восстановление parent clip;
- reset clip после выхода из outermost clipping context;
- пустой результат пересечения clip rectangles;
- clipping относительно внутренней границы border.

`overflow: hidden` не изменяет layout geometry descendant.

Полная geometry сохраняется в `PhysicalFragment`.

Ограничение видимой области применяется во время painting/rasterization.

## Clip geometry

Для элемента:

```text
border-box = (x, y, width, height)
border = b
```

текущий clip rectangle:

```text
x      = x + b
y      = y + b
width  = width  - 2b
height = height - 2b
```

То есть border не входит в clipping area потомков.

## Nested clipping

Новый clip не может расширить уже активную visible region.

Эффективный nested clip определяется как:

```text
effective_clip = parent_clip ∩ child_clip
```

Например:

```text
parent = (0, 0, 100, 100)
child  = (50, 50, 100, 100)
```

результат:

```text
(50, 50, 50, 50)
```

Если rectangles не пересекаются:

```text
width  = 0
height = 0
```

## Не поддерживается

- `overflow: auto`;
- `overflow: scroll`;
- scrollbars;
- scrolling;
- scroll containers;
- независимые `overflow-x` / `overflow-y`;
- rounded clipping;
- transforms + clipping;
- clip paths;
- произвольные clip shapes.

---

# Geometry

Статус: `PARTIAL`

Результатом layout является дерево:

```text
PhysicalFragment
```

Существуют специализированные fragments:

```text
BoxPhysicalFragment
TextPhysicalFragment
ImagePhysicalFragment
TextLineFragment
```

## Поддерживается

- local x/y относительно parent;
- width;
- height;
- nested fragments;
- фактический размер после AUTO sizing;
- geometry constraints;
- text lines;
- text baseline;
- image dimensions;
- geometry tree → display list.

Главный coordinate invariant:

```text
global_position =
    parent_global_position + fragment_local_position
```

Painter рекурсивно накапливает offsets при обходе geometry tree.

Этот контракт проверяется отдельно для вложенных text и image fragments.

---

# Painting

Статус: `PARTIAL`

Painter преобразует:

```text
PhysicalFragment tree
```

в:

```text
DisplayList
```

## Rendering commands

Текущий DisplayList содержит:

```text
FillRectCommand
DrawBorderCommand
ClipCommand
ResetClipCommand
DrawTextCommand
DrawImageCommand
```

## Поддерживается

- solid background;
- rectangular border;
- text commands;
- image commands;
- rectangular clipping;
- nested clipping;
- restoration предыдущего clip;
- global coordinate accumulation;
- parent painting перед descendants;
- независимость основного Web Platform слоя от SDL.

Painter определяет:

- что рисовать;
- геометрию drawing commands;
- global coordinates;
- clip geometry;
- clip nesting;
- момент восстановления clip.

Graphics backend выполняет уже сформированные команды.

## Не поддерживается

- gradients;
- shadows;
- opacity;
- transforms;
- filters;
- compositing;
- stacking contexts;
- z-index;
- border radius;
- clip paths;
- non-rectangular clipping;
- полноценная CSS painting order model.

---

# Graphics backend

Статус: `PARTIAL`

Текущий executable `voidbrowser` использует:

- SDL3;
- SDL_ttf;
- SDL_image.

Backend умеет исполнять текущий DisplayList.

Текущий запуск:

```text
voidbrowser <font.ttf> <page.html>
```

Backend предоставляет:

- window;
- renderer;
- solid rectangles;
- border rasterization;
- rectangular clip;
- text rasterization;
- image rasterization.

## Ограничения

- один font face на документ;
- один font size;
- font subsystem находится вне Web Platform слоя;
- image texture cache отсутствует;
- text texture cache отсутствует;
- display list строится один раз перед render loop;
- полноценный reflow/repaint lifecycle отсутствует.

---

# CSS

Статус: `PARTIAL`

VoidEngine реализует внутреннюю semantic model нескольких CSS concepts,
но пока не имеет CSS language frontend.

Style задаётся внутренней структурой:

```text
Style
```

## Семантически представлены

- width;
- height;
- width mode;
- height mode;
- background colour;
- padding;
- border width;
- margin;
- overflow.

## Не поддерживается

- CSS parser;
- stylesheets;
- selectors;
- cascade;
- specificity;
- inheritance;
- computed style system;
- initial values Web Platform;
- UA stylesheet;
- author stylesheet;
- media queries;
- custom properties;
- animations;
- transitions.

Следовательно наличие внутреннего поля `Style` не означает поддержку
соответствующего CSS syntax.

---

# Продвинутый Layout

## Inline Formatting

Статус: `UNSUPPORTED`

Существующая text wrapping модель не считается реализацией полноценного
CSS Inline Formatting Context.

## Flexbox

Статус: `UNSUPPORTED`

## Grid

Статус: `UNSUPPORTED`

## Positioned Layout

Статус: `UNSUPPORTED`

Не поддерживаются:

- `position: relative`;
- `position: absolute`;
- `position: fixed`;
- containing block rules positioned elements.

## Floats

Статус: `UNSUPPORTED`

## Tables

Статус: `UNSUPPORTED`

## Multi-column / fragmentation

Статус: `UNSUPPORTED`

---

# JavaScript

Статус: `UNSUPPORTED`

Не поддерживаются:

- JavaScript runtime;
- script execution;
- DOM bindings;
- event loop;
- timers;
- promises;
- Web APIs.

---

# Networking / Resources

Статус: `UNSUPPORTED`

Текущий browser executable читает HTML из локального файла.

Не поддерживаются как часть Web Platform:

- HTTP;
- HTTPS;
- URL loader;
- redirects;
- cache;
- cookies;
- CSP;
- CORS;
- fetch;
- resource scheduling.

Images сейчас также являются прямыми paths для backend-а, а не
полноценными Web Platform resources.

---

# Тестирование

Поддерживаемая семантика фиксируется regression/conformance tests.

Текущие тесты покрывают несколько независимых границ системы.

## Box geometry

Проверяются:

- margin;
- nested margin;
- sibling vertical positioning;
- fixed width;
- auto width;
- parent content width;
- fixed height;
- auto height;
- empty auto-height containers;
- padding;
- border contribution.

## Overflow

Проверяются:

- `overflow: visible`;
- базовый `overflow: hidden`;
- restoration parent clip;
- intersection nested clips;
- completely non-intersecting clips;
- clip geometry относительно border.

## Text

Проверяются:

- участие text fragment в vertical flow;
- участие текста в auto-height;
- wrapping по available width;
- global painting coordinates вложенного текста.

## Images

Проверяются:

- участие image в vertical flow;
- участие image в auto-height;
- global painting coordinates вложенного изображения.

## HTML parser

Проверяются:

- `<img>` как void element;
- nested `<div>` structure;
- text nodes внутри nested `<div>`.

## HTML tokenizer

Проверяются:

- EOF;
- plain character data;
- start/end tags;
- lowercasing tag names;
- quoted attributes;
- unquoted attributes;
- lowercasing attribute names;
- preservation attribute values;
- self-closing syntax;
- HTML whitespace между attributes;
- attribute lookup by name.

Тесты преимущественно проверяют semantic output:

```text
DOM
Geometry
DisplayList
```

а не pixels конкретного graphics backend.

---

# Явные ограничения текущего milestone

Следующие вещи могут быть технически близки к существующему коду,
но не считаются поддерживаемыми без отдельного контракта и тестов:

- произвольный malformed HTML;
- arbitrary HTML elements;
- полноценный DOM;
- CSS parsing;
- cascade;
- полноценный text layout;
- intrinsic sizing;
- scrolling;
- asynchronous resources;
- browser lifecycle;
- JavaScript.

---

# Текущий frontier

Текущий вертикальный slice уже проходит через:

```text
HTML
→ DOM-like tree
→ block/text/image layout
→ PhysicalFragment tree
→ Painter
→ DisplayList
→ SDL
```

Поэтому следующий архитектурный вопрос находится уже не внутри отдельной
drawing primitive.

Требуется определить границу:

```text
DOM
  ↓
Style resolution
  ↓
Layout tree / layout input
  ↓
Geometry
```

В частности необходимо исследовать:

- должен ли layout работать непосредственно с DOM nodes;
- где должен находиться resolved/computed Style;
- должен ли каждый DOM element обязательно иметь layout representation;
- какой объект является root layout;
- как соотносятся Document, DOM root и layout root;
- как обрабатывать элементы, которые существуют в DOM, но не создают box;
- где проходит граница между HTML semantics и layout semantics.

До определения этой границы не следует автоматически расширять поддержку
до `<html>`, `<body>` или CSS parser только потому, что эти возможности
являются следующими в Web Platform.

---

# Политика поддержки

Функциональность считается поддерживаемой только если:

1. определён её минимальный semantic contract;
2. реализация обеспечивает этот contract;
3. поведение имеет regression/conformance coverage.

Если функциональность случайно работает, но её поведение не определено
и не проверяется тестами, она не считается поддерживаемой.

Отсутствие неподдерживаемой функциональности не является багом.

Багом является поведение, нарушающее явно заявленную поддерживаемую
семантику.

---

# Принцип развития

VoidEngine развивается небольшими вертикальными slices.

Для новой возможности предпочтительный порядок:

```text
Capability
  ↓
Contract
  ↓
Implementation
  ↓
Test
  ↓
Visible result
```
