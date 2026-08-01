#pragma once

#include <string>
#include <variant>
#include <vector>

namespace ve {
namespace webplatform {
// прдеставляет список команд отрисовки
// Эмбедер получает DisplayList и последовательно исполняет команды отрисовки
//
struct Colour {};
struct Rect {};
struct Point {};
struct Font {};

struct FillRectCommand {
  Rect rect;
  Colour colour;
};

struct DrawText {
  Point base_line;
  std::string text;
  Colour colour;
  Font font;
};

using DrawCommant = std::variant<FillRectCommand, DrawText>;

class DisplayList {
public:
  void AddCommand(DrawCommant command);
  std::vector<DrawCommant> Commands();

private:
  std::vector<DrawCommant> commands_;
};
} // namespace webplatform
} // namespace ve
