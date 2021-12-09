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

using IntMap = std::map<int, int>;
using IntMapCustomAlloc =
    std::map<int, int, std::less<int>, CustomAllocator<IntPair, 10000>>;

template <typename Map> Map CreateMap() {
  Map result;
  for (size_t i = 0; i < 10; i++) {
    result.emplace(i, Factorial(i));
  }

  return result;
}

template <typename Container> Container CreateContainer() {
  Container result;
  for (size_t i = 0; i < 10; i++) {
    result.push_back(i);
  }

  return result;
}

template <typename Map>
void PrintMap(const std::string_view &header, const Map &map) {
  std::cout << header << std::endl;
  for (const auto &[key, value] : map) {
    std::cout << key << " " << value << std::endl;
  }
}

template <typename Container>
void PrintContainer(const std::string_view &header,
                    const Container &container) {
  std::cout << header << std::endl;
  for (size_t i = 0; i < container.size(); i++) {
    std::cout << container.at(i) << std::endl;
  }
}

int main(int argc, const char *const argv[]) {
  const auto mapWithStdAlloc = CreateMap<IntMap>();
  const auto mapWithCustomAlloc = CreateMap<IntMapCustomAlloc>();
  PrintMap("Map with std allocator", mapWithStdAlloc);
  PrintMap("Map with custom allocator", mapWithCustomAlloc);

  const auto containerWithStdAlloc =
      CreateContainer<CustomContainer<int, std::allocator<int>>>();
  const auto containerWithCustomAlloc =
      CreateContainer<CustomContainer<int, CustomAllocator<int>>>();
  PrintContainer("Custom container with std allocator", containerWithStdAlloc);
  PrintContainer("Custom container with custom allocator",
                 containerWithCustomAlloc);

  return 0;
}