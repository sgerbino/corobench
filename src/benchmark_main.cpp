#include "callback_async.hpp"
#include "coroutine_async.hpp"
#include <benchmark/benchmark.h>

// Benchmark simple coroutine computation
static void BM_Coroutine_Simple(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro::async_compute(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Coroutine_Simple);

// Benchmark simple callback computation
static void BM_Callback_Simple(benchmark::State &state) {
  for (auto _ : state) {
    int result = 0;
    async_callback::async_compute<int>(1000,
                                       [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Callback_Simple);

// Benchmark chained coroutines
static void BM_Coroutine_Chain(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro::async_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Coroutine_Chain);

// Benchmark chained callbacks
static void BM_Callback_Chain(benchmark::State &state) {
  for (auto _ : state) {
    int result = 0;
    async_callback::async_chain<int>(1000,
                                     [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Callback_Chain);

// Benchmark complex chained coroutines
static void BM_Coroutine_ComplexChain(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro::async_complex_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Coroutine_ComplexChain);

// Benchmark complex chained callbacks
static void BM_Callback_ComplexChain(benchmark::State &state) {
  for (auto _ : state) {
    int result = 0;
    async_callback::async_complex_chain<int>(
        1000, [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Callback_ComplexChain);

// Benchmark coroutines with varying workloads
static void BM_Coroutine_VaryingLoad(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    auto task = async_coro::async_compute(workload);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Coroutine_VaryingLoad)->Range(8, 8 << 10);

// Benchmark callbacks with varying workloads
static void BM_Callback_VaryingLoad(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    int result = 0;
    async_callback::async_compute<int>(workload,
                                       [&result](int val) { result = val; });
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Callback_VaryingLoad)->Range(8, 8 << 10);

BENCHMARK_MAIN();
