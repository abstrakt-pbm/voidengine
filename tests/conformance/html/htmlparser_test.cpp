#include "document/div.h"
#include "document/imageelement.h"
#include "html/htmlparser.h"

#include <gtest/gtest.h>

namespace ve::html {

TEST(HTMLParserTest, ParsesImageAsVoidElement) {
  const std::string html =
      "<div>"
      "<img src=\"/tmp/test.png\" width=\"160\" height=\"100\">"
      "<div></div>"
      "</div>";

  auto root = ParseHTML(html);

  ASSERT_NE(root, nullptr);

  auto *root_div = dynamic_cast<webplatform::Div *>(root.get());

  ASSERT_NE(root_div, nullptr);

  //
  // Root должен содержать:
  //
  // Div
  // ├── Image
  // └── Div
  //
  // То есть второй Div должен быть sibling изображения,
  // а не его child.
  //

  ASSERT_EQ(root_div->childs_.size(), 2);

  auto *image =
      dynamic_cast<webplatform::ImageElement *>(root_div->childs_[0].get());

  auto *sibling_div =
      dynamic_cast<webplatform::Div *>(root_div->childs_[1].get());

  ASSERT_NE(image, nullptr);
  ASSERT_NE(sibling_div, nullptr);

  //
  // Проверяем атрибуты <img>.
  //

  EXPECT_EQ(image->path_to_img_, "/tmp/test.png");
  EXPECT_FLOAT_EQ(image->widht_, 160.0f);
  EXPECT_FLOAT_EQ(image->height_, 100.0f);

  //
  // <img> — void element.
  // Следующий <div> не должен стать его ребёнком.
  //

  EXPECT_TRUE(image->childs_.empty());
}

} // namespace ve::html
