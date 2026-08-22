#include "document/div.h"
#include "document/imageelement.h"
#include "document/textelement.h"
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

TEST(HTMLParserTest, PreservesNestedDivStructure) {
  const std::string html = "<div>"
                           "A"
                           "<div>"
                           "B"
                           "<div>C</div>"
                           "</div>"
                           "D"
                           "</div>";

  auto root = ParseHTML(html);

  ASSERT_NE(root, nullptr);

  auto *outer_div = dynamic_cast<webplatform::Div *>(root.get());
  ASSERT_NE(outer_div, nullptr);

  // Ожидаем:
  //
  // Div
  // ├── Text("A")
  // ├── Div
  // │   ├── Text("B")
  // │   └── Div
  // │       └── Text("C")
  // └── Text("D")
  //

  ASSERT_EQ(outer_div->childs_.size(), 3);

  auto *text_a =
      dynamic_cast<webplatform::TextElement *>(outer_div->childs_[0].get());

  auto *middle_div =
      dynamic_cast<webplatform::Div *>(outer_div->childs_[1].get());

  auto *text_d =
      dynamic_cast<webplatform::TextElement *>(outer_div->childs_[2].get());

  ASSERT_NE(text_a, nullptr);
  ASSERT_NE(middle_div, nullptr);
  ASSERT_NE(text_d, nullptr);

  EXPECT_EQ(text_a->data, "A");
  EXPECT_EQ(text_d->data, "D");

  ASSERT_EQ(middle_div->childs_.size(), 2);

  auto *text_b =
      dynamic_cast<webplatform::TextElement *>(middle_div->childs_[0].get());

  auto *inner_div =
      dynamic_cast<webplatform::Div *>(middle_div->childs_[1].get());

  ASSERT_NE(text_b, nullptr);
  ASSERT_NE(inner_div, nullptr);

  EXPECT_EQ(text_b->data, "B");

  ASSERT_EQ(inner_div->childs_.size(), 1);

  auto *text_c =
      dynamic_cast<webplatform::TextElement *>(inner_div->childs_[0].get());

  ASSERT_NE(text_c, nullptr);
  EXPECT_EQ(text_c->data, "C");
}

} // namespace ve::html
