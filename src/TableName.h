#pragma once

#include <string>
#include <unordered_map>

enum class TableName { A, B };

[[nodiscard]] inline TableName ParseTableName(const std::string &tableNameStr) {
  static const std::unordered_map<std::string, TableName> lookup{
      {"A", TableName::A},
      {"B", TableName::B},
  };
  if (const auto it = lookup.find(tableNameStr); it != lookup.end()) {
    return it->second;
  }

  throw std::invalid_argument("Unknown table name: " + tableNameStr);
}