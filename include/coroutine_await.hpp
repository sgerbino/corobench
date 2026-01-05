#pragma once

#include <attributes.hpp>
#include <coroutine>

namespace async_coro_await {

// Awaitable task that can be co_awaited
template <typename T> class CORO_AWAIT_ELIDABLE task {
public:
  struct promise_type {
    T value;

    task get_return_object() {
      return task{std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    std::suspend_never initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    void return_value(T val) noexcept { value = val; }

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

  T get() noexcept { return handle.promise().value; }

  // Awaiter for co_await support
  struct Awaiter {
    std::coroutine_handle<promise_type> handle;

    bool await_ready() const noexcept { return handle.done(); }

    void await_suspend(std::coroutine_handle<>) noexcept {}

    T await_resume() noexcept { return handle.promise().value; }
  };

  Awaiter operator co_await() noexcept { return Awaiter{handle}; }

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

// Using co_await for true composition
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

} // namespace async_coro_await
