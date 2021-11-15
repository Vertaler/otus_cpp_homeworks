#pragma once

#include <iostream>
#include <string>
#include <type_traits>

namespace Detail {
/**
 * @brief Metafunction which always returns false regardless of parameters
 *
 */
template <typename... T> class AlwaysFalse : std::false_type {};

template <typename... T>
constexpr inline bool AlwaysFalseV = AlwaysFalse<T...>::value;

/**
 * @brief Metafunction which checks if T is STL container
 *
 */
template <typename T, typename _ = void>
struct IsContainer : std::false_type {};

template <typename T>
struct IsContainer<
    T,
    std::void_t<typename T::value_type, typename T::size_type,
                typename T::allocator_type, typename T::iterator,
                typename T::const_iterator, decltype(std::declval<T>().size()),
                decltype(std::declval<T>().begin()),
                decltype(std::declval<T>().end()),
                decltype(std::declval<T>().cbegin()),
                decltype(std::declval<T>().cend())>> : public std::true_type {};

template <typename T>
constexpr inline bool IsContainerV = IsContainer<T>::value;

} // namespace Detail

/**
 * @brief Class for printing IP addresses stored in different data types.
 *
 */
class IpPrinter {
public:
  /**
   * @brief Construct a new Ip Printer object
   *
   * @param outStream std::ostream for printing
   */
  explicit IpPrinter(std::ostream &outStream) : m_outStream(outStream) {}

  /**
   * @brief Prints param to output stream and adds line feed
   *
   * @tparam T type of parameter: integral type, vector, list or string
   * @param param contains ip address for printing
   * @return *this
   */
  template <typename T> IpPrinter &operator<<(const T &param) {
    Print(param);
    m_outStream << std::endl;
    return *this;
  }

private:
  template <typename T, bool> void Print(const T &param) {
    static_assert(Detail::AlwaysFalseV<T>,
                  "Print is not implemented for type T");
  }

  /**
   * @brief Prints ip stored as integer value
   *
   * @tparam Integer integral tyoe
   * @param param integer for printing
   */
  template <typename Integer,
            std::enable_if_t<std::is_integral_v<Integer>, bool> = true>
  void Print(const Integer &param) {
    constexpr auto countOfBytes = sizeof(Integer);
    bool first = true;
    for (int i = countOfBytes - 1; i >= 0; i--) {
      if (!first) {
        m_outStream << '.';
      }
      const auto byteToPrint = (param >> 8 * i) & 0xFF;
      m_outStream << byteToPrint;
      first = false;
    }
  }

  /**
   * @brief Prints ip stored as string
   *
   * @tparam String string
   * @param param string for printing
   */
  template <typename String,
            std::enable_if_t<std::is_same_v<String, std::string>, bool> = true>
  void Print(const String &param) {
    m_outStream << param;
  }

  /**
   * @brief Prints ip stored in STL container
   *
   * @tparam Container STL container
   * @param param contaoner for printing
   */

  template <typename Container,
            std::enable_if_t<Detail::IsContainerV<Container> &&
                                 !std::is_same_v<Container, std::string>,
                             bool> = true>
  void Print(const Container &container) {
    bool first = true;

    for (const auto &elem : container) {
      if (!first) {
        m_outStream << '.';
      }

      m_outStream << elem;
      first = false;
    }
  }

  std::ostream &m_outStream;
};
