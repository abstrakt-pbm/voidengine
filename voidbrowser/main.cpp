#include <document/div.h>
#include <document/documentpainter.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>
#include <vector>

// voidbrowser
int main(int argc, char **argv) {
  ve::webplatform::PainterEngine painter_engine;

  ve::webplatform::Div root_div(100, 50, ve::webplatform::Div::Colour::RED);
  ve::webplatform::Div child_div(50, 25, ve::webplatform::Div::Colour::GREEN);
  root_div.AddChild(&child_div);

  std::vector<ve::webplatform::Div> divs = {root_div};

  std::vector<ve::webplatform::FillRectCommand> command_list =
      painter_engine.Paint(divs);

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

      SDL_FRect rect{.x = render_command.x,
                     .y = render_command.y,
                     .w = render_command.width,
                     .h = render_command.height};

      std::cout << render_command.g << std::endl;
      SDL_SetRenderDrawColor(renderer, render_command.r, render_command.g,
                             render_command.b, 255);

      SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
