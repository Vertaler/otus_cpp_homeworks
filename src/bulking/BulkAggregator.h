#pragma once

#include <exception>
#include <iostream>

#include "CommandBulk.h"

namespace bulking {

/**
 * Helper class needed to satisfy following homeworw requirement
 * - commands from static bulks from different clients should be mixed
 * - commands from dynamic bulks from different clients should not be mixed
 *
 */
class BulkAggregator {

public:
  explicit BulkAggregator(size_t staticBulkSize)
      : m_staticBulkSize(staticBulkSize) {
    if (m_staticBulkSize == 0)
      throw std::invalid_argument("Bulk size can not be zero");
  }

  void SendBulk(const CommandBulk &bulk) {
    for (const auto &handler : m_commandHandlers) {
      try {
        handler(bulk);
      } catch (std::exception &exc) {
        std::cout << "Unexpected error during command bulk handling: "
                  << exc.what();
      }
    }
  }

  void SendCommand(std::string command) {
    m_currentStaticBulk.AddCommand(std::move(command));
    if (m_currentStaticBulk.Size() == m_staticBulkSize) {
      SendStaticBulk();
    }
  }

  BulkAggregator &RegisterHandler(CommandBulkHandler handler) {
    m_commandHandlers.push_back(std::move(handler));
    return *this;
  }

private:
  void SendStaticBulk() {
    if (m_currentStaticBulk.Empty()) {
      return;
    }
    SendBulk(m_currentStaticBulk);
    m_currentStaticBulk = CommandBulk{};
  }

private:
  size_t m_staticBulkSize{};
  CommandBulk m_currentStaticBulk{};
  std::vector<CommandBulkHandler> m_commandHandlers;
};

} // namespace bulking