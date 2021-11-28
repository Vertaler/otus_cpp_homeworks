#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/asio.hpp>

#include "Server.h"
#include "bulking/BulkAggregator.h"

void PrintToStdout(const bulking::CommandBulk &bulk) {
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

void PrintToFile(const bulking::CommandBulk &bulk) {
  auto timeStamp = bulk.FirstCommandTime().time_since_epoch().count();

  std::stringstream fileNameStream;
  fileNameStream << "bulk" << timeStamp << ".log";
  std::ofstream file(fileNameStream.str());

  for (const auto &command : bulk) {
    file << command << std::endl;
  }
}

int main(int argc, char *argv[]) try {

  if (argc != 3) {
    std::cout << "Usage: bulk_server <port> <static_bulk_size>" << std::endl;
    return EXIT_FAILURE;
  }

  const auto port = std::stoul(argv[1]);
  const auto bulk = std::stoul(argv[2]);

  if (port > std::numeric_limits<uint16_t>::max()) {
    throw std::invalid_argument("Too big port number");
  }

  bulking::BulkAggregator bulkAggregator(bulk);
  bulkAggregator.RegisterHandler(PrintToFile);
  bulkAggregator.RegisterHandler(PrintToStdout);

  boost::asio::io_context ioc;

  Server server(ioc, bulkAggregator, port);

  ioc.run();

  return EXIT_SUCCESS;
} catch (const std::exception &exc) {
  std::cerr << "Uncaught exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
