#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>

template <typename T> class SynchronizedQueue {
public:
  void Push(T val) {
    std::scoped_lock lock(m_mutex);
    m_queue.push(std::move(val));
    m_cv.notify_one();
  }

  template <class Rep, class Period = std::ratio<1>>
  [[nodiscard]] std::optional<T>
  TryPop(std::chrono::duration<Rep, Period> timeout) {
    std::unique_lock lock(m_mutex);
    m_cv.wait_for(lock, timeout, [this]() { return m_queue.empty(); });
    std::optional<T> result;
    if (!m_queue.empty()) {
      result = m_queue.front();
      m_queue.pop();
    }

    return result;
  }

  bool Empty() const
  {
	  std::scoped_lock lock(m_mutex);
	  return m_queue.empty();
  }

private:
  mutable std::mutex m_mutex;
  std::condition_variable m_cv;
  std::queue<T> m_queue;
};