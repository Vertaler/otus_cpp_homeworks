#pragma once

#include <iostream>

namespace Serialization {

template <typename T> void SerializeImpl(const T &obj, std::ostream &out) {
  static_assert(sizeof(T) < 0, "DeserializeImpl is not provided for type T");
}

template <typename T> T DeserializeImpl(T &obj, std::istream &in) {
  static_assert(sizeof(T) < 0, "SerializeImpl is not provided for type T");
}

/**
 * @brief Class for arbitary objects serialization
 *
 * This class delegates work to SerializeImpl/DeserializeImpl overloads for
 * corresponding types.
 */
class Serializer {
public:
  /**
   * @brief Saves object data to output stream
   *
   * @tparam T type of object
   * @param obj object for saving
   * @param out out strem
   */
  template <typename T> void Serialize(const T &obj, std::ostream &out) const {
    SerializeImpl(obj, out);
  }

  /**
   * @brief Constrcuts T object based on data in input stream
   *
   * @tparam T type of object
   * @param obj destination for saving inout data
   * @param in input stream
   */
  template <typename T> void Deserialize(T &obj, std::istream &in) const {
    return DeserializeImpl(obj, in);
  }
};

} // namespace Serialization