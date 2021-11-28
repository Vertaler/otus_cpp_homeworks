#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "bulking/CommandBulk.h"

namespace async {

using handle_t = uint32_t;

handle_t connect(std::size_t bulk,
                 const std::vector<bulking::CommandBulkHandler> &handlers);
void receive(handle_t handle, const char *data, std::size_t size);
void disconnect(handle_t handle);

} // namespace async
