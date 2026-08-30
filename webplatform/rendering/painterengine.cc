#include "painterengine.h"

#include "document/div.h"
#include "document/physicalfragment.h"
#include "document/style.h"
#include "document/textelement.h"

namespace ve::webplatform {
DisplayList PainterEngine::Paint(const PhysicalFragment &fragment) {
  DisplayList commands;
  DisplayList dom_root_commmands = PaintFragment(fragment, 0, 0);

  commands.insert(commands.end(), dom_root_commmands.begin(),
                  dom_root_commmands.end());
  return commands;
}

DisplayList
PainterEngine::MakeDrawCommands(const BoxPhysicalFragment &fragment) {
  FillRectCommand render_command;
  DrawBorderCommand border_command;

  border_command.width = fragment.width_;
  border_command.height = fragment.height_;
  border_command.border_width = fragment.border_width_;

  render_command.width = fragment.width_ - 2 * fragment.border_width_;
  render_command.height = fragment.height_ - 2 * fragment.border_width_;

  render_command.r = fragment.r_;
  render_command.g = fragment.g_;
  render_command.b = fragment.b_;
  return DisplayList{border_command, render_command};
}

DisplayList PainterEngine::PaintFragment(const PhysicalFragment &fragment,
                                         float offset_x, float offset_y) {
  if (auto *text_elem = dynamic_cast<const TextPhysicalFragment *>(&fragment)) {
    return PaintText(*text_elem, offset_x, offset_y);
  } else if (auto *div_elem =
                 dynamic_cast<const BoxPhysicalFragment *>(&fragment)) {
    return PaintDiv(*div_elem, offset_x, offset_y);
  } else if (auto *img_elem =
                 dynamic_cast<const ImagePhysicalFragment *>(&fragment)) {
    return PaintImage(*img_elem, offset_x, offset_y);
  }
  return DisplayList();
}

DisplayList PainterEngine::PaintImage(const ImagePhysicalFragment &fragment,
                                      float offset_x, float offset_y) {

  DrawImageCommand command;

  command.x = offset_x + fragment.x_;

  command.y = offset_y + fragment.y_;

  command.width = fragment.width_;

  command.height = fragment.height_;

  command.path_to_png_img = fragment.path_to_img_;

  return DisplayList{command};
}

DisplayList PainterEngine::PaintDiv(const BoxPhysicalFragment &fragment,
                                    float offset_x, float offset_y) {
  DisplayList commands;

  float cursor_x = offset_x + fragment.x_;
  float cursor_y = offset_y + fragment.y_;

  if (fragment.overflow_ == BoxPhysicalFragment::Overflow::HIDDEN) {
    ClipCommand clip_command;
    clip_command.x = cursor_x + fragment.border_width_;
    clip_command.y = cursor_y + fragment.border_width_;
    clip_command.width = fragment.width_ - 2 * fragment.border_width_;
    clip_command.height = fragment.height_ - 2 * fragment.border_width_;

    ClipCommand result_command = clip_command;
    if (!clip_command_stack_.empty()) {
      result_command =
          IntersectClipCommands(clip_command_stack_.top(), clip_command);
    }

    clip_command_stack_.push(result_command);
    commands.push_back(result_command);
  }

  DisplayList fragment_comands = MakeDrawCommands(fragment);

  for (auto &render_command : fragment_comands) {
    std::visit(
        [&](auto &command) {
          using Command = std::decay_t<decltype(command)>;

          if constexpr (std::is_same_v<Command,
                                       ve::webplatform::FillRectCommand>) {
            command.x = cursor_x + fragment.border_width_;
            command.y = cursor_y + fragment.border_width_;
          } else if constexpr (std::is_same_v<
                                   Command,
                                   ve::webplatform::DrawBorderCommand>) {
            command.x = cursor_x;
            command.y = cursor_y;
          }
        },
        render_command);
  }

  commands.insert(commands.end(), fragment_comands.begin(),
                  fragment_comands.end());

  for (size_t i = 0; i < fragment.child_fragments_.size(); ++i) {
    auto child_fragment = fragment.child_fragments_[i].get();
    float child_cursor_x = cursor_x;
    float child_cursor_y = cursor_y;

    DisplayList child_commands =
        PaintFragment(*child_fragment, child_cursor_x, child_cursor_y);

    commands.insert(commands.end(), child_commands.begin(),
                    child_commands.end());
  }
  if (fragment.overflow_ == BoxPhysicalFragment::Overflow::HIDDEN) {
    clip_command_stack_.pop();
    if (clip_command_stack_.empty()) {
      commands.push_back(ResetClipCommand{});
    } else {
      commands.push_back(clip_command_stack_.top());
    }
  }
  return commands;
}

ClipCommand
PainterEngine::IntersectClipCommands(const ClipCommand &current_clip_command,
                                     const ClipCommand &new_clip_command) {
  const float left = std::max(current_clip_command.x, new_clip_command.x);

  const float top = std::max(current_clip_command.y, new_clip_command.y);

  const float right =
      std::min(current_clip_command.x + current_clip_command.width,
               new_clip_command.x + new_clip_command.width);

  const float bottom =
      std::min(current_clip_command.y + current_clip_command.height,
               new_clip_command.y + new_clip_command.height);

  ClipCommand command;
  command.x = left;
  command.y = top;
  command.width = std::max(0.0f, right - left);
  command.height = std::max(0.0f, bottom - top);

  return command;
}

DisplayList PainterEngine::PaintText(const TextPhysicalFragment &fragment,
                                     float offset_x, float offset_y) {

  DisplayList commands;

  const TextElement *text_element = fragment.owner_;

  if (text_element == nullptr) {
    return commands;
  }

  const float fragment_x = offset_x + fragment.x_;

  const float fragment_y = offset_y + fragment.y_;

  for (const auto &text_line : fragment.text_lines_) {
    DrawTextCommand command;

    command.x = fragment_x + text_line.x_;

    command.baseline_y = fragment_y + text_line.y_ + fragment.baseline_;

    command.font_size = text_element->font_size;

    command.text = text_line.payload_;

    commands.push_back(command);
  }

  return commands;
}
} // namespace ve::webplatform
