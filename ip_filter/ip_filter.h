#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils.h"

class IPAddress {
public:
  static inline constexpr size_t Size = 4;

  explicit IPAddress(const std::string &ipString) {
    const auto ipParts = split(ipString, '.');
    if (ipParts.size() != Size) {
      throw std::invalid_argument("IP address must contain 4 parts");
    }

    for (size_t i = 0; i < ipParts.size(); i++) {
      const auto &part = ipParts[i];
      try {
        const auto partByte = std::stoul(part);
        if (partByte > std::numeric_limits<uint8_t>::max()) {
          throw std::out_of_range("IP part must be not greater than 255");
        }

        m_data[i] = static_cast<uint8_t>(partByte);
      } catch (const std::exception &exc) {
        std::stringstream msg;
        msg << "Error during parsing " << i
            << " part of IP addresss:" << exc.what();
        throw std::invalid_argument(msg.str());
      }
    }
  }

  template <size_t Index>[[nodiscard]] uint8_t Get() const noexcept {
    static_assert(Index < Size,
                  "Index must be not greater than IPAddress::Size");

    return m_data[Index];
  }

  [[nodiscard]] std::string ToString() const {
    std::stringstream result;
    bool first = true;

    for (const auto &part : m_data) {
      if (!first) {
        result << ".";
      } else {
        first = false;
      }

      result << std::to_string(part);
    }

    return result.str();
  }

  friend bool
  operator>(const IPAddress &lhs, const IPAddress &rhs) noexcept;
  friend bool operator==(const IPAddress &lhs, const IPAddress &rhs) noexcept;

private:
  std::array<uint8_t, Size> m_data;
};

[[nodiscard]] bool operator>(const IPAddress &lhs,
                             const IPAddress &rhs) noexcept {
  return lhs.m_data > rhs.m_data;
}

[[nodiscard]] bool operator==(const IPAddress &lhs,
                              const IPAddress &rhs) noexcept {
  return lhs.m_data == rhs.m_data;
}

std::ostream &operator<<(std::ostream &out, const IPAddress &ip) {
  return out << ip.ToString();
}

using IPList = std::vector<IPAddress>;

namespace detail {
template <typename... Args, std::size_t... I>
[[nodiscard]] IPList filterImpl(const IPList &ipSet, std::index_sequence<I...>,
                                const Args &... args) {
  IPList result;
  for (const auto &ip : ipSet) {
    if (((ip.Get<I>() == args) && ...)) {
      result.push_back(ip);
    }
  }

  return result;
}

template <typename... Args, std::size_t... I>
[[nodiscard]] IPList filterAnyImpl(const IPList &ipSet, uint8_t ipPart,
                                   std::index_sequence<I...>) {
  IPList result;
  for (const auto &ip : ipSet) {
    if (((ip.Get<I>() == ipPart) || ...)) {
      result.push_back(ip);
    }
  }

  return result;
}
} // namespace detail

template <typename... Args>
[[nodiscard]] IPList filter(const IPList &ipSet, const Args &... args) {
  constexpr size_t ArgsCount = sizeof...(Args);
  static_assert(ArgsCount > 0 && ArgsCount <= 4,
                "Count of filter arguments must be > 0 and <= 4.");
  return detail::filterImpl(ipSet, std::make_index_sequence<ArgsCount>(),
                            args...);
}

template <typename... Args>
[[nodiscard]] IPList filterAny(const IPList &ipSet, uint8_t ipPart) {
  return detail::filterAnyImpl(ipSet, ipPart,
                               std::make_index_sequence<IPAddress::Size>());
}
