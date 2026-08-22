#include <gtest/gtest.h>

#include <memory>

#include "document/div.h"
#include "document/documentpainter.h"
#include "document/style.h"
#include "document/textelement.h"

namespace ve {
namespace webplatform {

TEST(TextLayoutTest, TextAdvancesVerticalFlow) {
  Style root_style(200.0f, 200.0f, Style::Colour::RED);
  Div root(root_style);

  //
  // Text.
  //
  // Метрики задаём вручную, потому что этот тест проверяет layout,
  // а не SDL_ttf и не измерение шрифта.
  //

  auto text = std::make_unique<TextElement>();

  text->data = "Hello";

  text->font_size = 16.0f;

  text->text_width = 40.0f;
  text->text_height = 20.0f;

  text->font_ascent = 15.0f;
  text->font_descent = 5.0f;

  root.AddChild(std::move(text));

  //
  // Block after text.
  //

  Style child_style(100.0f, 50.0f, Style::Colour::GREEN);

  // left = 0, right = 0, top = 10, bottom = 0
  child_style.SetMargin(Margin(0.0f, 0.0f, 10.0f, 0.0f));

  root.AddChild(std::make_unique<Div>(child_style));

  GeometryEngine geometry_engine;

  GeometryConstraints constrains = {.max_width = 720.0f};
  auto fragment = geometry_engine.CalculateElementGeometry(root, constrains);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 2);

  const auto &text_fragment = fragment->child_fragments_[0];
  const auto &block_fragment = fragment->child_fragments_[1];

  //
  // Text starts at the beginning of the parent's content area.
  //

  EXPECT_FLOAT_EQ(text_fragment->x_, 0.0f);
  EXPECT_FLOAT_EQ(text_fragment->y_, 0.0f);

  EXPECT_FLOAT_EQ(text_fragment->width_, 40.0f);
  EXPECT_FLOAT_EQ(text_fragment->height_, 20.0f);

  //
  // Text participates in vertical flow:
  //
  // text.y      = 0
  // text.height = 20
  //
  // cursor = 0 + 20 = 20
  //
  // block margin-top = 10
  //
  // block.y = 20 + 10 = 30
  //

  EXPECT_FLOAT_EQ(block_fragment->y_, 30.0f);
}

TEST(TextLayoutTest, TextContributesToAutoParentHeight) {
  Style root_style(200.0f, 0.0f, Style::Colour::RED);

  root_style.width_mode_ = Style::WidthMode::FIXED;
  root_style.height_mode_ = Style::HeightMode::AUTO;

  Div root(root_style);

  // Метрики задаём вручную: этот тест проверяет layout,
  // а не SDL_ttf.
  auto text = std::make_unique<TextElement>();

  text->data = "Hello";
  text->font_size = 16.0f;

  text->text_width = 40.0f;
  text->text_height = 20.0f;

  text->font_ascent = 15.0f;
  text->font_descent = 5.0f;

  root.AddChild(std::move(text));

  GeometryEngine geometry_engine;

  GeometryConstraints constraints = {
      .max_width = 720.0f,
  };

  auto fragment = geometry_engine.CalculateElementGeometry(root, constraints);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 1);

  const auto &text_fragment = fragment->child_fragments_[0];

  // Текст действительно имеет заданную layout-высоту.
  EXPECT_FLOAT_EQ(text_fragment->y_, 0.0f);
  EXPECT_FLOAT_EQ(text_fragment->height_, 20.0f);

  // height:auto родителя должна включать высоту текста.
  EXPECT_FLOAT_EQ(fragment->height_, 20.0f);
}

