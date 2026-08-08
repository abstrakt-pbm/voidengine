#include "document/style.h"
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
  auto root_styles =
      ve::webplatform::Style(150, 50, ve::webplatform::Style::Colour::RED);
  root_styles.SetPadding(ve::webplatform::Padding(10.0f, 0.0f, 10.0f, 0.0f));
  root_styles.border_width = 1.0f;
  ve::webplatform::Div root_div(root_styles);

  auto child_styles =
      ve::webplatform::Style(50, 20, ve::webplatform::Style::Colour::GREEN);

  child_styles.SetPadding(ve::webplatform::Padding(2.0f, 0.0f, 2.0f, 0.0f));
  auto child_2 = std::make_unique<ve::webplatform::Div>(child_styles);
  child_2->AddChild(std::make_unique<ve::webplatform::Div>(
      ve::webplatform::Style(20, 10, ve::webplatform::Style::Colour::BLUE)));
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
