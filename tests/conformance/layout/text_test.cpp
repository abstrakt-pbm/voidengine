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

  PainterEngine painter;

  GeometryConstraints constrains = {.max_width = 720.0f};
  auto fragment = painter.CalculateElementGeometry(root, constrains);

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

} // namespace webplatform
} // namespace ve
