#include <iostream>

#include "InfiniteMatrix.h"

int main(int argc, const char *const argv[]) {
  InfiniteMatrix<int> m(0);

  const size_t lastFilledColumn = 9;
  for (size_t i = 0; i <= lastFilledColumn; i++) {
    // main diagonal
    m[i][i] = i;
    // antidiagonal
    m[i][lastFilledColumn - i] = static_cast<int>(lastFilledColumn - i);
  }

  std::cout << "m[1:8][1:8]: " << std::endl;
  for (size_t i = 1; i <= 8; i++) {
    for (size_t j = 1; j <= 8; j++) {
      if (j != 1)
        std::cout << " ";

      std::cout << m[i][j];
    }
    std::cout << std::endl;
  }

  std::cout << "Count of occupied positions: " << m.size() << std::endl;
  std::cout << "All occupied positions: " << std::endl;
  for (const auto &[pos, val] : m) {
    std::cout << "m[" << pos.first << "," << pos.second << "]=" << val
              << std::endl;
  }

  return 0;
}
