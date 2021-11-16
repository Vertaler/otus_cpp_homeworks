#include <iostream>
#include <string>
#include <vector>

#include "ip_filter.h"
#include "utils.h"

int main(int argc, char const *argv[]) try {

  IPList ipPool;

  for (std::string line; std::getline(std::cin, line);) {
    std::vector<std::string> v = split(line, '\t');
    ipPool.emplace_back(v.at(0));
  }

  // reverse lexicographically sort
  std::sort(ipPool.begin(), ipPool.end(), std::greater{});

  for (const auto &ip : ipPool) {
    std::cout << ip << std::endl;
  }

  // filter by first byte and output
  for (const auto &ip : filter(ipPool, 1)) {
    std::cout << ip << std::endl;
  }

  // filter by first and second bytes and output
  for (const auto &ip : filter(ipPool, 46, 70)) {
    std::cout << ip << std::endl;
  }

  // filter by first and second bytes and output
  for (const auto &ip : filterAny(ipPool, 46)) {
    std::cout << ip << std::endl;
  }

  return 0;
} catch (const std::exception &e) {
  std::cerr << e.what() << std::endl;
  return 1;
}