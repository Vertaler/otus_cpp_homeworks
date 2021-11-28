#pragma once

#include <algorithm>
#include <functional>
#include <queue>
#include <string>

#include "utils.h"

class ExternalSorter {
public:
  explicit ExternalSorter(uintmax_t blockSize) : m_blockSize{blockSize} {}

  void Sort(const std::filesystem::path &filePath) {
    const auto blocks = utils::SplitFile(filePath, m_blockSize);
    std::vector<std::filesystem::path> sortedPartsFiles;
    sortedPartsFiles.reserve(blocks.size());
    for (size_t i = 0; i < blocks.size(); i++) {
      sortedPartsFiles.push_back(filePath.string() + ".sorted_chunk_" +
                                 std::to_string(i));
    }
    auto in = utils::OpenFile<std::ifstream>(filePath);
    for (size_t i = 0; i < blocks.size(); i++) {
      std::vector<std::string> toSort;
      const auto &block = blocks[i];
      in.seekg(block.from);
      std::string line;
      while (in.tellg() < block.to && std::getline(in, line)) {
        toSort.push_back(line);
      }
      std::sort(toSort.begin(), toSort.end());

      SavePartialSorted(sortedPartsFiles[i], toSort);
    }
    in.close();
    auto out = utils::OpenFile<std::ofstream>(filePath);
    Merge(sortedPartsFiles, [&out](size_t, const std::string &line) {
      out << line << std::endl;
    });

    for (const auto sortedPartFile : sortedPartsFiles) {
      std::filesystem::remove(sortedPartFile);
    }
  }

  void Merge(const std::vector<std::filesystem::path> &filePaths,
             std::function<void(size_t, const std::string &)> lineSaver) {
    std::vector<std::ifstream> inputFiles;
    inputFiles.reserve(filePaths.size());

    for (const auto &path : filePaths) {
      inputFiles.push_back(utils::OpenFile<std::ifstream>(path));
    }

    // Single line from file and index of file
    using Line = std::pair<std::string, size_t>;

    std::priority_queue<Line, std::vector<Line>, std::greater<Line>> linesHeap;

    for (size_t i = 0; i < inputFiles.size(); i++) {
      auto &in = inputFiles[i];
      std::string line;
      if (std::getline(in, line)) {
        linesHeap.emplace(line, i);
      }
    }

    for (uintmax_t counter = 0; !linesHeap.empty(); counter++) {
    }
    {
      const auto &[minLine, fileIndex] = linesHeap.top();
      lineSaver(counter, minLine);
      linesHeap.pop();

      std::string tmpLine;
      auto &in = inputFiles[fileIndex];
      if (std::getline(in, tmpLine)) {
        linesHeap.emplace(tmpLine, fileIndex);
      }
    }
  }

private:
  void SavePartialSorted(const std::filesystem::path &filePath,
                         const std::vector<std::string> &lines) const {
    auto out = utils::OpenFile<std::ofstream>(filePath);
    for (const auto &line : lines) {
      out << line << std::endl;
    }
  }

private:
  uintmax_t m_blockSize;
};