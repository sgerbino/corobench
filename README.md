# corobench - Coroutines vs Callbacks Performance Comparison

A C++20 project that benchmarks the performance of C++20 coroutines against traditional callback-based async patterns using Google Benchmark. This project explores multiple coroutine optimization strategies and provides comprehensive performance comparisons.

## Project Structure

```
corobench/
├── CMakeLists.txt                  # CMake configuration
├── include/
│   ├── attributes.hpp              # Compiler-specific optimization attributes
│   ├── callback_async.hpp          # Callback-based async implementation
│   ├── coroutine_async.hpp         # Standard coroutine (with safety features)
│   ├── coroutine_optimized.hpp     # Optimized coroutine (minimal overhead)
│   └── coroutine_elidable.hpp      # Elidable coroutine with [[clang::coro_await_elidable]]
└── src/
    └── benchmark_main.cpp          # Comprehensive benchmark suite
```

## Requirements

- C++20 compatible compiler (GCC 14+, Clang 18+, or MSVC 2022+)
- CMake 3.25 or higher
- Internet connection (for fetching Google Benchmark)
- For elidable benchmarks: Non-Apple Clang compiler

## Building

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build .
```

## Running Benchmarks

After building, run the benchmark executable:

```bash
./corobench
```

### Useful Benchmark Options

```bash
# Run specific benchmark groups
./corobench --benchmark_filter=Simple
./corobench --benchmark_filter=Chain
./corobench --benchmark_filter=VaryingLoad

# Run only optimized coroutine benchmarks
./corobench --benchmark_filter=CoroOptimized

# Run with repetitions for statistical confidence
./corobench --benchmark_repetitions=3

# Output results in JSON format
./corobench --benchmark_format=json > results.json

# Display counters (iterations, CPU time, etc.)
./corobench --benchmark_counters_tabular=true

