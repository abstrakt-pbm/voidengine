#include <gtest/gtest.h>

#include <memory>
#include <variant>
#include <vector>

#include "document/div.h"
#include "document/documentpainter.h"
#include "document/style.h"

namespace ve {
namespace webplatform {

TEST(OverflowTest, VisibleDoesNotCreateClipCommand) {
  Style root_style(150.0f, 50.0f, Style::Colour::RED);
  root_style.overflow_ = Style::Overflow::VISIBLE;

  Div root(root_style);

  Style child_style(500.0f, 200.0f, Style::Colour::GREEN);
  root.AddChild(std::make_unique<Div>(child_style));

  PainterEngine painter;
  GeometryEngine geometry_engine;
  DisplayList commands =
      painter.Paint(*geometry_engine.CalculateDocumentGeometry(root).get());

  bool has_clip_command = false;

  for (const auto &command : commands) {
    if (std::holds_alternative<ClipCommand>(command)) {
      has_clip_command = true;
    }
  }

  EXPECT_FALSE(has_clip_command);
}

TEST(OverflowTest, HiddenCreatesClipCommand) {
  Style root_style(150.0f, 50.0f, Style::Colour::RED);
  root_style.overflow_ = Style::Overflow::HIDDEN;

  Div root(root_style);

  Style child_style(500.0f, 200.0f, Style::Colour::GREEN);
  root.AddChild(std::make_unique<Div>(child_style));

  PainterEngine painter;
  GeometryEngine geometry_engine;
  DisplayList commands =
      painter.Paint(*geometry_engine.CalculateDocumentGeometry(root).get());

  const ClipCommand *clip = nullptr;

  for (const auto &command : commands) {
    if (const auto *current_clip = std::get_if<ClipCommand>(&command)) {
      clip = current_clip;
      break;
    }
  }

  ASSERT_NE(clip, nullptr);

  EXPECT_FLOAT_EQ(clip->x, 0.0f);
  EXPECT_FLOAT_EQ(clip->y, 0.0f);
  EXPECT_FLOAT_EQ(clip->width, 150.0f);
  EXPECT_FLOAT_EQ(clip->height, 50.0f);
}

TEST(OverflowTest, NestedClipRestoresParentClip) {
  Style root_style(300.0f, 180.0f, Style::Colour::RED);
  root_style.width_mode_ = Style::WidthMode::FIXED;
  root_style.height_mode_ = Style::HeightMode::FIXED;
  root_style.overflow_ = Style::Overflow::HIDDEN;

  Div root(root_style);

  Style child_style(120.0f, 60.0f, Style::Colour::GREEN);
  child_style.width_mode_ = Style::WidthMode::FIXED;
  child_style.height_mode_ = Style::HeightMode::FIXED;
  child_style.overflow_ = Style::Overflow::HIDDEN;

  // Margin(left, right, top, bottom)
  child_style.SetMargin(Margin(10.0f, 0.0f, 10.0f, 0.0f));

  auto child = std::make_unique<Div>(child_style);

  Style grandchild_style(300.0f, 100.0f, Style::Colour::BLUE);
  grandchild_style.width_mode_ = Style::WidthMode::FIXED;
  grandchild_style.height_mode_ = Style::HeightMode::FIXED;

  child->AddChild(std::make_unique<Div>(grandchild_style));

  // Нужен sibling после вложенного clip, чтобы Painter
  // восстановил root clip после выхода из child.
  Style sibling_style(500.0f, 60.0f, Style::Colour::GREEN);
  sibling_style.width_mode_ = Style::WidthMode::FIXED;
  sibling_style.height_mode_ = Style::HeightMode::FIXED;

  root.AddChild(std::move(child));
  root.AddChild(std::make_unique<Div>(sibling_style));

  PainterEngine painter;
  GeometryEngine geometry_engine;
  DisplayList commands =
      painter.Paint(*geometry_engine.CalculateDocumentGeometry(root).get());

  std::vector<const ClipCommand *> clip_commands;
  size_t reset_count = 0;

  for (const auto &command : commands) {
    if (const auto *clip = std::get_if<ClipCommand>(&command)) {
      clip_commands.push_back(clip);
    } else if (std::holds_alternative<ResetClipCommand>(command)) {
      ++reset_count;
    }
  }

  // Ожидаем:
  //
  // SetClip(root)
  // SetClip(root ∩ child)
  // SetClip(root)          <- восстановление после выхода из child
  // ResetClip              <- выход из root
  ASSERT_EQ(clip_commands.size(), 3);
  EXPECT_EQ(reset_count, 1);

  // Root clip.
  EXPECT_FLOAT_EQ(clip_commands[0]->x, 0.0f);
  EXPECT_FLOAT_EQ(clip_commands[0]->y, 0.0f);
  EXPECT_FLOAT_EQ(clip_commands[0]->width, 300.0f);
  EXPECT_FLOAT_EQ(clip_commands[0]->height, 180.0f);

  // Nested child clip.
  EXPECT_FLOAT_EQ(clip_commands[1]->x, 10.0f);
  EXPECT_FLOAT_EQ(clip_commands[1]->y, 10.0f);
  EXPECT_FLOAT_EQ(clip_commands[1]->width, 120.0f);
  EXPECT_FLOAT_EQ(clip_commands[1]->height, 60.0f);

  // После pop вложенного child должен восстановиться root clip.
  EXPECT_FLOAT_EQ(clip_commands[2]->x, 0.0f);
  EXPECT_FLOAT_EQ(clip_commands[2]->y, 0.0f);
  EXPECT_FLOAT_EQ(clip_commands[2]->width, 300.0f);
  EXPECT_FLOAT_EQ(clip_commands[2]->height, 180.0f);
}

TEST(OverflowTest, NestedClipIsIntersectedWithParentClip) {
  Style root_style(100.0f, 100.0f, Style::Colour::RED);
  root_style.width_mode_ = Style::WidthMode::FIXED;
  root_style.height_mode_ = Style::HeightMode::FIXED;
  root_style.overflow_ = Style::Overflow::HIDDEN;

  Div root(root_style);

  Style child_style(100.0f, 100.0f, Style::Colour::GREEN);
  child_style.width_mode_ = Style::WidthMode::FIXED;
  child_style.height_mode_ = Style::HeightMode::FIXED;
  child_style.overflow_ = Style::Overflow::HIDDEN;

  // Child начинается в (50, 50), поэтому половина его box
  // находится за пределами root.
  child_style.SetMargin(Margin(50.0f, 0.0f, 50.0f, 0.0f));

  root.AddChild(std::make_unique<Div>(child_style));

  PainterEngine painter;
  GeometryEngine geometry_engine;

  DisplayList commands =
      painter.Paint(*geometry_engine.CalculateDocumentGeometry(root).get());

  std::vector<const ClipCommand *> clip_commands;

  for (const auto &command : commands) {
    if (const auto *clip = std::get_if<ClipCommand>(&command)) {
      clip_commands.push_back(clip);
    }
  }

  ASSERT_GE(clip_commands.size(), 2);

  // Root:
  //
  // (0, 0) ───────────── (100, 0)
  //   │
  //   │
  //   │       child:
  //   │       (50,50) ───────────── (150,50)
  //   │          │
  // (0,100) ─────┼────── (100,100)
  //              │
  //              │
  //           (50,150)
  //
  // Пересечение root и child:
  //
  // x = 50
  // y = 50
  // width = 50
  // height = 50

  const ClipCommand &root_clip = *clip_commands[0];

  EXPECT_FLOAT_EQ(root_clip.x, 0.0f);
  EXPECT_FLOAT_EQ(root_clip.y, 0.0f);
  EXPECT_FLOAT_EQ(root_clip.width, 100.0f);
  EXPECT_FLOAT_EQ(root_clip.height, 100.0f);

  const ClipCommand &child_clip = *clip_commands[1];

  EXPECT_FLOAT_EQ(child_clip.x, 50.0f);
  EXPECT_FLOAT_EQ(child_clip.y, 50.0f);
  EXPECT_FLOAT_EQ(child_clip.width, 50.0f);
  EXPECT_FLOAT_EQ(child_clip.height, 50.0f);
}

TEST(OverflowTest, NonIntersectingNestedClipIsEmpty) {
  Style root_style(100.0f, 100.0f, Style::Colour::RED);
  root_style.overflow_ = Style::Overflow::HIDDEN;

  Div root(root_style);

  Style child_style(50.0f, 50.0f, Style::Colour::GREEN);
  child_style.overflow_ = Style::Overflow::HIDDEN;

  // Child полностью справа и снизу от root.
  child_style.SetMargin(Margin(150.0f, 0.0f, 150.0f, 0.0f));

  root.AddChild(std::make_unique<Div>(child_style));

  GeometryEngine geometry_engine;
  PainterEngine painter;

  DisplayList commands =
      painter.Paint(*geometry_engine.CalculateDocumentGeometry(root));

  std::vector<const ClipCommand *> clips;

  for (const auto &command : commands) {
    if (const auto *clip = std::get_if<ClipCommand>(&command)) {
      clips.push_back(clip);
    }
  }

  ASSERT_GE(clips.size(), 2);

  EXPECT_FLOAT_EQ(clips[1]->width, 0.0f);
  EXPECT_FLOAT_EQ(clips[1]->height, 0.0f);
}

} // namespace webplatform
} // namespace ve
