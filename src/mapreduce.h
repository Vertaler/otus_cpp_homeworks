#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "externalSorting.h"
#include "utils.h"

constexpr size_t SORT_BLOCK_SIZE = 4096;

class MapReduce {
public:
  using Mapper = std::function<std::string(const std::string &)>;
  using Reducer =
      std::function<std::string(const std::string &, const std::string &)>;

  std::vector<std::filesystem::path> Run(const std::filesystem::path &input,
                                         const std::filesystem::path &out) {
    if (!m_mapper) {
      throw std::logic_error("mapper is not set");
    }

    if (!m_reducer) {
      throw std::logic_error("reducer is not set");
    }

    const auto mapResult = Map(input, out);
    const auto toReduce = Shuffle(mapResult, out);
    return Reduce(toReduce, out);
  }

  MapReduce &MappersCount(size_t mappersCount) {
    m_mappersCount = mappersCount;
    return *this;
  }

  MapReduce &ReducersCount(size_t reducersCount) {
    m_reducersCount = reducersCount;
    return *this;
  }

  MapReduce &SetMapper(Mapper mapper) {
    m_mapper = std::move(mapper);
    return *this;
  }

  MapReduce &SetReducer(Reducer reducer) {
    m_reducer = std::move(reducer);
    return *this;
  }

private:
  struct MapResult {
    uintmax_t totalCountOfLines{0};
    std::vector<std::filesystem::path> mappedFiles;
  };

  MapResult Map(const std::filesystem::path &inputFile,
                const std::filesystem::path &outputDir) const {
    const auto blockSize =
        std::filesystem::file_size(inputFile) / m_mappersCount;
    const auto blocks = utils::SplitFile(inputFile, blockSize);

    MapResult result;
    result.mappedFiles.reserve(blocks.size());
    for (size_t i = 0; i < blocks.size(); i++) {
      result.mappedFiles.push_back(outputDir / ("mapper_" + std::to_string(i)));
    }

    std::vector<std::thread> threads;
    threads.reserve(blocks.size());

    MapResult result;
    for (size_t i = 0; i < blocks.size(); i++) {
      threads.emplace_back([this, block = blocks[i],
                            outFile = result.mappedFiles[i], &inputFile,
                            &result]() {
        auto in = utils::OpenFile<std::ifstream>(inputFile);
        auto out = utils::OpenFile<std::ofstream>(outFile);

        std::string line;
        while (in.tellg() < block.to && std::getline(in, line)) {
          out << m_mapper(line) << std::endl;
        }
      });
    }

    for (auto &t : threads) {
      t.join();
    }

    return result;
  }

  std::vector<std::filesystem::path>
  Shuffle(const MapResult &mapResult, const std::filesystem::path &outputDir) {
    for (const auto &mapFile : mapResult.mappedFiles) {
      m_sorter.Sort(mapFile);
    }

    std::vector<std::filesystem::path> outFiles;
    outFiles.reserve(m_reducersCount);
    for (size_t i = 0; i < m_reducersCount; i++) {
      outFiles.push_back(outputDir / ("to_reduce_" + std::to_string(i)));
    }

    const auto linesPerFile = mapResult.totalCountOfLines / m_reducersCount;
    size_t currentIndex = 0;
    auto out = utils::OpenFile<std::ofstream>(outFiles[0]);
    m_sorter.Merge(mapResult.mappedFiles,
                   [&outFiles, &out, &currentIndex,
                    linesPerFile](size_t counter, const std::string &line) {
                     size_t newIndex = counter / linesPerFile;
                     if (newIndex != currentIndex) {
                       out = utils::OpenFile<std::ifstream>(outFiles[0]);
                       currentIndex = newIndex;
                     }

                     out << line << std::endl;
                   });
  }

  std::vector<std::filesystem::path>
  Reduce(const std::vector<std::filesystem::path> toReduce,
         const std::filesystem::path &outputDir) const {

    std::vector<std::filesystem::path> outFiles;
    outFiles.reserve(toReduce.size());
    for (size_t i = 0; i < toReduce.size(); i++) {
      outFiles.push_back(outputDir / ("reducer_" + std::to_string(i)));
    }

    std::vector<std::thread> threads;
    threads.reserve(toReduce.size());

    for (size_t i = 0; i < toReduce.size(); i++) {
      threads.emplace_back(
          [this, outFile = outFiles[i], inputFile = toReduce[i]]() {
            auto in = utils::OpenFile<std::ifstream>(inputFile);

            std::string line;
            std::string res;

            while (std::getline(in, line)) {
              res = m_reducer(line, res);
            }

            auto out = utils::OpenFile<std::ofstream>(outFile);
            out << res;
          });
    }

    for (auto &t : threads) {
      t.join();
    }

    return outFiles;
  }

  size_t m_mappersCount{1};
  size_t m_reducersCount{1};

  Mapper m_mapper;
  Reducer m_reducer;
  ExternalSorter m_sorter{SORT_BLOCK_SIZE};
};