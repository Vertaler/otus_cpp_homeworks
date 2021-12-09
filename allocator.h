#pragma once

#include <cstdlib>
#include <memory>
#include <new>
#include <vector>

namespace Detail {
class MemoryPool {
public:
  explicit MemoryPool(size_t size) : m_size(size), m_data(new uint8_t[size]) {}
  ~MemoryPool() { delete[](m_data); }

  MemoryPool(const MemoryPool &) = delete;
  MemoryPool &operator=(const MemoryPool &) = delete;
  MemoryPool(MemoryPool &&) = delete;
  MemoryPool &operator=(MemoryPool &&) = delete;

  template <typename T> T *Allocate(size_t n) {
    if (n == 0) {
      return nullptr;
    }

    const auto newPosition = m_freePos + n * sizeof(T);
    if (newPosition >= m_size) {
      throw std::bad_alloc();
    }

    auto result = reinterpret_cast<T *>(&m_data[m_freePos]);
    m_freePos = newPosition;
    return result;
  }

private:
  size_t m_freePos{};
  size_t m_size{};
  uint8_t *m_data{};
};
} // namespace Detail

static constexpr inline size_t DEFAULT_ALLOCATOR_POOL_SIZE = 10 * 1024;

template <typename T, size_t BufferSize = DEFAULT_ALLOCATOR_POOL_SIZE>
struct CustomAllocator {
  using value_type = T;

  // Needed for m_memoryPool access in converting copy constructor
  // implementation
  template <typename, size_t> friend struct CustomAllocator;

  template <typename U> struct rebind {
    using other = CustomAllocator<U, BufferSize>;
  };

  CustomAllocator()
      : m_memoryPool(std::make_shared<Detail::MemoryPool>(BufferSize)) {}
  ~CustomAllocator() = default;

  template <typename U, size_t BufSize = DEFAULT_ALLOCATOR_POOL_SIZE>
  CustomAllocator(const CustomAllocator<U, BufSize> &rhs) noexcept
      : m_memoryPool(rhs.m_memoryPool) {}

  template <class U, size_t BufSize = DEFAULT_ALLOCATOR_POOL_SIZE>
  bool operator==(const CustomAllocator<U, BufSize> &rhs) const noexcept {
    return m_memoryPool == rhs.m_memoryPool;
  }
  template <class U, size_t BufSize = DEFAULT_ALLOCATOR_POOL_SIZE>
  bool operator!=(const CustomAllocator<U, BufSize> &rhs) const noexcept {
    return !(*this == rhs);
  }

  T *allocate(std::size_t n) { return m_memoryPool->Allocate<T>(n); }

  void deallocate(T *, std::size_t) {
    // Deallocate is not needed to implement in this homework
  }

private:
  std::shared_ptr<Detail::MemoryPool> m_memoryPool;
};