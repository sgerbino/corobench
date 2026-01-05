#pragma once

#include <coroutine>

namespace async_coro_opt {

// Optimized Task with minimal overhead
template <typename T> class task {
public:
  struct promise_type {
    T value;

    task get_return_object() {
      return task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    // Eager execution - no suspension at start
    std::suspend_never initial_suspend() noexcept { return {}; }

    // Suspend at end to preserve value
    std::suspend_always final_suspend() noexcept { return {}; }

    void return_value(T val) noexcept { value = val; }

    // No exception handling for performance
    void unhandled_exception() noexcept {}
  };

  explicit task(std::coroutine_handle<promise_type> h) noexcept : handle(h) {}

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

  // Simplified get - no error checking for performance
  T get() noexcept { return handle.promise().value; }

  bool done() const noexcept { return handle && handle.done(); }

  // Awaiter for co_await support
  struct awaiter {
    std::coroutine_handle<promise_type> handle;

    bool await_ready() const noexcept { return handle.done(); }

    void await_suspend(std::coroutine_handle<>) noexcept {}

    T await_resume() noexcept { return handle.promise().value; }
  };

  awaiter operator co_await() noexcept { return awaiter{handle}; }

private:
  std::coroutine_handle<promise_type> handle;
};

// Simple async computation
task<int> async_compute(int x) {
  volatile int result = 0;
  for (int i = 0; i < x; i = i + 1) {
    volatile int temp = i * 31 + (i & 1);
    result += temp;
  }
  co_return static_cast<int>(result);
}

task<int> async_chain(int x) {
  int val1 = co_await async_compute(x);
  int val2 = co_await async_compute(val1 % 100);
  co_return val1 + val2;
}

task<int> async_complex_chain(int x) {
  int v1 = co_await async_compute(x);
  int v2 = co_await async_compute(v1 % 100);
  int v3 = co_await async_compute(v2 % 50);
  co_return v1 + v2 + v3;
}

} // namespace async_coro_opt
