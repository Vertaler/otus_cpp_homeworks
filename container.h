#pragma once

#include <memory>
#include <stdexcept>
#include <string>

template <typename T, typename Allocator = std::allocator<T>>
class CustomContainer {
public:
  CustomContainer() {
    if constexpr (DEFAULT_CAPACITY != 0)
      Alloc(DEFAULT_CAPACITY);
  }
  ~CustomContainer() { Cleanup(); }

  CustomContainer(const CustomContainer<T, Allocator> &rhs) = delete;
  CustomContainer<T, Allocator> &
  operator=(const CustomContainer<T, Allocator> &rhs) = delete;
  CustomContainer(CustomContainer<T, Allocator> &&rhs)
      : m_data(rhs.m_data), m_size(rhs.m_size), m_capacity(rhs.m_capacity),
        m_alloc(std::move(rhs.m_alloc)) {
    rhs.m_data = nullptr;
    rhs.m_size = 0;
    rhs.m_capacity = 0;
  }
  CustomContainer<T, Allocator> &
  operator=(CustomContainer<T, Allocator> &&rhs) = delete;

  void push_back(const T &element) {
    if (m_size + 1 >= m_capacity) {
      Realloc(m_capacity * 2 + 1);
    }
    std::allocator_traits<Allocator>::construct(m_alloc, m_data + m_size,
                                                element);
    m_size++;
  }

  [[nodiscard]] const T &at(size_t i) const {
    if (i >= m_size)
      throw std::out_of_range("Index " + std::to_string(i) + " out of range");

    return m_data[i];
  }

  size_t size() const noexcept {
    return m_size;
  }

private:
  static constexpr size_t DEFAULT_CAPACITY = 2;
  static constexpr size_t GROWTH_FACTOR = 2;
  static constexpr size_t GROWTH_TERM = 1;

  void Alloc(size_t capacity) {
    m_data = m_alloc.allocate(capacity);
    m_capacity = capacity;
  }

  void Realloc(size_t newCapacity) {
    auto newData = m_alloc.allocate(newCapacity);
    size_t i = 0;
    try {
      for (; i < m_size; ++i) {
        newData[i] = m_data[i];
      }
    } catch (...) {
      for (auto j = static_cast<int64_t>(i); j >= 0; --j) {
        std::allocator_traits<Allocator>::destroy(m_alloc, newData + j);
      }
      m_alloc.deallocate(newData, newCapacity);
      throw;
    }

    Cleanup();
    m_data = newData;
    m_capacity = newCapacity;
  }

  void Cleanup() noexcept {
    if (m_data == nullptr) {
      return;
    }

    for (size_t i = 0; i < m_size; ++i) {
      std::allocator_traits<Allocator>::destroy(m_alloc, m_data + i);
    }
    m_alloc.deallocate(m_data, m_capacity);
  }

private:
  size_t m_size{};
  size_t m_capacity{};
  T *m_data{};
  Allocator m_alloc;
};