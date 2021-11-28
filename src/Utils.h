#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace utils {

template <typename Stream> Stream OpenFile(const std::filesystem::path &file) {
  Stream in;
  in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  in.open(file);
  return in;
}

struct Block {
  Block(uintmax_t f, uintmax_t t) : from(f), to(t) {}

  uintmax_t from;
  uintmax_t to;
};

std::vector<Block> SplitFile(const std::filesystem::path &file,
                             uintmax_t blockSize) const {

  if (blockSize == 0) {
    throw std::invalid_argument("blocksCount cannot be 0");
  }

  const auto fileSize = std::filesystem::file_size(file);
  if (fileSize == 0) {
    return {};
  }

  auto in = OpenFile<std::ifstream>(file);

  std::vector<Block> result;
  for (uintmax_t i = 0; i < fileSize;) {
    const auto from = (result.empty()) ? 0 : result.back().to;
    auto to = blockSize + fileSize;
    if (to >= fileSize) {
      result.emplace_back(from, fileSize);
      break;
    }
    in.seekg(to);
    char byte{};
    while (in.get(byte) && byte != '\n') {
      to++;
    }

    result.emplace_back(from, to);
    i = to;

    if (in.eof()) {
      break;
    }
  }

  return result;
}

} // namespace utils