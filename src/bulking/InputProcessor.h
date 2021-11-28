#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

#include "CommandBulk.h"

namespace bulking {

class InputProcessor {
public:
  explicit InputProcessor(size_t staticBulkSize)
      : m_staticBulkSize(staticBulkSize) {}

  InputProcessor &RegisterHandler(CommandBulkHandler handler) {
    m_commandHandlers.push_back(std::move(handler));
    return *this;
  }

  /**
   * @brief Processes input stream of bulk commands
   *
   * @param in input stream
   * @param endBulkOnEndOfStream treat end of stream as end of bulk
   */
  void ProcessStream(std::istream &in, bool endBulkOnEndOfStream = true) {
    for (std::string token; std::getline(in, token);) {
      if (token.empty()) {
        continue;
      }
      if (token == "{") {
        OpenDynamicBulk();
      } else if (token == "}") {
        CloseDynamicBulk();
      } else {
        ProcessCommand(std::move(token));
      }
    }
    if (endBulkOnEndOfStream && IsStaticBulk()) {
      SendCurrentBulk();
    }
  }

  void SendCurrentBulk() {
    if (m_currentBulk.Empty()) {
      return;
    }

    for (const auto &handler : m_commandHandlers) {
      try {
        handler(m_currentBulk);
      } catch (std::exception &exc) {
        std::cout << "Unexpected error during command bulk handling: "
                  << exc.what();
      }
    }
    m_currentBulk = CommandBulk{};
  }

private:
  [[nodiscard]] bool IsStaticBulk() const { return m_nestingLevel == 0; }
  void OpenDynamicBulk() {
    if (IsStaticBulk()) {
      SendCurrentBulk();
    }

    m_nestingLevel++;
  }
  void CloseDynamicBulk() {
    if (IsStaticBulk()) {
      throw std::runtime_error("Attempt to close not opened dynamic bulk");
    }
    m_nestingLevel--;

    if (m_nestingLevel == 0) {
      SendCurrentBulk();
    }
  }
  void ProcessCommand(std::string command) {
    m_currentBulk.AddCommand(std::move(command));
    if (IsStaticBulk() && m_currentBulk.Size() == m_staticBulkSize) {
      SendCurrentBulk();
    }
  }

private:
  size_t m_staticBulkSize{};

  CommandBulk m_currentBulk{};
  size_t m_nestingLevel{};

  std::vector<CommandBulkHandler> m_commandHandlers;
};

} // namespace bulking