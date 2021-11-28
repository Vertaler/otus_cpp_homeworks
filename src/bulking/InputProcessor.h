#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

#include "BulkAggregator.h"
#include "CommandBulk.h"

namespace bulking {

class InputProcessor {
public:
  explicit InputProcessor(BulkAggregator &bulkAggregator)
      : m_bulkAggregator(bulkAggregator) {}

  /**
   * @brief Processes input stream of bulk commands.
   *
   * For static bulks sends command to BulkAggregator immediately
   * For dynamic bulks sends to BulkAggregator whole dynamic bulk
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
    if (endBulkOnEndOfStream) {
      SendCurrentBulk();
    }
  }

  void SendCurrentBulk() {
    m_bulkAggregator.SendBulk(m_currentBulk);
    m_currentBulk = CommandBulk{};
  }

private:
  [[nodiscard]] bool IsStaticBulk() const { return m_nestingLevel == 0; }
  void OpenDynamicBulk() { m_nestingLevel++; }
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
    if (IsStaticBulk()) {
      m_bulkAggregator.SendCommand(std::move(command));
    } else {
      m_currentBulk.AddCommand(std::move(command));
    }
  }

private:
  BulkAggregator &m_bulkAggregator;

  CommandBulk m_currentBulk{};
  size_t m_nestingLevel{};
};

} // namespace bulking