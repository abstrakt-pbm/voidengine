#include "document/div.h"
#include "document/documentpainter.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/style.h"

#include <gtest/gtest.h>

namespace ve::webplatform {

TEST(ImageLayoutTest, ImageAdvancesVerticalFlow) {
  //
  // Root.
  //

  Style root_style(300.0f, 0.0f, Style::Colour::RED);

  root_style.width_mode_ = Style::WidthMode::FIXED;

  root_style.height_mode_ = Style::HeightMode::AUTO;

  Div root(root_style);

  //
  // Image: 80x40.
  //

  root.AddChild(std::make_unique<ImageElement>(80.0f, 40.0f, "test.png"));

  //
  // Следующий block имеет margin-top = 10.
  //

  Style child_style(100.0f, 20.0f, Style::Colour::GREEN);

  child_style.width_mode_ = Style::WidthMode::FIXED;

  child_style.height_mode_ = Style::HeightMode::FIXED;

  child_style.SetMargin(Margin(0.0f,  // left
                               0.0f,  // right
                               10.0f, // top
                               0.0f   // bottom
                               ));

  root.AddChild(std::make_unique<Div>(child_style));

  //
  // Layout.
  //

  GeometryEngine geometry_engine;

  auto root_fragment = geometry_engine.CalculateDocumentGeometry(root);

  ASSERT_NE(root_fragment, nullptr);
  ASSERT_EQ(root_fragment->child_fragments_.size(), 2);

  const auto *image_fragment = dynamic_cast<const ImagePhysicalFragment *>(
      root_fragment->child_fragments_[0].get());

  const auto *div_fragment = dynamic_cast<const BoxPhysicalFragment *>(
      root_fragment->child_fragments_[1].get());

  ASSERT_NE(image_fragment, nullptr);
  ASSERT_NE(div_fragment, nullptr);

  //
  // Image начинается с y=0 и занимает 40 px.
  //
  // Следующий block:
  //
  // 40 image height
  // +10 margin-top
  // =50
  //

  EXPECT_FLOAT_EQ(image_fragment->y_, 0.0f);
  EXPECT_FLOAT_EQ(image_fragment->height_, 40.0f);

  EXPECT_FLOAT_EQ(div_fragment->y_, 50.0f);
}

TEST(ImageLayoutTest, ImageContributesToAutoParentHeight) {
  //
  // Root с height:auto.
  //

  Style root_style(300.0f, 0.0f, Style::Colour::RED);

  root_style.width_mode_ = Style::WidthMode::FIXED;

  root_style.height_mode_ = Style::HeightMode::AUTO;

  Div root(root_style);

  //
  // Единственный child — image высотой 40.
  //

  root.AddChild(std::make_unique<ImageElement>(80.0f, 40.0f, "test.png"));

  GeometryEngine geometry_engine;

  auto root_fragment = geometry_engine.CalculateDocumentGeometry(root);

  ASSERT_NE(root_fragment, nullptr);
  ASSERT_EQ(root_fragment->child_fragments_.size(), 1);

  const auto *image_fragment = dynamic_cast<const ImagePhysicalFragment *>(
      root_fragment->child_fragments_[0].get());

  ASSERT_NE(image_fragment, nullptr);

  EXPECT_FLOAT_EQ(image_fragment->width_, 80.0f);
  EXPECT_FLOAT_EQ(image_fragment->height_, 40.0f);

  //
  // Padding и border отсутствуют, поэтому
  // auto-height root равен высоте image.
  //

  EXPECT_FLOAT_EQ(root_fragment->height_, 40.0f);
}

} // namespace ve::webplatform
