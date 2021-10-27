#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string &str, char d) {
  std::vector<std::string> r;

  std::string::size_type start = 0;
  std::string::size_type stop = str.find_first_of(d);
  while (stop != std::string::npos) {
    r.push_back(str.substr(start, stop - start));

    start = stop + 1;
    stop = str.find_first_of(d, start);
  }

  r.push_back(str.substr(start));

  return r;
}

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

  template <size_t Index> uint8_t Get() const noexcept {
    static_assert(Index < Size,
                  "Index must be not greater than IPAddress::Size");

    return m_data[Index];
  }

  std::string ToString() const {
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

  friend bool operator>(const IPAddress &lhs, const IPAddress &rhs) noexcept;

private:
  std::array<uint8_t, Size> m_data;
};

bool operator>(const IPAddress &lhs, const IPAddress &rhs) noexcept {
  return lhs.m_data > rhs.m_data;
}

std::ostream &operator<<(std::ostream &out, const IPAddress &ip) {
  return out << ip.ToString();
}

using IPList = std::vector<IPAddress>;

template <typename... Args, std::size_t... I>
IPList filterImpl(const IPList &ipSet, std::index_sequence<I...>,
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
IPList filterAnyImpl(const IPList &ipSet, uint8_t ipPart,
                     std::index_sequence<I...>) {
  IPList result;
  for (const auto &ip : ipSet) {
    if (((ip.Get<I>() == ipPart) || ...)) {
      result.push_back(ip);
    }
  }

  return result;
}

template <typename... Args>
IPList filter(const IPList &ipSet, const Args &... args) {
  constexpr size_t ArgsCount = sizeof...(Args);
  static_assert(ArgsCount > 0 && ArgsCount <= 4,
                "Count of filter arguments must be > 0 and <= 4.");
  return filterImpl(ipSet, std::make_index_sequence<ArgsCount>(), args...);
}

template <typename... Args>
IPList filterAny(const IPList &ipSet, uint8_t ipPart) {
  return filterAnyImpl(ipSet, ipPart,
                       std::make_index_sequence<IPAddress::Size>());
}

int main(int argc, char const *argv[]) try {

  IPList ipPool;

  for (std::string line; std::getline(std::cin, line);) {
    std::vector<std::string> v = split(line, '\t');
    ipPool.emplace_back(v.at(0));
  }

  // reverse lexicographically sort
  std::sort(ipPool.begin(), ipPool.end(), std::greater{});

  for (const auto &ip : ipPool) {
    std::cout << ip << std::endl;
  }

  // filter by first byte and output
  for (const auto &ip : filter(ipPool, 1)) {
    std::cout << ip << std::endl;
  }

  // filter by first and second bytes and output
  for (const auto &ip : filter(ipPool, 46, 70)) {
    std::cout << ip << std::endl;
  }

  // filter by first and second bytes and output
  for (const auto &ip : filterAny(ipPool, 46)) {
    std::cout << ip << std::endl;
  }

  return 0;
} catch (const std::exception &e) {
  std::cerr << e.what() << std::endl;
  return 1;
}