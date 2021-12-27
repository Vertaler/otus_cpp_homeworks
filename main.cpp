#include <fstream>
#include <iostream>

#include "InputProcessor.h"

void PrintToStdout(const CommandBulk &bulk) {
  bool first = true;
  std::cout << "bulk: ";
  for (const auto &command : bulk) {
    if (!first) {
      std::cout << ", ";
    }
    std::cout << command;
    first = false;
  }
  std::cout << std::endl;
}

void PrintToFile(const CommandBulk &bulk) {
  using namespace std::chrono;
  auto timeStamp =
      duration_cast<seconds>(bulk.FirstCommandTime().time_since_epoch())
          .count();
  std::ofstream file("bulk" + std::to_string(timeStamp) + ".log");

  for (const auto &command : bulk) {
    file << command << std::endl;
  }
}

int main(int argc, const char *const argv[]) try {
  if (argc != 2) {
    std::cerr << "Usage: bulk <static_bulk_size>" << std::endl;
    return 1;
  }

  const auto staticBulkSize = std::stoi(argv[1]);
  InputProcessor inputProcessor(staticBulkSize);
  inputProcessor.RegisterHandler(PrintToStdout);
  inputProcessor.RegisterHandler(PrintToFile);
  inputProcessor.ProcessStream(std::cin);

  return 0;
} catch (std::exception &exc) {
  std::cerr << "Unexpected error: " << exc.what();
  return 1;
}