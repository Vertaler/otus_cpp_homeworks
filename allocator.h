#pragma once

#include <memory>
#include <new>
#include <vector>

namespace Detail {
class MemoryPool {
public:
  MemoryPool() : MemoryPool(DEFAULT_MEMORY_POOL_SIZE) {}
  explicit MemoryPool(size_t size) : m_data(size) {}

  template <typename T> T *Allocate(size_t n) {
    if (n == 0) {
      return nullptr;
    }

    const auto newPosition = m_freePos + n * sizeof(T);
    if (newPosition >= m_data.size()) {
      throw std::bad_alloc();
    }

    auto result = reinterpret_cast<T *>(&m_data[m_freePos]);
    m_freePos = newPosition;
    return result;
  }

  bool operator==(const MemoryPool &rhs) const noexcept {
    return m_data.data() == rhs.m_data.data() && m_freePos == rhs.m_freePos;
  }

private:
  static constexpr size_t DEFAULT_MEMORY_POOL_SIZE = 10 * 1024;

  size_t m_freePos{};
  std::vector<uint8_t> m_data;
};
} // namespace Detail

template <typename T> struct CustomAllocator {
  using value_type = T;

  // Needed for m_memoryPool access in converting copy constructor
  // implementation
  template <typename> friend struct CustomAllocator;

  template <typename U> struct rebind { using other = CustomAllocator<U>; };

  CustomAllocator() : m_memoryPool(std::make_shared<Detail::MemoryPool>()) {}
  CustomAllocator(size_t elementsCount)
      : m_memoryPool(
            std::make_shared<Detail::MemoryPool>(elementsCount * sizeof(T))) {}
  ~CustomAllocator() = default;

  template <typename U>
  CustomAllocator(const CustomAllocator<U> &rhs) noexcept
      : m_memoryPool(rhs.m_memoryPool) {}

  template <class U>
  bool operator==(const CustomAllocator<U> &rhs) const noexcept {
    return m_memoryPool == rhs.m_memoryPool;
  }
  template <class U>
  bool operator!=(const CustomAllocator<U> &rhs) const noexcept {
    return !(*this == rhs);
  }

  T *allocate(std::size_t n) { return m_memoryPool->Allocate<T>(n); }

  void deallocate(T *, std::size_t) {
    // Deallocate is not needed to implement in this homework
  }

private:
  std::shared_ptr<Detail::MemoryPool> m_memoryPool;
};