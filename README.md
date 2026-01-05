# corobench - Coroutines vs Callbacks Performance Comparison

A C++26 project that benchmarks the performance of C++20 coroutines against traditional callback-based async patterns using Google Benchmark. This project explores multiple coroutine optimization strategies and provides comprehensive performance comparisons.

## Project Structure

```
corobench/
├── CMakeLists.txt                  # CMake configuration
├── include/
│   ├── attributes.hpp              # Compiler-specific optimization attributes
│   ├── callback_async.hpp          # Callback-based async implementation
│   ├── coroutine_async.hpp         # Standard coroutine (with safety features)
│   ├── coroutine_optimized.hpp     # Optimized coroutine (minimal overhead)
│   ├── coroutine_elidable.hpp      # Awaiter-decorated elidable coroutine
│   └── coroutine_elidable_task.hpp # task-decorated elidable coroutine
└── src/
    └── benchmark_main.cpp          # Comprehensive benchmark suite
```

## Requirements

- C++26 compatible compiler (GCC 14+, Clang 18+, or MSVC 2022+)
- CMake 3.25 or higher
- Internet connection (for fetching Google Benchmark)

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
| **CoroElidable** | Minimal | `co_await` | `[[coro_await_elidable]]` on Awaiter | Compiler optimization (Awaiter) |
| **CoroElidableTask** | Minimal | `co_await` | `[[coro_await_elidable]]` on task | Compiler optimization (task) |

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
- `[[clang::coro_await_elidable]]` on Awaiter struct
- `[[clang::coro_await_elidable_argument]]` on function parameters
- Allows compiler to elide unnecessary suspensions
- Excellent for simple operations

**CoroElidableTask (coroutine_elidable_task.hpp)**
- Minimal promise type like CoroOptimized
- `[[clang::coro_await_elidable]]` on task class itself
- Alternative attribute placement strategy
- Semantically declares the entire type as elidable

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

## Performance Results

Based on benchmark results (3-run average, Clang compiler):

### Simple Operations (1000 iterations)

| Implementation | Time (ns) | vs Callback | Notes |
|----------------|-----------|-------------|-------|
| Callback | 1,176 | baseline | - |
| Coroutine | 1,240 | +5.4% | Full safety overhead |
| CoroOptimized | 1,171 | -0.4% | Matches callbacks ⚡ |
| **CoroElidable** | **1,158** | **-1.5%** | **Fastest** ⚡⚡ |
| CoroElidableTask | 1,175 | -0.1% | Alternative placement |

### Two-Level Chains

| Implementation | Time (ns) | vs Callback | Notes |
|----------------|-----------|-------------|-------|
| Callback | 1,376 | baseline | - |
| Coroutine | 1,316 | -4.4% | Better than callbacks ⚡ |
| CoroOptimized | 1,312 | -4.7% | Competitive |
| **CoroElidable** | **1,300** | **-5.5%** | **Fastest** ⚡⚡ |
| CoroElidableTask | 1,306 | -5.1% | Close second |

### Three-Level Complex Chains

| Implementation | Time (ns) | vs Callback | Notes |
|----------------|-----------|-------------|-------|
| Callback | 1,404 | baseline | Callback pyramid |
| Coroutine | 1,329 | -5.3% | Clean code wins ⚡ |
| **CoroOptimized** | **1,299** | **-7.5%** | **Fastest** ⚡⚡ |
| CoroElidable | 1,353 | -3.6% | Good performance |
| CoroElidableTask | 1,372 | -2.3% | Competitive |

## Key Conclusions

### 1. **Coroutines Can Beat Callbacks**
With proper `co_await` composition (not `.get()`), coroutines achieve 4-7% better performance than callbacks in chained operations. This is the realistic use case for async code.

### 2. **Elidable Attributes Are Effective**
`[[clang::coro_await_elidable]]` provides measurable performance gains, especially for simple operations where CoroElidable beats even callbacks by 1.5%.

### 3. **Safety Has Minimal Cost**
The standard Coroutine implementation (with full exception safety) is only 5.4% slower than callbacks for simple operations, but actually outperforms callbacks (-4.4% to -5.3%) in chained scenarios.

### 4. **Complexity Favors Coroutines**
As async operations are chained together:
- Callbacks show the "pyramid of doom" pattern
- Coroutines maintain clean, sequential code
- Performance gap widens in favor of coroutines (up to 7.5% faster)

### 5. **Attribute Placement Matters**
- **Awaiter decoration** (`CoroElidable`): Best for simple ops and chains
- **task decoration** (`CoroElidableTask`): Semantically cleaner, slightly less optimization
- Both are valid; choose based on API design preferences

### 6. **Use `co_await`, Not `.get()`**
All implementations now use `co_await` for composition. Previously, using `.get()` blocked the coroutine benefits. Proper `co_await` usage is essential for realistic benchmarks.

## Optimization Techniques Used

The optimized coroutine implementation demonstrates several techniques:

1. **Removed Exception Overhead**: No `std::exception_ptr` storage
2. **Direct Value Storage**: No `std::optional<T>` wrapper
3. **Noexcept Annotations**: Helps compiler optimize away checks
4. **Simplified Promise Type**: Minimal state in promise_type
5. **Eager Execution**: Uses `suspend_never` for initial_suspend

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
        int result = task.get();
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_YourTest_CoroOptimized);
```

## Practical Recommendations

Based on the benchmark results, here's when to use each approach:

### Use Callbacks When:
- ❌ **Generally not recommended** - coroutines outperform in realistic scenarios
- Interfacing with C APIs that require function pointers
- Working in pre-C++20 codebases

### Use Standard Coroutines (Coroutine) When:
- ✅ **Production code requiring safety** - exception handling and error propagation
- You value correctness over maximum performance
- 5% overhead is acceptable for peace of mind
- **Still faster than callbacks in chained operations!** (-4.4% to -5.3%)

### Use Optimized Coroutines (CoroOptimized) When:
- ✅ **Best balance of performance and clean code**
- Complex async workflows (multiple levels of chaining)
- You handle errors at a higher level (outside the async layer)
- **7.5% faster than callbacks in complex chains**

### Use Elidable Coroutines (CoroElidable) When:
- ✅ **Maximum performance with Clang**
- Simple to medium complexity operations
- You want compiler-assisted optimization
- **1.5% faster than callbacks even in simple operations**
- **5.5% faster in chains**

### Use task-Decorated Elidable (CoroElidableTask) When:
- ✅ **Semantically cleaner API design**
- You want the entire task type marked as elidable
- Prefer declarative over implementation-specific attributes
- **Still competitive with all other approaches**

## General Guidelines

1. **Default to CoroOptimized or CoroElidable** for new projects
2. **Use Standard Coroutine** when safety is paramount
3. **Avoid callbacks** unless interfacing with legacy C code
4. **Always use `co_await`** for composition, never `.get()` in chains
5. **Benchmark your specific use case** - results vary by workload

## License

This is a demonstration project for educational purposes.