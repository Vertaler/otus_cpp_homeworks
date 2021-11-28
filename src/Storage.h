#pragma once

#include <optional>
#include <set>
#include <string>

#include "TableName.h"

struct TableItem {
  explicit TableItem(int id_, std::string name_ = {})
      : id(id_), name(std::move(name_)) {}

  int id{};
  std::string name;
};

/**
 * @brief Single item of insertion or symmetric difference result
 *
 */
struct JoinItem {
  explicit JoinItem(int id_) : id(id_) {}
  explicit JoinItem(int id_, std::string a, std::string b)
      : id(id_), aName(std::move(a)), bName(std::move(b)) {}

  explicit JoinItem(int id_, TableName tableName, std::string name) : id(id_) {
    switch (tableName) {
    case TableName::A:
      aName.emplace(std::move(name));
      break;
    case TableName::B:
      bName.emplace(std::move(name));
      break;
    default:
      throw std::out_of_range("Unexpected table name value");
    }
  }

  int id{};
  std::optional<std::string> aName;
  std::optional<std::string> bName;
};

std::ostream &operator<<(std::ostream &out, const JoinItem &item) {
  const auto optionalToString = [](const std::optional<std::string> &val) {
    return val.has_value() ? *val : "";
  };
  out << item.id << "," << optionalToString(item.aName) << ","
      << optionalToString(item.bName);

  return out;
}

template <typename T> class IdComparator {
public:
  bool operator()(const T &lhs, const T &rhs) const { return lhs.id < rhs.id; }
};

using JoinResult = std::set<JoinItem, IdComparator<JoinItem>>;
using Table = std::set<TableItem, IdComparator<TableItem>>;

class Storage {
public:
  void Insert(TableName t, int id, std::string name) try {
    const auto [it, newElemAdded] = GetTable(t).emplace(id, std::move(name));
    if (!newElemAdded) {
      throw std::invalid_argument("Duplicate " + std::to_string(id));
    }

    // Copy on write
    auto newSymdif = std::make_shared<JoinResult>(*m_symmetricDifference);
    auto newIntersection = std::make_shared<JoinResult>(*m_intersection);

    const auto &otherTable = GetOtherTable(t);
    if (const auto otherIt = otherTable.find(TableItem(id));
        otherIt != otherTable.end()) {
      newIntersection->emplace(id, it->name, otherIt->name);
      newSymdif->erase(JoinItem(id));
    } else {
      newSymdif->emplace(id, t, it->name);
    }

    std::swap(newSymdif, m_symmetricDifference);
    std::swap(newIntersection, m_intersection);
  } catch (...) {
    // Erase element to ensure strict exception safety guarantee.
    // For m_intersection and m_symmetricDifference guarantee ensured based on
    // copy and swap idiom
    GetTable(t).erase(TableItem(id));
    throw;
  }

  void Truncate(TableName t) {
    // Copy on write
    auto newSymdif = std::make_shared<JoinResult>();
    auto newIntersection = std::make_shared<JoinResult>();

    const auto otherTableName = GetOtherTableName(t);
    const auto &otherTable = GetTable(otherTableName);
    for (const auto &[id, name] : otherTable) {
      newSymdif->emplace(id, otherTableName, name);
    }
    GetTable(t).clear();
    std::swap(newSymdif, m_symmetricDifference);
    std::swap(newIntersection, m_intersection);
  }

  [[nodiscard]] std::shared_ptr<JoinResult> Intersection() const {
    return m_intersection;
  }

  [[nodiscard]] std::shared_ptr<JoinResult> SymmetricDifference() const {
    return m_symmetricDifference;
  }

private:
  [[nodiscard]] static TableName GetOtherTableName(TableName name) {
    switch (name) {
    case TableName::A:
      return TableName::B;
    case TableName::B:
      return TableName::A;
    default:
      throw std::out_of_range("Unexpected table name value");
    }
  }

  Table &GetTable(TableName name) {
    switch (name) {
    case TableName::A:
      return m_a;
    case TableName::B:
      return m_b;
    default:
      throw std::out_of_range("Unexpected table name value");
    }
  }

  Table &GetOtherTable(TableName name) {
    return GetTable(GetOtherTableName(name));
  }

  Table m_a;
  Table m_b;

  // We use shared_ptr to simplify copy on write implementation
  std::shared_ptr<JoinResult> m_intersection = std::make_shared<JoinResult>();
  std::shared_ptr<JoinResult> m_symmetricDifference = std::make_shared<JoinResult>();
};