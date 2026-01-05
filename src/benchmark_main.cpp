#include <benchmark/benchmark.h>
#include <callback_async.hpp>
#include <coroutine_async.hpp>
#include <coroutine_optimized.hpp>
#include <attributes.hpp>

// Only include elidable benchmarks if the decorator is actually being used
#if defined(__clang__) && !defined(__apple_build_version__)
#define ENABLE_ELIDABLE_BENCHMARKS
#include <coroutine_elidable.hpp>
#include <coroutine_elidable_task.hpp>
#endif

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

#ifdef ENABLE_ELIDABLE_BENCHMARKS
static void BM_Simple_CoroElidable(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_elidable::async_compute(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Simple_CoroElidable);

static void BM_Simple_CoroElidableTask(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_elidable_task::async_compute(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Simple_CoroElidableTask);
#endif

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

#ifdef ENABLE_ELIDABLE_BENCHMARKS
static void BM_Chain_CoroElidable(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_elidable::async_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Chain_CoroElidable);

static void BM_Chain_CoroElidableTask(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_elidable_task::async_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_Chain_CoroElidableTask);
#endif

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

#ifdef ENABLE_ELIDABLE_BENCHMARKS
static void BM_ComplexChain_CoroElidable(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_elidable::async_complex_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ComplexChain_CoroElidable);

static void BM_ComplexChain_CoroElidableTask(benchmark::State &state) {
  for (auto _ : state) {
    auto task = async_coro_elidable_task::async_complex_chain(1000);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_ComplexChain_CoroElidableTask);
#endif

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

#ifdef ENABLE_ELIDABLE_BENCHMARKS
static void BM_VaryingLoad_CoroElidable(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    auto task = async_coro_elidable::async_compute(workload);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_VaryingLoad_CoroElidable)->Range(8, 8 << 10);

static void BM_VaryingLoad_CoroElidableTask(benchmark::State &state) {
  int workload = state.range(0);
  for (auto _ : state) {
    auto task = async_coro_elidable_task::async_compute(workload);
    int result = task.get();
    benchmark::DoNotOptimize(result);
  }
}
BENCHMARK(BM_VaryingLoad_CoroElidableTask)->Range(8, 8 << 10);
#endif

BENCHMARK_MAIN();