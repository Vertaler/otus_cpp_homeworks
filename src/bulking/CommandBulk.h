#pragma once

#include <cassert>
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace bulking {

struct CommandBulk {

public:
  using Clock = std::chrono::system_clock;

  void AddCommand(std::string command) {
    if (m_commands.empty()) {
      m_firstCommandTime = Clock::now();
    }
    m_commands.push_back(std::move(command));
  }
  [[nodiscard]] auto begin() const { return m_commands.cbegin(); }
  [[nodiscard]] auto end() const { return m_commands.cend(); }
  [[nodiscard]] auto Empty() const { return m_commands.empty(); }
  [[nodiscard]] auto Size() const { return m_commands.size(); }

  [[nodiscard]] auto FirstCommandTime() const noexcept {
    assert(m_firstCommandTime &&
           "FirstCommandTime() should not be called for empty bulk");
    return *m_firstCommandTime;
  }

private:
  std::vector<std::string> m_commands;
  std::optional<Clock::time_point> m_firstCommandTime;
};

using CommandBulkHandler = std::function<void(const CommandBulk &)>;

} // namespace bulking