# Show all available options
./corobench --help
```

## Implementation Comparison

All coroutine implementations use `co_await` for proper async composition in chain/complex chain scenarios.

| Implementation | Promise Type | Composition | Clang Attributes | Use Case |
|----------------|--------------|-------------|------------------|----------|
| **Callback** | N/A | Nested lambdas | None | Baseline comparison |
| **Coroutine** | Full safety (exception + optional) | `co_await` | None | Production code needing safety |
| **CoroOptimized** | Minimal (direct value) | `co_await` | None | Performance-critical code |
| **CoroElidable** | Minimal | `co_await` | `[[coro_await_elidable]]` on task class, `[[coro_await_elidable_argument]]` on parameters | Compiler heap allocation elision (Clang only) |

### Key Differences

**Callback (callback_async.hpp)**
- Traditional `std::function` based callbacks
- Nested lambdas for chained operations
- "Callback pyramid of doom" in complex scenarios
- No coroutine overhead, but harder to maintain

**Coroutine (coroutine_async.hpp)**
- Full `task<T>` with `std::optional<T>` and `std::exception_ptr`
- Exception-safe with proper error propagation
- Awaiter supports `co_await` composition
- Best for production code requiring safety guarantees

**CoroOptimized (coroutine_optimized.hpp)**
- Minimal promise type (direct value storage, no exceptions)
- All methods marked `noexcept`
- Awaiter supports `co_await` composition
- Best balance of performance and clean code

**CoroElidable (coroutine_elidable.hpp)**
- Minimal promise type like CoroOptimized
- `[[clang::coro_await_elidable]]` on task class (class attribute)
- `[[clang::coro_await_elidable_argument]]` on function parameters
- Allows compiler to perform heap allocation elision
- Only enabled on non-Apple Clang compilers
- Provides hints for aggressive heap allocation elision at call sites

## Compiler Attributes

### `[[clang::coro_await_elidable]]`

This is a **class attribute** applied to the coroutine return type (the `task` class). It provides a hint to the compiler to apply heap allocation elision more aggressively.

The compiler performs heap allocation elision on call expressions under a safe elide context. Such elision replaces the heap-allocated activation frame of the callee coroutine with a local variable within the enclosing braces in the caller's stack frame.

Example from `coroutine_elidable.hpp`:
```cpp
template <typename T> class [[clang::coro_await_elidable]] task { ... };
```

### `[[clang::coro_await_elidable_argument]]`

This is a **function parameter attribute** that propagates safe elide context to arguments if the function is also under a safe elide context.

Example from `coroutine_elidable.hpp`:
```cpp
task<int> async_chain([[clang::coro_await_elidable_argument]] task<int> task1) {
  int val1 = co_await task1;
  // ...
}
```

### Platform Support

The elidable benchmarks are conditionally compiled based on compiler support:
- **Enabled**: Non-Apple Clang (Clang on Linux, Windows)
- **Disabled**: Apple Clang, GCC, MSVC

When building with unsupported compilers, the elidable benchmarks are excluded from compilation using preprocessor guards.

## Benchmark Organization

Benchmarks are organized by scenario, testing all implementations:

### 1. Simple Operations (workload=1000)
Single async computation performance.

### 2. Two-Level Chains
Composition of two async operations (`async_chain`).

### 3. Three-Level Complex Chains
Testing callback pyramid vs coroutine composition (`async_complex_chain`).

### 4. Varying Workloads
Performance scaling from 8 to 8192 iterations.

## Optimization Techniques Used

The optimized coroutine implementations demonstrate several techniques:

1. **Removed Exception Overhead**: No `std::exception_ptr` storage
2. **Direct Value Storage**: No `std::optional<T>` wrapper
3. **Noexcept Annotations**: Helps compiler optimize away checks
4. **Simplified Promise Type**: Minimal state in promise_type
5. **Eager Execution**: Uses `suspend_never` for initial_suspend
6. **Compiler Attributes** (CoroElidable only): Hints for heap allocation elision

These optimizations are fair because:
- The computation work is identical across all implementations
- No unsafe operations are used
- The API remains usable and correct
- Real async code often doesn't need exception propagation in the async layer

## Preventing Compiler Optimization

The benchmarks use `volatile` variables and non-trivial computation to prevent the compiler from optimizing away the work:

```cpp
volatile int result = 0;
for (int i = 0; i < x; i = i + 1) {
    volatile int temp = i * 31 + (i & 1);
    result += temp;
}
```

This ensures we're measuring the actual async mechanism overhead, not just compiler cleverness.

## Customizing Benchmarks

To add new benchmarks, edit `src/benchmark_main.cpp`:

```cpp
static void BM_YourTest_Callback(benchmark::State& state) {
    for (auto _ : state) {
        // Your callback code
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_YourTest_Callback);

static void BM_YourTest_CoroOptimized(benchmark::State& state) {
    for (auto _ : state) {
        auto task = async_coro_opt::your_function();
        // Use the task result
        benchmark::DoNotOptimize(task);
    }
}
BENCHMARK(BM_YourTest_CoroOptimized);

#ifdef ENABLE_ELIDABLE_BENCHMARKS
static void BM_YourTest_CoroElidable(benchmark::State& state) {
    for (auto _ : state) {
        auto task = async_coro_elidable::your_function();
        // Use the task result
        benchmark::DoNotOptimize(task);
    }
}
BENCHMARK(BM_YourTest_CoroElidable);
#endif
```

## Practical Recommendations

Based on typical benchmark results:

### Use Callbacks When:
- ❌ **Generally not recommended** - coroutines often outperform in realistic scenarios
- Interfacing with C APIs that require function pointers
- Working in pre-C++20 codebases

### Use Standard Coroutines (Coroutine) When:
- ✅ **Production code requiring safety** - exception handling and error propagation
- You value correctness over maximum performance
- You need proper error propagation through async chains
- The small overhead is acceptable for peace of mind

### Use Optimized Coroutines (CoroOptimized) When:
- ✅ **Best balance of performance and clean code**
- Complex async workflows (multiple levels of chaining)
- You handle errors at a higher level (outside the async layer)
- You want predictable performance across all compilers

### Use Elidable Coroutines (CoroElidable) When:
- ✅ **Maximum performance with non-Apple Clang**
- You're using a supported compiler (Linux/Windows Clang)
- You want compiler-assisted heap allocation elision
- Performance is critical and you can rely on Clang-specific features

## General Guidelines

1. **Default to CoroOptimized** for new projects (best portability)
2. **Use CoroElidable** when targeting Clang specifically for maximum performance
3. **Use Standard Coroutine** when safety is paramount
4. **Avoid callbacks** unless interfacing with legacy C code
5. **Always use `co_await`** for composition in async chains
6. **Benchmark your specific use case** - results vary by workload

## Key Takeaways

### 1. **Coroutines Can Beat Callbacks**
With proper `co_await` composition, coroutines can achieve better performance than callbacks in chained operations. This is the realistic use case for async code.

### 2. **Elidable Attributes Can Help**
`[[clang::coro_await_elidable]]` provides compiler hints for heap allocation elision, potentially improving performance when using supported compilers.

### 3. **Safety Has Minimal Cost**
The standard Coroutine implementation (with full exception safety) has modest overhead for simple operations, but can be competitive in chained scenarios.

### 4. **Complexity Favors Coroutines**
As async operations are chained together:
- Callbacks show the "pyramid of doom" pattern
- Coroutines maintain clean, sequential code
- Code readability improves dramatically with coroutines

### 5. **Proper Composition Matters**
All implementations use `co_await` for composition in async chains. Proper `co_await` usage is essential for realistic async code and accurate benchmarks.

### 6. **Compiler Support Matters**
The elidable attributes are Clang-specific and not supported on Apple Clang. Design your code to gracefully handle different compiler capabilities using preprocessor guards.

## License

This is a demonstration project for educational purposes.