TEST(TextLayoutTest, WrapsTextByAvailableWidth) {
  TextElement text;

  text.data = "ABCDEFGH";

  text.font_size = 16.0f;
  text.font_ascent = 15.0f;
  text.font_descent = 5.0f;

  // Один символ занимает 10 px.
  text.glyph_advance = 10.0f;

  GeometryConstraints constraints = {
      .max_width = 30.0f,
  };

  GeometryEngine geometry_engine;

  auto fragment = geometry_engine.CalculateTextGeometry(text, constraints);

  ASSERT_NE(fragment, nullptr);

  auto *text_fragment = dynamic_cast<TextPhysicalFragment *>(fragment.get());

  ASSERT_NE(text_fragment, nullptr);

  //
  // В 30 px помещается 3 символа:
  //
  // ABC
  // DEF
  // GH
  //

  ASSERT_EQ(text_fragment->text_lines_.size(), 3);

  EXPECT_EQ(text_fragment->text_lines_[0].payload_, "ABC");
  EXPECT_EQ(text_fragment->text_lines_[1].payload_, "DEF");
  EXPECT_EQ(text_fragment->text_lines_[2].payload_, "GH");

  //
  // line-height = ascent + descent = 20.
  //

  EXPECT_FLOAT_EQ(text_fragment->text_lines_[0].y_, 0.0f);
  EXPECT_FLOAT_EQ(text_fragment->text_lines_[1].y_, 20.0f);
  EXPECT_FLOAT_EQ(text_fragment->text_lines_[2].y_, 40.0f);

  //
  // Первые две строки занимают всю доступную ширину.
  // Последняя содержит только два символа.
  //

  EXPECT_FLOAT_EQ(text_fragment->text_lines_[0].width_, 30.0f);
  EXPECT_FLOAT_EQ(text_fragment->text_lines_[1].width_, 30.0f);
  EXPECT_FLOAT_EQ(text_fragment->text_lines_[2].width_, 20.0f);

  //
  // Общая высота TextPhysicalFragment:
  // 3 строки * 20 px = 60 px.
  //

  EXPECT_FLOAT_EQ(text_fragment->height_, 60.0f);
}

TEST(TextPaintTest, NestedTextUsesGlobalCoordinates) {
  Style root_style(300.0f, 200.0f, Style::Colour::RED);
  root_style.border_width = 2.0f;
  root_style.SetPadding(Padding(5.0f, // left
                                0.0f, // right
                                7.0f, // top
                                0.0f  // bottom
                                ));

  Div root(root_style);

  Style child_style(200.0f, 100.0f, Style::Colour::GREEN);
  child_style.border_width = 3.0f;

  child_style.SetMargin(Margin(11.0f, // left
                               0.0f,  // right
                               13.0f, // top
                               0.0f   // bottom
                               ));

  child_style.SetPadding(Padding(17.0f, // left
                                 0.0f,  // right
                                 19.0f, // top
                                 0.0f   // bottom
                                 ));

  auto child = std::make_unique<Div>(child_style);

  auto text = std::make_unique<TextElement>();
  text->data = "Hello";
  text->font_size = 16.0f;
  text->font_ascent = 15.0f;
  text->font_descent = 5.0f;
  text->glyph_advance = 8.0f;

  child->AddChild(std::move(text));
  root.AddChild(std::move(child));

  GeometryEngine geometry_engine;
  PainterEngine painter;

  auto geometry = geometry_engine.CalculateDocumentGeometry(root);
  ASSERT_NE(geometry, nullptr);

  DisplayList commands = painter.Paint(*geometry);

  const DrawTextCommand *draw_text = nullptr;

  for (const auto &command : commands) {
    if (const auto *current = std::get_if<DrawTextCommand>(&command)) {
      draw_text = current;
      break;
    }
  }

  ASSERT_NE(draw_text, nullptr);

  // Root content origin:
  //
  // x = border 2 + padding-left 5 = 7
  // y = border 2 + padding-top  7 = 9
  //
  // Child origin:
  //
  // x = 7 + margin-left 11 = 18
  // y = 9 + margin-top  13 = 22
  //
  // Text origin inside child:
  //
  // x = 18 + border 3 + padding-left 17 = 38
  // y = 22 + border 3 + padding-top  19 = 44
  //
  // Text baseline:
  //
  // 44 + ascent 15 = 59

  EXPECT_FLOAT_EQ(draw_text->x, 38.0f);
  EXPECT_FLOAT_EQ(draw_text->baseline_y, 59.0f);
  EXPECT_EQ(draw_text->text, "Hello");
}

} // namespace webplatform
} // namespace ve
