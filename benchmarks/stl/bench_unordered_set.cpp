/** bench_unordered_set.cpp
 *
 * STL reference benchmarks for std::unordered_set<int>:
 * add, contains_hit, contains_miss, remove, iterate, clear —
 * at every BENCH_SCALES scale point.
 * Output: CSV rows to stdout (matching bench_hashset.c format).
 */

#include "../bench_harness.h"
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <unordered_set>

/* -------------------------------------------------------------------------
 * Timing helper — C++17 steady_clock
 * ---------------------------------------------------------------------- */

static inline int64_t
stl_now_ns()
{
    using namespace std::chrono;
    return static_cast<int64_t>(
      duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count()
    );
}

/* -------------------------------------------------------------------------
 * Benchmark helper templates
 * ---------------------------------------------------------------------- */

/**
 * Pattern A / B: setup runs once (un-timed) before all iterations,
 * body is timed BENCH_TIMED_RUNS times, teardown runs once at the end.
 */
template <typename Setup, typename Body, typename Teardown>
static int64_t
timed_bench(Setup setup, Body body, Teardown teardown)
{
    int64_t times[BENCH_TIMED_RUNS];
    setup();
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        body();
    }
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        int64_t t0 = stl_now_ns();
        body();
        int64_t t1 = stl_now_ns();
        times[t] = t1 - t0;
    }
    teardown();
    return bench_median(times, BENCH_TIMED_RUNS);
}

/**
 * Per-iteration pattern for remove / clear: warm_body runs for warm-up,
 * then pre() (un-timed populate) and op() (timed operation) alternate.
 * Matches the cutil benchmark timing: only the destructive operation is timed.
 */
template <typename WarmBody, typename Pre, typename Op>
static int64_t
timed_bench_per_iter(WarmBody warm_body, Pre pre, Op op)
{
    int64_t times[BENCH_TIMED_RUNS];
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        warm_body();
    }
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        pre();
        int64_t t0 = stl_now_ns();
        op();
        int64_t t1 = stl_now_ns();
        times[t] = t1 - t0;
    }
    return bench_median(times, BENCH_TIMED_RUNS);
}

/* -------------------------------------------------------------------------
 * add — Pattern A: body = insert n elements then s.clear()
 * ---------------------------------------------------------------------- */

static void
bench_unordered_set_add(std::size_t n)
{
    std::unordered_set<int> s;
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (int k = 0; k < (int) n; ++k) {
              s.insert(k);
          }
          bench_do_not_optimize(&s);
          s.clear();
      },
      [&]() {}
    );
    bench_print_csv_row("HashSet", "stl", "add", n, med);
}

/* -------------------------------------------------------------------------
 * contains_hit — Pattern B: count() for all present elements 0..n-1
 * ---------------------------------------------------------------------- */

static void
bench_unordered_set_contains_hit(std::size_t n)
{
    std::unordered_set<int> s;
    for (int k = 0; k < (int) n; ++k) {
        s.insert(k);
    }
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (int k = 0; k < (int) n; ++k) {
              std::size_t c = s.count(k);
              bench_do_not_optimize(&c);
          }
      },
      [&]() {}
    );
    bench_print_csv_row("HashSet", "stl", "contains_hit", n, med);
}

/* -------------------------------------------------------------------------
 * contains_miss — Pattern B: count() for absent elements n..2n-1
 * ---------------------------------------------------------------------- */

static void
bench_unordered_set_contains_miss(std::size_t n)
{
    std::unordered_set<int> s;
    for (int k = 0; k < (int) n; ++k) {
        s.insert(k);
    }
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (int k = (int) n; k < 2 * (int) n; ++k) {
              std::size_t c = s.count(k);
              bench_do_not_optimize(&c);
          }
      },
      [&]() {}
    );
    bench_print_csv_row("HashSet", "stl", "contains_miss", n, med);
}

/* -------------------------------------------------------------------------
 * remove — per-iter: populate before timing, time only erase loop
 * ---------------------------------------------------------------------- */

static void
bench_unordered_set_remove(std::size_t n)
{
    std::unordered_set<int> s;
    int64_t med = timed_bench_per_iter(
      [&]() {
          s.clear();
          for (int k = 0; k < (int) n; ++k) {
              s.insert(k);
          }
          for (int k = 0; k < (int) n; ++k) {
              s.erase(k);
          }
          bench_do_not_optimize(&s);
      },
      [&]() {
          s.clear();
          for (int k = 0; k < (int) n; ++k) {
              s.insert(k);
          }
      },
      [&]() {
          for (int k = 0; k < (int) n; ++k) {
              s.erase(k);
          }
          bench_do_not_optimize(&s);
      }
    );
    bench_print_csv_row("HashSet", "stl", "remove", n, med);
}

/* -------------------------------------------------------------------------
 * iterate — Pattern B: range-for over pre-populated set
 * ---------------------------------------------------------------------- */

static void
bench_unordered_set_iterate(std::size_t n)
{
    std::unordered_set<int> s;
    for (int k = 0; k < (int) n; ++k) {
        s.insert(k);
    }
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (const auto &v : s) {
              bench_do_not_optimize((void *) &v);
          }
      },
      [&]() {}
    );
    bench_print_csv_row("HashSet", "stl", "iterate", n, med);
}

/* -------------------------------------------------------------------------
 * clear — per-iter: populate before timing, time only s.clear()
 * ---------------------------------------------------------------------- */

static void
bench_unordered_set_clear(std::size_t n)
{
    std::unordered_set<int> s;
    int64_t med = timed_bench_per_iter(
      [&]() {
          s.clear();
          for (int k = 0; k < (int) n; ++k) {
              s.insert(k);
          }
          s.clear();
          bench_do_not_optimize(&s);
      },
      [&]() {
          s.clear();
          for (int k = 0; k < (int) n; ++k) {
              s.insert(k);
          }
      },
      [&]() {
          s.clear();
          bench_do_not_optimize(&s);
      }
    );
    bench_print_csv_row("HashSet", "stl", "clear", n, med);
}

/* -------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------- */

int
main()
{
    bench_print_csv_header();
    for (std::size_t si = 0; si < BENCH_SCALES_LEN; ++si) {
        const std::size_t n = BENCH_SCALES[si];
        bench_unordered_set_add(n);
        bench_unordered_set_contains_hit(n);
        bench_unordered_set_contains_miss(n);
        bench_unordered_set_remove(n);
        bench_unordered_set_iterate(n);
        bench_unordered_set_clear(n);
    }
    return 0;
}
