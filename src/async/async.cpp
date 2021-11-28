#include "async.h"

#include "ContextManager.h"

namespace async {

handle_t connect(std::size_t bulk,
                 const std::vector<bulking::CommandBulkHandler> &handlers) {
  return ContextManager::Instance().Connect(bulk, handlers);
}

void receive(handle_t handle, const char *data, std::size_t size) {
  ContextManager::Instance().Receive(handle, data, size);
}

void disconnect(handle_t handle) {
  ContextManager::Instance().Disconnect(handle);
}

} // namespace async
