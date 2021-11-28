#pragma once

#include "Models/GraphicalPrimitives.h"

namespace Drawing {

/**
 * @brief Interface for drawing single point
 * Other graphical primitives is implemented as drawing multiple points
 *
 */
class IPointDrawer {
public:
  virtual ~IPointDrawer() = default;

  virtual void Draw(const Models::Point &) const = 0;
};

} // namespace Drawing