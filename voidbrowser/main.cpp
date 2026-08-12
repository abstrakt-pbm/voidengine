#include "document/div.h"
#include "document/documentpainter.h"
#include "document/style.h"
#include "document/textelement.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <memory>
#include <type_traits>
#include <variant>
#include <vector>

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

constexpr float kFontSize = 16.0f;

void DrawBorder(SDL_Renderer *renderer,
                const ve::webplatform::DrawBorderCommand &command) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  const float border_width = command.border_width;

  // Top.
  SDL_FRect top{
      .x = command.x,
      .y = command.y,
      .w = command.width,
      .h = border_width,
  };

  // Bottom.
  SDL_FRect bottom{
      .x = command.x,
      .y = command.y + command.height - border_width,
      .w = command.width,
      .h = border_width,
  };

  // Left.
  SDL_FRect left{
      .x = command.x,
      .y = command.y + border_width,
      .w = border_width,
      .h = command.height - 2.0f * border_width,
  };

  // Right.
  SDL_FRect right{
      .x = command.x + command.width - border_width,
      .y = command.y + border_width,
      .w = border_width,
      .h = command.height - 2.0f * border_width,
  };

  SDL_RenderFillRect(renderer, &top);
  SDL_RenderFillRect(renderer, &bottom);
  SDL_RenderFillRect(renderer, &left);
  SDL_RenderFillRect(renderer, &right);
}

void DrawText(SDL_Renderer *renderer, TTF_Font *font,
              const ve::webplatform::DrawTextCommand &command) {
  SDL_Color colour{
      .r = 0,
      .g = 0,
      .b = 0,
      .a = 255,
  };

  SDL_Surface *surface = TTF_RenderText_Blended(font, command.text.c_str(),
                                                command.text.size(), colour);

  if (!surface) {
    SDL_Log("TTF_RenderText_Blended failed: %s", SDL_GetError());
    return;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  if (!texture) {
    SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
    SDL_DestroySurface(surface);
    return;
  }

  const float ascent = static_cast<float>(TTF_GetFontAscent(font));

  SDL_FRect destination{
      .x = command.x,
      .y = command.baseline_y - ascent,
      .w = static_cast<float>(surface->w),
      .h = static_cast<float>(surface->h),
  };

  SDL_RenderTexture(renderer, texture, nullptr, &destination);

  SDL_DestroyTexture(texture);
  SDL_DestroySurface(surface);
}

} // namespace

