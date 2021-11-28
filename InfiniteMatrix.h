#pragma once

#include <map>

template <typename T> class InfiniteMatrix {
public:
  explicit InfiniteMatrix(T defaultValue)
      : m_defaultValue(std::move(defaultValue)) {}

  class Element {
  public:
    Element(size_t i, size_t j, InfiniteMatrix<T> &matrix)
        : m_i(i), m_j(j), m_matrix(matrix) {}

    Element &operator=(const T &elem) {
      if (elem == m_matrix.m_defaultValue) {
        m_matrix.m_occupiedPositions.erase({m_i, m_j});
      } else {
        m_matrix.m_occupiedPositions[{m_i, m_j}] = elem;
      }
      return *this;
    }

    operator const T &() { return m_matrix.Get(m_i, m_j); }

    size_t m_i{0};
    size_t m_j{0};
    InfiniteMatrix<T> &m_matrix;
  };

  class Row {
  public:
    Row(size_t i, InfiniteMatrix<T> &matrix) : m_i(i), m_matrix(matrix) {}

    Element operator[](size_t j) { return Element(m_i, j, m_matrix); }

  private:
    size_t m_i{0};
    InfiniteMatrix<T> &m_matrix;
  };

  class ConstRow {
    ConstRow(size_t i, const InfiniteMatrix<T> &matrix)
        : m_i(i), m_matrix(matrix) {}

    const T &operator[](size_t j) const { return m_matrix.Get(m_i, j); }

  private:
    size_t m_i{0};
    const InfiniteMatrix<T> &m_matrix;
  };

  Row operator[](size_t i) { return Row(i, *this); }

  ConstRow operator[](size_t i) const { return ConstRow(i, *this); }

  const T &Get(size_t i, size_t j) const noexcept {
    if (const auto it = m_occupiedPositions.find(std::pair{i, j});
        it != m_occupiedPositions.end()) {
      return it->second;
    }

    return m_defaultValue;
  }

  [[nodiscard]] auto begin() const noexcept {
    return m_occupiedPositions.cbegin();
  }

  [[nodiscard]] auto end() const noexcept { return m_occupiedPositions.cend(); }

  [[nodiscard]] size_t size() const noexcept {
    return m_occupiedPositions.size();
  }

private:
  std::map<std::pair<size_t, size_t>, T> m_occupiedPositions;
  T m_defaultValue{};
};