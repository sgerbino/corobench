#pragma once

#include <attributes.hpp>
#include <coroutine>
#include <optional>
#include <stdexcept>

namespace async_coro {

template <typename T> class CORO_AWAIT_ELIDABLE task {
public:
  struct promise_type {
    std::optional<T> value;
    std::exception_ptr exception;

    task get_return_object() {
      return task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    void return_value(T val) { value = std::move(val); }

    void unhandled_exception() { exception = std::current_exception(); }
  };

  explicit task(std::coroutine_handle<promise_type> h) : handle(h) {}

  task(task &&other) noexcept : handle(other.handle) { other.handle = nullptr; }

  task &operator=(task &&other) noexcept {
    if (this != &other) {
      if (handle) {
        handle.destroy();
      }
      handle = other.handle;
      other.handle = nullptr;
    }
    return *this;
  }

  ~task() {
    if (handle) {
      handle.destroy();
    }
  }

  task(const task &) = delete;
  task &operator=(const task &) = delete;

  T get() {
    if (!handle) {
      throw std::runtime_error("Invalid coroutine handle");
    }

    auto &promise = handle.promise();
    if (promise.exception) {
      std::rethrow_exception(promise.exception);
    }

    if (!promise.value) {
      throw std::runtime_error("No value available");
    }

    return *promise.value;
  }

  bool done() const { return handle && handle.done(); }

private:
  std::coroutine_handle<promise_type> handle;
};

// Simple async computation examples
// Use volatile to prevent optimization and make computation depend on actual
// work
task<int> async_compute(int x) {
  volatile int result = 0;
  // Perform actual computation that can't be constant-folded
  for (int i = 0; i < x; i = i + 1) {
    volatile int temp = i * 31 + (i & 1); // Non-trivial computation
    result += temp;
  }
  co_return static_cast<int>(result);
}

task<int> async_chain(int x) {
  auto result1 = async_compute(x);
  int val1 = result1.get();

  auto result2 = async_compute(val1 % 100);
  int val2 = result2.get();

  co_return val1 + val2;
}

task<int> async_complex_chain(int x) {
  auto r1 = async_compute(x);
  int v1 = r1.get();

  auto r2 = async_compute(v1 % 100);
  int v2 = r2.get();

  auto r3 = async_compute(v2 % 50);
  int v3 = r3.get();

  co_return v1 + v2 + v3;
}

} // namespace async_coro
