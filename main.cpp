#include <cassert>
#include <iostream>
#include <map>
#include <string_view>
#include <type_traits>

#include "allocator.h"
#include "container.h"

template <typename T> constexpr T Factorial(T n) {
  static_assert(std::is_integral_v<T>,
                "Factorial is implemented only for integer numbers");
  assert(n >= 0 && "Factorial for negative integers is not defined");
  if (n == 0)
    return 1;

  return n * Factorial(n - 1);
}

using IntPair = std::pair<const int, int>;

template <template <typename> typename Alloc>
using IntMap = std::map<int, int, std::less<int>, Alloc<IntPair>>;

template <template <typename> typename Alloc, typename... Args>
IntMap<Alloc> CreateMap(Args &&... args) {
  IntMap<Alloc> result(std::forward<Args>(args)...);
  for (size_t i = 0; i < 10; i++) {
    result.emplace(i, Factorial(i));
  }

  return result;
}

template <template <typename> typename Alloc>
CustomContainer<int, Alloc<int>> CreateContainer() {
  CustomContainer<int, Alloc<int>> result;
  for (size_t i = 0; i < 10; i++) {
    result.push_back(i);
  }

  return result;
}

template <template <typename> typename Alloc>
void PrintMap(const std::string_view &header, const IntMap<Alloc> &map) {
  std::cout << header << std::endl;
  for (const auto &[key, value] : map) {
    std::cout << key << " " << value << std::endl;
  }
}

template <template <typename> typename Alloc>
void PrintContainer(const std::string_view &header,
                    const CustomContainer<int, Alloc<int>> &container) {
  std::cout << header << std::endl;
  for (size_t i = 0; i < container.size(); i++) {
    std::cout << container.at(i) << std::endl;
  }
}

int main(int argc, const char *const argv[]) {
  const auto mapWithStdAlloc = CreateMap<std::allocator>();
  const auto mapWithCustomAlloc =
      CreateMap<CustomAllocator>(CustomAllocator<IntPair>(1000));
  PrintMap("Map with std allocator", mapWithStdAlloc);
  PrintMap("Map with custom allocator", mapWithCustomAlloc);

  const auto containerWithStdAlloc = CreateContainer<std::allocator>();
  const auto containerWithCustomAlloc = CreateContainer<CustomAllocator>();
  PrintContainer("Custom container with std allocator", containerWithStdAlloc);
  PrintContainer("Custom container with custom allocator",
                 containerWithCustomAlloc);

  return 0;
}