#include "document/style.h"
#include "document/textelement.h"

#include <document/div.h>
#include <document/documentpainter.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <memory>
#include <variant>
#include <vector>

int main(int argc, char **argv) {
  //
  // Пока путь к шрифту передаём embedder-у извне.
  //
  // ./voidbrowser /path/to/font.ttf
  //
  if (argc < 2) {
    std::cerr << "Usage: voidbrowser <font.ttf>" << std::endl;
    return 1;
  }

  const char *font_path = argv[1];

  ve::webplatform::PainterEngine painter_engine;

  // Root.
  auto root_styles =
      ve::webplatform::Style(300, 180, ve::webplatform::Style::Colour::RED);

  root_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  root_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  root_styles.overflow_ = ve::webplatform::Style::Overflow::VISIBLE;

  ve::webplatform::Div root_div(root_styles);

  // Text.
  auto text = std::make_unique<ve::webplatform::TextElement>();

  text->data = "Hello VoidEngine";
  text->font_size = 16.0f;

  root_div.AddChild(std::move(text));

  // Первый child.
  auto child_1_styles =
      ve::webplatform::Style(120, 60, ve::webplatform::Style::Colour::GREEN);

  child_1_styles.SetMargin(ve::webplatform::Margin(10.0f, 0.0f, 10.0f, 10.0f));

  child_1_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  child_1_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  child_1_styles.overflow_ = ve::webplatform::Style::Overflow::HIDDEN;

  auto child_1 = std::make_unique<ve::webplatform::Div>(child_1_styles);

  // Grandchild.
  auto grandchild_styles =
      ve::webplatform::Style(300, 100, ve::webplatform::Style::Colour::BLUE);

  grandchild_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  grandchild_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  child_1->AddChild(std::make_unique<ve::webplatform::Div>(grandchild_styles));

  // Второй child.
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

  //
  // SDL
  //

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  //
  // SDL_ttf
  //

  if (!TTF_Init()) {
    SDL_Log("TTF_Init failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  //
  // Пока один font face для всего embedder-а.
  //
  TTF_Font *font = TTF_OpenFont(font_path, 16.0f);

  if (!font) {
    SDL_Log("TTF_OpenFont failed: %s", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }

  float current_font_size = 16.0f;

  SDL_Window *window = SDL_CreateWindow("VoidEngine", 1280, 720, 0);

  if (!window) {
    SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

  if (!renderer) {
    SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());

    SDL_DestroyWindow(window);

    TTF_CloseFont(font);
    TTF_Quit();
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

            //
            // FillRect
            //
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

              //
              // Border
              //
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

              //
              // Clip
              //
            } else if constexpr (std::is_same_v<Command,
                                                ve::webplatform::ClipCommand>) {

              SDL_Rect clip_rect{
                  .x = static_cast<int>(command.x),
                  .y = static_cast<int>(command.y),
                  .w = static_cast<int>(command.width),
                  .h = static_cast<int>(command.height),
              };

              SDL_SetRenderClipRect(renderer, &clip_rect);

              //
              // Reset clip
              //
            } else if constexpr (std::is_same_v<
                                     Command,
                                     ve::webplatform::ResetClipCommand>) {

              SDL_SetRenderClipRect(renderer, nullptr);

              //
              // Text
              //
            } else if constexpr (std::is_same_v<
                                     Command,
                                     ve::webplatform::DrawTextCommand>) {

              //
              // Пока переиспользуем один TTF_Font
              // и меняем его размер при необходимости.
              //
              if (command.font_size != current_font_size) {
                if (!TTF_SetFontSize(font, command.font_size)) {

                  SDL_Log("TTF_SetFontSize failed: %s", SDL_GetError());

                  return;
                }

                current_font_size = command.font_size;
              }

              SDL_Color color{
                  .r = 0,
                  .g = 0,
                  .b = 0,
                  .a = 255,
              };

              SDL_Surface *surface = TTF_RenderText_Blended(
                  font, command.text.c_str(), command.text.size(), color);

              if (!surface) {
                SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());

                return;
              }

              SDL_Texture *texture =
                  SDL_CreateTextureFromSurface(renderer, surface);

              if (!texture) {
                SDL_Log("SDL_CreateTextureFromSurface failed: %s",
                        SDL_GetError());

                SDL_DestroySurface(surface);
                return;
              }

              //
              // DrawTextCommand хранит baseline,
              // а SDL_RenderTexture принимает
              // верхний левый угол destination rect.
              //
              // Поэтому поднимаемся от baseline
              // на font ascent.
              //
              const float ascent = static_cast<float>(TTF_GetFontAscent(font));

              SDL_FRect text_rect{
                  .x = command.x,
                  .y = command.baseline_y - ascent,
                  .w = static_cast<float>(surface->w),
                  .h = static_cast<float>(surface->h),
              };

              SDL_RenderTexture(renderer, texture, nullptr, &text_rect);

              SDL_DestroyTexture(texture);
              SDL_DestroySurface(surface);
            }
          },
          render_command);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_CloseFont(font);
  TTF_Quit();

  SDL_Quit();

  return 0;
}
