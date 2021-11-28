#pragma once

#include <atomic>
#include <map>
#include <mutex>
#include <stdexcept>

#include "async.h"
#include "bulking/InputProcessor.h"

namespace async {
class ContextManager {
public:
  ContextManager(const ContextManager &) = delete;
  ContextManager &operator=(const ContextManager &) = delete;
  ContextManager(ContextManager &&) = delete;
  ContextManager &operator=(ContextManager &&) = delete;

  static ContextManager &Instance() {
    static ContextManager instance;
    return instance;
  }

  handle_t Connect(std::size_t bulk,
                   const std::vector<bulking::CommandBulkHandler> &handlers) {
    auto handle = m_contextIdCounter++;
    bulking::InputProcessor inputProcessor(bulk);
    for (const auto &handler : handlers) {
      inputProcessor.RegisterHandler(handler);
    }
    std::scoped_lock lock(m_contextMutex);
    m_contexts.emplace(handle, inputProcessor);
    return handle;
  }

  void Disconnect(handle_t handle) {
    std::scoped_lock lock(m_contextMutex);
    m_contexts.erase(handle);
  }

  void Receive(handle_t handle, const char *data, std::size_t size) {

    std::scoped_lock lock(m_contextMutex);
    auto it = m_contexts.find(handle);
    if (it == m_contexts.end()) {
      throw std::logic_error("Attempt to receive data for unknown context");
    }
  }

private:
  ContextManager() = default;

  std::atomic<handle_t> m_contextIdCounter{};
  std::map<handle_t, bulking::InputProcessor> m_contexts;
  std::mutex m_contextMutex;
};

} // namespace async