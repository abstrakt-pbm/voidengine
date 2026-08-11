#include <gtest/gtest.h>

#include <memory>

#include "document/div.h"
#include "document/documentpainter.h"
#include "document/style.h"

namespace ve {
namespace webplatform {

TEST(HeightTest, FixedHeightUsesStyleHeight) {
  Style style(100.0f, 50.0f, Style::Colour::RED);
  style.height_mode_ = Style::HeightMode::FIXED;

  Div div(style);

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(&div);

  ASSERT_NE(fragment, nullptr);

  EXPECT_FLOAT_EQ(fragment->height_, 50.0f);
}

TEST(HeightTest, AutoHeightIsCalculatedFromChildren) {
  Style root_style(100.0f, 50.0f, Style::Colour::RED);

  // Padding(left, right, top, bottom)
  root_style.SetPadding(Padding(0.0f, 0.0f, 10.0f, 5.0f));
  root_style.border_width = 1.0f;
  root_style.height_mode_ = Style::HeightMode::AUTO;

  Div root(root_style);

  Style first_style(50.0f, 20.0f, Style::Colour::GREEN);
  first_style.height_mode_ = Style::HeightMode::FIXED;

  // Margin(left, right, top, bottom)
  first_style.SetMargin(Margin(0.0f, 0.0f, 2.0f, 3.0f));

  Style second_style(50.0f, 30.0f, Style::Colour::BLUE);
  second_style.height_mode_ = Style::HeightMode::FIXED;
  second_style.SetMargin(Margin(0.0f, 0.0f, 4.0f, 6.0f));

  root.AddChild(std::make_unique<Div>(first_style));
  root.AddChild(std::make_unique<Div>(second_style));

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(&root);

  ASSERT_NE(fragment, nullptr);

  // border-top       1
  // padding-top     10
  // margin-top       2
  // first height    20
  // margin-bottom    3
  // margin-top       4
  // second height   30
  // margin-bottom    6
  // padding-bottom   5
  // border-bottom    1
  //
  // total = 82
  EXPECT_FLOAT_EQ(fragment->height_, 82.0f);
}

TEST(HeightTest, EmptyAutoHeightUsesPaddingAndBorder) {
  Style style(100.0f, 50.0f, Style::Colour::RED);

  style.SetPadding(Padding(0.0f, 0.0f, 3.0f, 5.0f));
  style.border_width = 1.0f;
  style.height_mode_ = Style::HeightMode::AUTO;

  Div div(style);

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(&div);

  ASSERT_NE(fragment, nullptr);

  // 1 border-top
  // 3 padding-top
  // 5 padding-bottom
  // 1 border-bottom
  EXPECT_FLOAT_EQ(fragment->height_, 10.0f);
}

} // namespace webplatform
} // namespace ve
