/**
 * @file Drawing.h
 * @brief Models drawing
 * @version 0.1
 * @date 2021-12-19
 *
 * @copyright Copyright (c) 2021
 *
 */
#pragma once

#include <iostream>

#include "CanvasData.h"
#include "GraphicalPrimitives.h"

#include "Drawing/IPointDrawer.h"

namespace Models {

inline void DrawImpl(const Drawing::IPointDrawer &drawer, const Point &point) {
  std::cout << "Drawing point" << std::endl;
}

inline void DrawImpl(const Drawing::IPointDrawer &drawer, const Line &line) {
  std::cout << "Drawing line" << std::endl;
}

inline void DrawImpl(const Drawing::IPointDrawer &drawer,
                     const Circle &circle) {
  std::cout << "Drawing circle" << std::endl;
}

inline void DrawImpl(const Drawing::IPointDrawer &drawer,
                     const Polygon &polygon) {
  std::cout << "Drawing polygon" << std::endl;
}

inline void DrawImpl(const Drawing::IPointDrawer &drawer,
                     const GraphicalPrimitive &primitive) {
  std::visit(
      [&drawer](const auto &specificPremitive) {
        DrawImpl(drawer, specificPremitive);
      },
      primitive);
}

inline void DrawImpl(const Drawing::IPointDrawer &drawer,
                     const CanvasData &canvas) {
  std::cout << "Drawing all primitives on canvas" << std::endl;
}

} // namespace Models