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

  std::vector<Div> divs;
  divs.push_back(std::move(root));

  PainterEngine painter;
  DisplayList commands = painter.Paint(divs);

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

  std::vector<Div> divs;
  divs.push_back(std::move(root));

  PainterEngine painter;
  DisplayList commands = painter.Paint(divs);

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

} // namespace webplatform
} // namespace ve
