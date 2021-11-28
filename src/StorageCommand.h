#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "TableName.h"
#include "Utils.h"

namespace StorageCommands {

enum class Type { INSERT, TRUNCATE, INTERSECTION, SYMMETRIC_DIFFERENCE };

struct Insert {
  TableName table;
  int id{};
  std::string name;
};
struct Truncate {
  TableName table;
};
struct Intersection {};
struct SymmetricDifference {};

using Command =
    std::variant<Insert, Truncate, Intersection, SymmetricDifference>;

[[nodiscard]] inline Type ParseCommandType(const std::string &typeStr) {
  static const std::unordered_map<std::string, Type> lookup{
      {"INSERT", Type::INSERT},
      {"TRUNCATE", Type::TRUNCATE},
      {"INTERSECTION", Type::INTERSECTION},
      {"SYMMETRIC_DIFFERENCE", Type::SYMMETRIC_DIFFERENCE},
  };

  if (const auto it = lookup.find(typeStr); it != lookup.end()) {
    return it->second;
  }

  throw std::invalid_argument("Unknown command type: " + typeStr);
}

[[nodiscard]] inline Insert
ParseInsertCommand(const std::vector<std::string> &arguments) {
  if (arguments.size() != 3) {
    throw std::invalid_argument(
        "Insert command must be called with 3 arguments");
  }

  Insert result;

  result.table = ParseTableName(arguments[0]);
  result.id = std::stoi(arguments[1]);
  result.name = arguments[2];
  return result;
}

[[nodiscard]] inline Truncate
ParseTruncateCommand(const std::vector<std::string> &arguments) {
  if (arguments.size() != 1) {
    throw std::invalid_argument(
        "Truncate command must be called with single argument");
  }

  Truncate result;

  result.table = ParseTableName(arguments[0]);
  return result;
}

[[nodiscard]] inline Intersection
ParseIntersectionCommand(const std::vector<std::string> &arguments) {
  if (arguments.size() != 0) {
    throw std::invalid_argument(
        "Intersection command must be called without any arguments");
  }
  return Intersection{};
}

[[nodiscard]] inline SymmetricDifference
ParseSymmetricDifferenceCommand(const std::vector<std::string> &arguments) {
  if (arguments.size() != 0) {
    throw std::invalid_argument(
        "SymmetricDifference command must be called without any arguments");
  }
  return SymmetricDifference{};
}

[[nodiscard]] inline Command ParseCommand(const std::string &commandStr) {
  std::cout << "Command received " << commandStr << std::endl;
  auto commandParts = Split(commandStr, ' ');
  if (commandParts.size() == 0) {
    throw std::invalid_argument("Invalid command format");
  }
  const auto commandType = ParseCommandType(commandParts[0]);
  std::vector<std::string> arguments;
  arguments.reserve(commandParts.size() - 1);
  std::move(std::next(commandParts.begin()), commandParts.end(),
            std::back_inserter(arguments));

  switch (commandType) {
  case Type::INSERT:
    return ParseInsertCommand(arguments);
  case Type::TRUNCATE:
    return ParseTruncateCommand(arguments);
  case Type::INTERSECTION:
    return ParseIntersectionCommand(arguments);
  case Type::SYMMETRIC_DIFFERENCE:
    return ParseSymmetricDifferenceCommand(arguments);
  default:
    throw std::out_of_range("Unexpected command type value");
  }
}

} // namespace StorageCommands
