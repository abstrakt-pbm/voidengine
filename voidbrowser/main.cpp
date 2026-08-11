#include "document/style.h"
#include "document/textelement.h"
#include <document/div.h>
#include <document/documentpainter.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>
#include <memory>
#include <variant>
#include <vector>

// voidbrowser
int main(int argc, char **argv) {
  ve::webplatform::PainterEngine painter_engine;

  // Root: большой внешний clip.
  auto root_styles =
      ve::webplatform::Style(300, 180, ve::webplatform::Style::Colour::RED);

  root_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;
  root_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;
  root_styles.overflow_ = ve::webplatform::Style::Overflow::VISIBLE;

  ve::webplatform::Div root_div(root_styles);
  ve::webplatform::TextElement text_elem;
  root_div.AddChild(std::make_unique<ve::webplatform::TextElement>());
  // Первый child — маленький внутренний clip.
  auto child_1_styles =
      ve::webplatform::Style(120, 60, ve::webplatform::Style::Colour::GREEN);

  child_1_styles.SetMargin(ve::webplatform::Margin(10.0f, 0.0f, 10.0f, 10.0f));

  child_1_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  child_1_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  child_1_styles.overflow_ = ve::webplatform::Style::Overflow::HIDDEN;

  auto child_1 = std::make_unique<ve::webplatform::Div>(child_1_styles);

  // Огромный grandchild.
  // Он должен быть обрезан по child_1, а не только по root.
  auto grandchild_styles =
      ve::webplatform::Style(300, 100, ve::webplatform::Style::Colour::BLUE);

  grandchild_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  grandchild_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  child_1->AddChild(std::make_unique<ve::webplatform::Div>(grandchild_styles));

  // Второй child — широкий.
  // Он нужен именно для проверки восстановления root clip после pop child_1.
  auto child_2_styles =
      ve::webplatform::Style(500, 60, ve::webplatform::Style::Colour::GREEN);

  child_2_styles.SetMargin(ve::webplatform::Margin(10.0f, 0.0f, 10.0f, 0.0f));

  child_2_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  child_2_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  auto child_2 = std::make_unique<ve::webplatform::Div>(child_2_styles);

  root_div.AddChild(std::move(child_1));
  root_div.AddChild(std::move(child_2));

  std::vector<ve::webplatform::Div> divs;
  divs.push_back(std::move(root_div));

  ve::webplatform::DisplayList command_list = painter_engine.Paint(divs);

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("VoidEngine", 1280, 720, 0);

  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  bool running = true;

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (const auto &render_command : command_list) {
      std::visit(
          [&](const auto &command) {
            using Command = std::decay_t<decltype(command)>;

            if constexpr (std::is_same_v<Command,
                                         ve::webplatform::FillRectCommand>) {

              SDL_FRect rect{
                  .x = command.x,
                  .y = command.y,
                  .w = command.width,
                  .h = command.height,
              };

              SDL_SetRenderDrawColor(renderer, command.r, command.g, command.b,
                                     255);

              SDL_RenderFillRect(renderer, &rect);

            } else if constexpr (std::is_same_v<
                                     Command,
                                     ve::webplatform::DrawBorderCommand>) {

              SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

              const float border_width = command.border_width;

              SDL_FRect top{
                  .x = command.x,
                  .y = command.y,
                  .w = command.width,
                  .h = border_width,
              };

              SDL_FRect bottom{
                  .x = command.x,
                  .y = command.y + command.height - border_width,
                  .w = command.width,
                  .h = border_width,
              };

              SDL_FRect left{
                  .x = command.x,
                  .y = command.y,
                  .w = border_width,
                  .h = command.height,
              };

              SDL_FRect right{
                  .x = command.x + command.width - border_width,
                  .y = command.y,
                  .w = border_width,
                  .h = command.height,
              };

              SDL_RenderFillRect(renderer, &top);
              SDL_RenderFillRect(renderer, &bottom);
              SDL_RenderFillRect(renderer, &left);
              SDL_RenderFillRect(renderer, &right);

            } else if constexpr (std::is_same_v<Command,
                                                ve::webplatform::ClipCommand>) {

              SDL_Rect clip_rect{
                  .x = static_cast<int>(command.x),
                  .y = static_cast<int>(command.y),
                  .w = static_cast<int>(command.width),
                  .h = static_cast<int>(command.height),
              };

              SDL_SetRenderClipRect(renderer, &clip_rect);
            } else if constexpr (std::is_same_v<
                                     Command,
                                     ve::webplatform::ResetClipCommand>) {

              SDL_SetRenderClipRect(renderer, nullptr);
            }
          },
          render_command);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
