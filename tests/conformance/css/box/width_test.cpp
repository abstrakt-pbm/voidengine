#include <gtest/gtest.h>

#include <memory>

#include "document/div.h"
#include "document/documentpainter.h"
#include "document/style.h"

namespace ve {
namespace webplatform {

TEST(WidthTest, FixedWidthUsesStyleWidth) {
  Style style(100.0f, 50.0f, Style::Colour::RED);
  style.width_mode_ = Style::WidthMode::FIXED;

  Div div(style);

  GeometryConstraints constraints{
      .max_width = 500.0f,
  };

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(div, constraints);

  ASSERT_NE(fragment, nullptr);

  EXPECT_FLOAT_EQ(fragment->width_, 100.0f);
}

TEST(WidthTest, AutoWidthUsesAvailableWidth) {
  Style style(100.0f, 50.0f, Style::Colour::RED);
  style.width_mode_ = Style::WidthMode::AUTO;

  // Margin(left, right, top, bottom)
  style.SetMargin(Margin(10.0f, 20.0f, 0.0f, 0.0f));

  Div div(style);

  GeometryConstraints constraints{
      .max_width = 500.0f,
  };

  PainterEngine painter;
  auto fragment = painter.CalculateElementGeometry(div, constraints);

  ASSERT_NE(fragment, nullptr);

  // available width = 500
  // margin-left     = 10
  // margin-right    = 20
  //
  // border-box width = 470
  EXPECT_FLOAT_EQ(fragment->width_, 470.0f);
}

TEST(WidthTest, AutoChildUsesParentContentWidth) {
  Style parent_style(200.0f, 50.0f, Style::Colour::RED);
  parent_style.width_mode_ = Style::WidthMode::FIXED;

  // Padding(left, right, top, bottom)
  parent_style.SetPadding(Padding(10.0f, 20.0f, 0.0f, 0.0f));

  parent_style.border_width = 2.0f;

  Div parent(parent_style);

  Style child_style(50.0f, 20.0f, Style::Colour::GREEN);
  child_style.width_mode_ = Style::WidthMode::AUTO;

  child_style.SetMargin(Margin(5.0f, 7.0f, 0.0f, 0.0f));

  parent.AddChild(std::make_unique<Div>(child_style));

  GeometryConstraints constraints{
      .max_width = 500.0f,
  };

  PainterEngine painter;
  auto parent_fragment = painter.CalculateElementGeometry(parent, constraints);

  ASSERT_NE(parent_fragment, nullptr);
  ASSERT_EQ(parent_fragment->child_fragments_.size(), 1);

  const auto &child_fragment = parent_fragment->child_fragments_[0];

  // Parent content width:
  //
  // 200
  // - padding-left  10
  // - padding-right 20
  // - borders        4
  // = 166
  //
  // Child auto width:
  //
  // 166
  // - margin-left 5
  // - margin-right 7
  // = 154
  EXPECT_FLOAT_EQ(child_fragment->width_, 154.0f);

  // Child starts after parent's border + padding + its margin-left:
  //
  // 2 + 10 + 5 = 17
  EXPECT_FLOAT_EQ(child_fragment->x_, 17.0f);
}

} // namespace webplatform
} // namespace ve
