#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace Models {

struct Color {
  uint8_t red{};
  uint8_t green{};
  uint8_t blue{};
  uint8_t alpha{};
};

struct Point {
  Color color{};
  uint32_t x{};
  uint32_t y{};
};

struct Line {
  Color color{};
  uint16_t width{};
  Point firstPoint{};
  Point secondPoint{};
};

struct Polygon {
  std::vector<Point> points{};
  Color fillColor{};
  Color borderColor{};
  uint16_t borderWidth{};
};

struct Circle {
  Point center{};
  Color fillColor{};
  Color borderColor{};
  uint32_t radius{};
  uint16_t borderWidth{};
};

using GraphicalPrimitive = std::variant<Point, Line, Polygon, Circle>;
} // namespace Models