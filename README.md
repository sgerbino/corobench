# AsyncBench - Coroutines vs Callbacks Performance Comparison

A C++26 project that benchmarks the performance of C++20 coroutines against traditional callback-based async patterns using Google Benchmark.

## Project Structure

```
asyncbench/
├── CMakeLists.txt              # CMake configuration
├── include/
│   ├── coroutine_async.hpp     # Coroutine-based async implementation
│   └── callback_async.hpp      # Callback-based async implementation
└── src/
    └── benchmark_main.cpp      # Benchmark tests
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
./asyncbench
```

### Benchmark Options

Google Benchmark provides various command-line options:

```bash
# Run specific benchmarks matching a pattern
./asyncbench --benchmark_filter=Coroutine

# Output results in JSON format
./asyncbench --benchmark_format=json

# Run benchmarks for a minimum time
./asyncbench --benchmark_min_time=5.0

# Display all options
./asyncbench --help
```

## Benchmarks Included

1. **BM_Coroutine_Simple / BM_Callback_Simple**
   - Simple async computation comparison

2. **BM_Coroutine_Chain / BM_Callback_Chain**
   - Two-level async operation chaining

3. **BM_Coroutine_ComplexChain / BM_Callback_ComplexChain**
   - Three-level async operation chaining (tests callback pyramid)

4. **BM_Coroutine_VaryingLoad / BM_Callback_VaryingLoad**
   - Performance across different workload sizes (8 to 8192)

## Implementation Details

### Coroutine Implementation
- Uses C++20 coroutines with custom `Task<T>` type
- Implements promise_type for coroutine infrastructure
- Provides clean, sequential-looking async code

### Callback Implementation
- Traditional callback-based approach using `std::function`
- Demonstrates callback nesting for chained operations
- Shows the "callback pyramid" pattern

## Expected Results

Performance characteristics may vary based on:
- Compiler optimizations
- Workload complexity
- Chain depth
- Platform and hardware

Generally:
- Simple operations: Similar performance
- Deep chains: Coroutines may have better composability with minimal overhead
- Callbacks: More predictable but can suffer from nested complexity

## Customizing Benchmarks

To add new benchmarks, edit `src/benchmark_main.cpp` and add new benchmark functions:

```cpp
static void BM_YourBenchmark(benchmark::State& state) {
    for (auto _ : state) {
        // Your code here
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_YourBenchmark);
```

## License

This is a demonstration project for educational purposes.
