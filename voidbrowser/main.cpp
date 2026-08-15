#include "document/documentpainter.h"
#include "document/imageelement.h"
#include "document/physicalfragment.h"
#include "document/textelement.h"
#include "html/htmlparser.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

constexpr float kFontSize = 16.0f;

//
// File loading.
//

std::string ReadFile(const std::string &path) {
  std::ifstream file(path);

  if (!file) {
    return {};
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  return buffer.str();
}

//
// Пока в VoidEngine нет собственного font subsystem.
//
// После HTML parsing рекурсивно проходим DOM и
// записываем font metrics во все TextElement.
//
// Layout и rasterization используют один TTF_Font.
//

void PrepareTextMetrics(ve::webplatform::DomNode &node, TTF_Font *font) {
  if (auto *text = dynamic_cast<ve::webplatform::TextElement *>(&node)) {

    text->font_size = kFontSize;

    int text_width = 0;
    int text_height = 0;

    if (!TTF_GetStringSize(font, text->data.c_str(), text->data.size(),
                           &text_width, &text_height)) {

      SDL_Log("TTF_GetStringSize failed for '%s': %s", text->data.c_str(),
              SDL_GetError());

      return;
    }

    text->text_width = static_cast<float>(text_width);

    text->text_height = static_cast<float>(text_height);

    text->font_ascent = static_cast<float>(TTF_GetFontAscent(font));

    //
    // SDL_ttf возвращает descent отрицательным.
    // В VoidEngine descent хранится как положительное
    // расстояние вниз от baseline.
    //

    text->font_descent = static_cast<float>(-TTF_GetFontDescent(font));

    //
    // Текущий text layout предполагает monospace font
    // и один glyph advance на все символы.
    //

    int glyph_width = 0;
    int glyph_height = 0;

    if (!TTF_GetStringSize(font, "M", 1, &glyph_width, &glyph_height)) {

      SDL_Log("Failed to measure glyph advance: %s", SDL_GetError());

      return;
    }

    text->glyph_advance = static_cast<float>(glyph_width);
  }

  //
  // Рекурсивно подготавливаем все text nodes.
  //

  for (auto &child : node.childs_) {
    PrepareTextMetrics(*child, font);
  }
}

//
// SDL drawing.
//

void DrawBorder(SDL_Renderer *renderer,
                const ve::webplatform::DrawBorderCommand &command) {

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
      .y = command.y + border_width,

      .w = border_width,

      .h = command.height - 2.0f * border_width,
  };

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

void DrawImage(SDL_Renderer *renderer,
               const ve::webplatform::DrawImageCommand &command) {

  SDL_Texture *texture =
      IMG_LoadTexture(renderer, command.path_to_png_img.c_str());

  if (!texture) {
    SDL_Log("IMG_LoadTexture failed for '%s': %s",
            command.path_to_png_img.c_str(), SDL_GetError());

    return;
  }

  SDL_FRect destination{
      .x = command.x,
      .y = command.y,
      .w = command.width,
      .h = command.height,
  };

  SDL_RenderTexture(renderer, texture, nullptr, &destination);

  //
  // Пока texture cache отсутствует.
  // Изображение загружается каждый frame.
  //

  SDL_DestroyTexture(texture);
}

} // namespace

int main(int argc, char **argv) {
  //
  // Usage:
  //
  // ./voidbrowser <font.ttf> <page.html>
  //

  if (argc < 3) {
    std::cerr << "Usage: voidbrowser <font.ttf> <page.html>" << std::endl;

    return 1;
  }

  const std::string font_path = argv[1];
  const std::string html_path = argv[2];

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
  // Пока один font face и один font size
  // на весь документ.
  //

  TTF_Font *font = TTF_OpenFont(font_path.c_str(), kFontSize);

  if (!font) {
    SDL_Log("TTF_OpenFont failed: %s", SDL_GetError());

    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  //
  // ==========================================
  // HTML FILE
  // ==========================================
  //

  std::string raw_html = ReadFile(html_path);

  if (raw_html.empty()) {
    std::cerr << "Failed to read HTML file: " << html_path << std::endl;

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  //
  // ==========================================
  // HTML -> DOM
  // ==========================================
  //

  std::unique_ptr<ve::webplatform::DomNode> dom_root =
      ve::html::ParseHTML(raw_html);

  if (!dom_root) {
    std::cerr << "ParseHTML failed" << std::endl;

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  //
  // ==========================================
  // Temporary font preparation
  // ==========================================
  //
  // Потом это должно уйти из embedder-а
  // в font subsystem VoidEngine.
  //

  PrepareTextMetrics(*dom_root, font);

  //
  // ==========================================
  // DOM -> PhysicalFragment tree
  // ==========================================
  //

  ve::webplatform::GeometryEngine geometry_engine;

  auto root_geometry = geometry_engine.CalculateDocumentGeometry(*dom_root);

  if (!root_geometry) {
    std::cerr << "Geometry calculation failed" << std::endl;

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();

    return 1;
  }

  //
  // ==========================================
  // PhysicalFragment tree -> DisplayList
  // ==========================================
  //

  ve::webplatform::PainterEngine painter_engine;

  ve::webplatform::DisplayList command_list =
      painter_engine.Paint(*root_geometry);

  //
  // ==========================================
  // Window + renderer
  // ==========================================
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
  // ==========================================
  // Event/render loop
  // ==========================================
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
    // White viewport background.
    //

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderClear(renderer);

    //
    // Execute DisplayList.
    //

    for (const auto &rendering_command : command_list) {

      std::visit(
          [&](const auto &command) {
            using Command = std::decay_t<decltype(command)>;

            //
            // FillRect
            //

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

            //
            // Border
            //

            else if constexpr (std::is_same_v<
                                   Command,
                                   ve::webplatform::DrawBorderCommand>) {

              DrawBorder(renderer, command);
            }

            //
            // Clip
            //

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

            //
            // Reset clip
            //

            else if constexpr (std::is_same_v<
                                   Command,
                                   ve::webplatform::ResetClipCommand>) {

              SDL_SetRenderClipRect(renderer, nullptr);
            }

            //
            // Text
            //

            else if constexpr (std::is_same_v<
                                   Command, ve::webplatform::DrawTextCommand>) {

              DrawText(renderer, font, command);
            }

            //
            // Image
            //

            else if constexpr (std::is_same_v<
                                   Command,
                                   ve::webplatform::DrawImageCommand>) {

              DrawImage(renderer, command);
            }
          },
          rendering_command);
    }

    SDL_RenderPresent(renderer);
  }

  //
  // ==========================================
  // Cleanup
  // ==========================================
  //

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  TTF_CloseFont(font);
  TTF_Quit();

  SDL_Quit();

  return 0;
}
