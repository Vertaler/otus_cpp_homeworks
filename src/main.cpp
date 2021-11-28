#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>
#include <vector>

#include "async/async.h"

#include "SynchronizedQueue.h"

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
  fileNameStream << "bulk" << timeStamp << "_" << std::this_thread::get_id()
                 << ".log";
  std::ofstream file(fileNameStream.str());

  for (const auto &command : bulk) {
    file << command << std::endl;
  }
}

void CommandBulkProcessor(std::atomic_bool &stopCondition,
                          SynchronizedQueue<bulking::CommandBulk> &bulkQueue,
                          const bulking::CommandBulkHandler &handler) {
  using namespace std::chrono_literals;
  while (!stopCondition || !bulkQueue.Empty()) {
    auto bulk = bulkQueue.TryPop(100ms);
    if (bulk) {
      handler(*bulk);
    }
  }
}

std::thread
CommandBulkProcessorThread(std::atomic_bool &stopCondition,
                           SynchronizedQueue<bulking::CommandBulk> &bulkQueue,
                           bulking::CommandBulkHandler handler) {
  return std::thread([&stopCondition, &bulkQueue, handler] {
    CommandBulkProcessor(stopCondition, bulkQueue, handler);
  });
}

bulking::CommandBulkHandler
PushToQueueHandler(SynchronizedQueue<bulking::CommandBulk> &queue) {
  return [&queue](const bulking::CommandBulk &bulk) { queue.Push(bulk); };
}

int main(int, char *[]) {
  SynchronizedQueue<bulking::CommandBulk> fileLogQueue;
  SynchronizedQueue<bulking::CommandBulk> stdoutLogQueue;

  auto fileLogHandler = PushToQueueHandler(fileLogQueue);
  auto stdoutHandler = PushToQueueHandler(stdoutLogQueue);

  std::atomic_bool stopCondition{false};

  std::vector<std::thread> threads;
  threads.push_back(
      CommandBulkProcessorThread(stopCondition, fileLogQueue, PrintToFile));
  threads.push_back(
      CommandBulkProcessorThread(stopCondition, fileLogQueue, PrintToFile));
  threads.push_back(
      CommandBulkProcessorThread(stopCondition, stdoutLogQueue, PrintToStdout));

  std::size_t bulk = 5;
  auto h = async::connect(bulk, {fileLogHandler, stdoutHandler});
  auto h2 = async::connect(bulk, {fileLogHandler, stdoutHandler});

  async::receive(h, "1", 1);
  async::receive(h2, "1\n", 2);
  async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
  async::receive(h, "b\nc\nd\n}\n89\n", 11);

  async::disconnect(h);
  async::disconnect(h2);

  stopCondition = true;
  for (auto &thread : threads) {
    thread.join();
  }

  return 0;
}
