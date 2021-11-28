/**
 * @file Serialization.h
 * @brief Models serialization
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

#include "Serialization/Serialization.h"

namespace Models {

inline void SerializeImpl(const Point &point, std::ostream &out) {
  std::cout << "Serializing point " << std::endl;
}

inline void SerializeImpl(const Line &line, std::ostream &out) {
  std::cout << "Serializing line " << std::endl;
}

inline void SerializeImpl(const Circle &circle, std::ostream &out) {
  std::cout << "Serializing circle " << std::endl;
}

inline void SerializeImpl(const Polygon &polygon, std::ostream &out) {
  std::cout << "Serializing polygon " << std::endl;
}

inline void SerializeImpl(const GraphicalPrimitive &primitive,
                          std::ostream &out) {
  std::visit(
      [&out](const auto &specificPremitive) {
        SerializeImpl(specificPremitive, out);
      },
      primitive);
}

inline void SerializeImpl(const CanvasData &canvasData, std::ostream &out) {
  std::cout << "Serializing canvas data " << std::endl;
}

inline void DeserializeImpl(Point &point, std::istream &in) {
  std::cout << "Deserializing point " << std::endl;
}

inline void DeserializeImpl(Line &line, std::istream &in) {
  std::cout << "Deserializing line " << std::endl;
}

inline void DeserializeImpl(Circle &circle, std::istream &in) {
  std::cout << "Deserializing circle " << std::endl;
}

inline void DeserializeImpl(Polygon &polygon, std::istream &in) {
  std::cout << "Deserializing polygon " << std::endl;
}

inline void DeserializeImpl(GraphicalPrimitive &primitive, std::istream &in) {
  std::cout << "Deserializing primitive " << std::endl;
}

inline void DeserializeImpl(CanvasData &canvasData, std::istream &in) {
  std::cout << "Deserializing canvas data " << std::endl;
}

} // namespace Models