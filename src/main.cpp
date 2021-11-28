#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/asio.hpp>

#include "Server.h"

int main(int argc, char *argv[]) try {

  if (argc != 2) {
    std::cout << "Usage: join_server <port> <static_bulk_size>" << std::endl;
    return EXIT_FAILURE;
  }

  const auto port = std::stoul(argv[1]);

  if (port > std::numeric_limits<uint16_t>::max()) {
    throw std::invalid_argument("Too big port number");
  }

  boost::asio::io_context ioc;
  Server server(ioc, static_cast<uint16_t>(port));
  ioc.run();

  return EXIT_SUCCESS;
} catch (const std::exception &exc) {
  std::cerr << "Uncaught exception: " << exc.what() << std::endl;
  return EXIT_FAILURE;
}
