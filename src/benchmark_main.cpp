#include <benchmark/benchmark.h>
#include <callback_async.hpp>
#include <coroutine_async.hpp>
#include <coroutine_await.hpp>
#include <coroutine_optimized.hpp>

// ============================================================================
// SIMPLE OPERATIONS - Single async computation (workload=1000)
// ============================================================================

static void BM_Simple_Callback(benchmark::State &state) {
  for (auto _ : state) {
    int result = 0;
    async_callback::async_compute<int>(1000,
                                       [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Simple_Callback);

static void BM_Simple_Coroutine(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro::async_compute(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Simple_Coroutine);

static void BM_Simple_CoroOptimized(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_opt::async_compute(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Simple_CoroOptimized);

static void BM_Simple_CoroAwait(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_await::async_compute(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Simple_CoroAwait);

// ============================================================================
// TWO-LEVEL CHAINS - Chaining two async operations
// ============================================================================

static void BM_Chain_Callback(benchmark::State &state) {
  for (auto _ : state) {
    int result = 0;
    async_callback::async_chain<int>(1000,
                                     [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Chain_Callback);

static void BM_Chain_Coroutine(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro::async_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Chain_Coroutine);

static void BM_Chain_CoroOptimized(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_opt::async_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Chain_CoroOptimized);

static void BM_Chain_CoroAwait(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_await::async_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Chain_CoroAwait);

// ============================================================================
// THREE-LEVEL COMPLEX CHAINS - Testing callback pyramid vs coroutines
// ============================================================================

static void BM_ComplexChain_Callback(benchmark::State &state) {
  for (auto _ : state) {
    int result = 0;
    async_callback::async_complex_chain<int>(
        1000, [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ComplexChain_Callback);

static void BM_ComplexChain_Coroutine(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro::async_complex_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ComplexChain_Coroutine);

static void BM_ComplexChain_CoroOptimized(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_opt::async_complex_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ComplexChain_CoroOptimized);

static void BM_ComplexChain_CoroAwait(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_await::async_complex_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ComplexChain_CoroAwait);

// ============================================================================
// VARYING WORKLOADS - Performance scaling (8 to 8192 iterations)
// ============================================================================

static void BM_VaryingLoad_Callback(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    int result = 0;
    async_callback::async_compute<int>(workload,
                                       [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_VaryingLoad_Callback)->Range(8, 8 << 10);

static void BM_VaryingLoad_Coroutine(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    auto task = async_coro::async_compute(workload);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_VaryingLoad_Coroutine)->Range(8, 8 << 10);

static void BM_VaryingLoad_CoroOptimized(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    auto task = async_coro_opt::async_compute(workload);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_VaryingLoad_CoroOptimized)->Range(8, 8 << 10);

static void BM_VaryingLoad_CoroAwait(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    auto task = async_coro_await::async_compute(workload);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_VaryingLoad_CoroAwait)->Range(8, 8 << 10);

BENCHMARK_MAIN();
