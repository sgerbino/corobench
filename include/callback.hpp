#pragma once

#include <functional>

namespace async_callback {

template <typename T> using Callback = std::function<void(T)>;

// Simple callback-based async computation examples
// Use volatile to prevent optimization and make computation depend on actual
// work
template <typename T> void async_compute(int x, Callback<T> callback) {
  volatile T result = 0;
  // Perform actual computation that can't be constant-folded
  for (int i = 0; i < x; i = i + 1) {
    volatile T temp = i * 31 + (i & 1); // Non-trivial computation
    result += temp;
  }
  callback(static_cast<T>(result));
}

template <typename T> void async_chain(int x, Callback<T> final_callback) {
  async_compute<T>(x, [final_callback](T result1) {
    async_compute<T>(result1 % 100, [result1, final_callback](T result2) {
      final_callback(result1 + result2);
    });
  });
}

template <typename T>
void async_complex_chain(int x, Callback<T> final_callback) {
  async_compute<T>(x, [final_callback](T v1) {
    async_compute<T>(v1 % 100, [v1, final_callback](T v2) {
      async_compute<T>(v2 % 50, [v1, v2, final_callback](T v3) {
        final_callback(v1 + v2 + v3);
      });
    });
  });
}

} // namespace async_callback
