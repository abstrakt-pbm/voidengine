#include <gtest/gtest.h>

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
  GeometryConstraints constrains = {.max_width = 720.0f};
  auto fragment = painter.CalculateElementGeometry(&root, constrains);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 1);

  const auto &child_fragment = fragment->child_fragments_[0];

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

  GeometryConstraints constrains = {.max_width = 720.0f};
  auto fragment = painter.CalculateElementGeometry(&root, constrains);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 2);

  const auto &first = fragment->child_fragments_[0];
  const auto &second = fragment->child_fragments_[1];

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
  GeometryConstraints constrains = {.max_width = 720.0f};
  auto fragment = painter.CalculateElementGeometry(&root, constrains);

  ASSERT_NE(fragment, nullptr);
  ASSERT_EQ(fragment->child_fragments_.size(), 1);

  const auto &child = fragment->child_fragments_[0];

  EXPECT_FLOAT_EQ(child->x_, 1.0f + 10.0f + 6.0f);
  EXPECT_FLOAT_EQ(child->y_, 1.0f + 10.0f + 4.0f);
}

} // namespace webplatform
} // namespace ve
