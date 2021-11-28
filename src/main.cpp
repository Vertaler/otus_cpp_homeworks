#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/asio.hpp>

#include "mapreduce.h"

int main(int argc, char *argv[]) try {
  MapReduce mapReduce;
  mapReduce
      .SetMapper(
          [](const std::string &str) { return std::to_string(str.size()); })
      .SetReducer([](const std::string &str, const std::string &accumulated) {
        const auto num = std::stoi(str);
        int accumulatedNum = 0;
        if (accumulated.empty()) {
          accumulatedNum = std::stoi(accumulated);
        }

        return std::to_string(num + accumulatedNum);
      });

  mapReduce.Run("test_file.txt", "test_dir");
  return EXIT_SUCCESS;
} catch (const std::exception &exc) {
  std::cerr << "Uncaught exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
