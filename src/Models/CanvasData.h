#pragma once

#include <cstdint>
#include <map>

#include "GraphicalPrimitives.h"

namespace Models {

using PrimitiveId = uint32_t;

struct CanvasData {
  std::map<PrimitiveId, GraphicalPrimitive> primitives;
  uint32_t height{};
  uint32_t width{};
};

} // namespace Models