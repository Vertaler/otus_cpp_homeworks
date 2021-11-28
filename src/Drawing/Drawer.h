#pragma once

#include "IPointDrawer.h"

namespace Drawing {

/**
 * @brief Class for drawing objects of arbitary types
 *
 * This class delegates work to Draw function overloads for corresponding types
 */
class Drawer {
public:
  /**
   * @brief Draws object using point drawer
   *
   * @tparam T type of object
   * @param drawer point drawer
   * @param obj object for drawing
   */
  template <typename T>
  void Draw(const IPointDrawer &drawer, const T &obj) const {
    DrawImpl(drawer, obj);
  }
};

template <typename T> void DrawImpl(const IPointDrawer &drawer, const T &obj) {
  static_assert(sizeof(T) < 0, "DrawImpl is not provided for type T");
}

} // namespace Drawing