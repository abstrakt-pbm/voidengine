#pragma once

namespace ve {
namespace webplatform {
// итоговые стили одного конкретного элемента

class ContentBox {
public:
private:
  float width_;
  float height_;
  float min_width_;
  float max_width_;
  float min_height_;
  float max_height_;
};

class PaddingBox {};

class BorderBox {};

struct ComputedStyle {
  enum class BoxSizing {
    ContentBox,
    BorderBox,
  };

  float width;
  float height;

  float min_width;
  float max_width;
  float min_height;
  float max_height;

  float padding_top;
  float padding_botom;
  float padding_left;
  float padding_right;

  float border_top_width;
  float border_botom_width;
  float border_left_width;
  float border_right_width;

  float border_top_style;
  float border_botom_style;
  float border_left_style;
  float border_right_style;

  BoxSizing box_sizing;
};

} // namespace webplatform
} // namespace ve