int main(int argc, char **argv) {
  //
  // Пока VoidEngine использует один font face.
  //
  // ./voidbrowser /path/to/font.ttf
  //

  if (argc < 2) {
    std::cerr << "Usage: voidbrowser <font.ttf>" << std::endl;
    return 1;
  }

  const char *font_path = argv[1];

  //
  // SDL.
  //

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init failed: %s", SDL_GetError());
    return 1;
  }

  //
  // SDL_ttf.
  //

  if (!TTF_Init()) {
    SDL_Log("TTF_Init failed: %s", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  //
  // Пока один font face и один размер на весь VoidEngine.
  //

  TTF_Font *font = TTF_OpenFont(font_path, kFontSize);

  if (!font) {
    SDL_Log("TTF_OpenFont failed: %s", SDL_GetError());

    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  //
  // Создаём DOM.
  //

  ve::webplatform::PainterEngine painter_engine;

  //
  // Root.
  //

  auto root_styles =
      ve::webplatform::Style(300, 180, ve::webplatform::Style::Colour::RED);

  root_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  root_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  root_styles.overflow_ = ve::webplatform::Style::Overflow::VISIBLE;

  ve::webplatform::Div root_div(root_styles);

  //
  // Text.
  //

  auto text = std::make_unique<ve::webplatform::TextElement>();

  text->data = "Hello VoidEngine";
  text->font_size = kFontSize;

  //
  // Пока font metrics вычисляет embedder и записывает их
  // непосредственно в TextElement.
  //
  // Layout и rasterization используют один и тот же TTF_Font.
  //

  int text_width = 0;
  int text_height = 0;

  if (!TTF_GetStringSize(font, text->data.c_str(), text->data.size(),
                         &text_width, &text_height)) {
    SDL_Log("TTF_GetStringSize failed: %s", SDL_GetError());

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  text->text_width = static_cast<float>(text_width);

  text->text_height = static_cast<float>(text_height);

  text->font_ascent = static_cast<float>(TTF_GetFontAscent(font));

  //
  // SDL_ttf возвращает descent отрицательным.
  // В VoidEngine храним положительное расстояние вниз от baseline.
  //

  text->font_descent = static_cast<float>(-TTF_GetFontDescent(font));

  root_div.AddChild(std::move(text));

  //
  // Первый child.
  //

  auto child_1_styles =
      ve::webplatform::Style(120, 60, ve::webplatform::Style::Colour::GREEN);

  child_1_styles.SetMargin(ve::webplatform::Margin(10.0f, 0.0f, 10.0f, 10.0f));

  child_1_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  child_1_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  child_1_styles.overflow_ = ve::webplatform::Style::Overflow::HIDDEN;

  auto child_1 = std::make_unique<ve::webplatform::Div>(child_1_styles);

  //
  // Grandchild.
  //

  auto grandchild_styles =
      ve::webplatform::Style(300, 100, ve::webplatform::Style::Colour::BLUE);

  grandchild_styles.height_mode_ = ve::webplatform::Style::HeightMode::FIXED;

  grandchild_styles.width_mode_ = ve::webplatform::Style::WidthMode::FIXED;

  child_1->AddChild(std::make_unique<ve::webplatform::Div>(grandchild_styles));

  //
  // Второй child.
  //

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

  //
  // Layout + Paint.
  //
  // Страница статическая, поэтому пока считаем display list один раз.
  //

  auto command_list = painter_engine.Paint(divs);

  //
  // Window + renderer.
  //

  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  if (!SDL_CreateWindowAndRenderer("VoidEngine", kWindowWidth, kWindowHeight, 0,
                                   &window, &renderer)) {
    SDL_Log("SDL_CreateWindowAndRenderer failed: %s", SDL_GetError());

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  //
  // Event/render loop.
  //

  bool running = true;

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        running = false;
      }
    }

    //
    // White background.
    //

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderClear(renderer);

    //
    // Display list.
    //

    for (const auto &rendering_command : command_list) {
      std::visit(
          [&](const auto &command) {
            using Command = std::decay_t<decltype(command)>;

            if constexpr (std::is_same_v<Command,
                                         ve::webplatform::FillRectCommand>) {
              SDL_SetRenderDrawColor(renderer, command.r, command.g, command.b,
                                     255);

              SDL_FRect rect{
                  .x = command.x,
                  .y = command.y,
                  .w = command.width,
                  .h = command.height,
              };

              SDL_RenderFillRect(renderer, &rect);
            }

            else if constexpr (std::is_same_v<
                                   Command,
                                   ve::webplatform::DrawBorderCommand>) {
              DrawBorder(renderer, command);
            }

            else if constexpr (std::is_same_v<Command,
                                              ve::webplatform::ClipCommand>) {
              SDL_Rect clip_rect{
                  .x = static_cast<int>(command.x),
                  .y = static_cast<int>(command.y),
                  .w = static_cast<int>(command.width),
                  .h = static_cast<int>(command.height),
              };

              SDL_SetRenderClipRect(renderer, &clip_rect);
            }

            else if constexpr (std::is_same_v<
                                   Command,
                                   ve::webplatform::ResetClipCommand>) {
              SDL_SetRenderClipRect(renderer, nullptr);
            }

            else if constexpr (std::is_same_v<
                                   Command, ve::webplatform::DrawTextCommand>) {
              DrawText(renderer, font, command);
            }
          },
          rendering_command);
    }

    SDL_RenderPresent(renderer);
  }

  //
  // Cleanup.
  //

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_CloseFont(font);
  TTF_Quit();

  SDL_Quit();

  return 0;
}
