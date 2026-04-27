/** bench_vector.cpp
 *
 * STL reference benchmarks for std::vector<int>:
 * append, get, contains_hit, insert_front, remove_front, iterate, clear —
 * at every BENCH_SCALES scale point.
 * insert_front and remove_front are capped at n <= 100 000 (O(n) operations).
 * Output: CSV rows to stdout (matching bench_arraylist.c format).
 */

#include "../bench_harness.h"
#include <algorithm> /* std::find */
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <vector>

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
 * Per-iteration pattern for remove_front / clear: warm_body runs for warm-up,
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
 * append — Pattern A: body = push_back n elements then v.clear()
 * No reserve() — matches cutil ArrayList dynamic resize behaviour.
 * ---------------------------------------------------------------------- */

static void
bench_vector_append(std::size_t n)
{
    std::vector<int> v;
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (int k = 0; k < (int) n; ++k) {
              v.push_back(k);
          }
          bench_do_not_optimize(v.data());
          v.clear();
      },
      [&]() {}
    );
    bench_print_csv_row("ArrayList", "stl", "append", n, med);
}

/* -------------------------------------------------------------------------
 * get — Pattern B: pre-populated, sequential index access
 * ---------------------------------------------------------------------- */

static void
bench_vector_get(std::size_t n)
{
    std::vector<int> v;
    for (int k = 0; k < (int) n; ++k) {
        v.push_back(k);
    }
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (std::size_t i = 0; i < n; ++i) {
              int val = v[i];
              bench_do_not_optimize(&val);
          }
      },
      [&]() {}
    );
    bench_print_csv_row("ArrayList", "stl", "get", n, med);
}

/* -------------------------------------------------------------------------
 * contains_hit — Pattern B: std::find for worst-case needle (last element)
 * One find call per timed iteration — forces full linear scan.
 * ---------------------------------------------------------------------- */

static void
bench_vector_contains_hit(std::size_t n)
{
    std::vector<int> v;
    for (int k = 0; k < (int) n; ++k) {
        v.push_back(k);
    }
    int needle = (int) n - 1;
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          auto it = std::find(v.begin(), v.end(), needle);
          if (it != v.end()) {
              bench_do_not_optimize(&(*it));
          }
      },
      [&]() {}
    );
    bench_print_csv_row("ArrayList", "stl", "contains_hit", n, med);
}

/* -------------------------------------------------------------------------
 * insert_front — Pattern A, capped at n <= 100 000 (O(n) shift per insert)
 * body = insert n elements at begin() then v.clear()
 * ---------------------------------------------------------------------- */

static void
bench_vector_insert_front(std::size_t n)
{
    if (n > 100000) {
        return; /* skip large scale points for O(n²) operation */
    }

    std::vector<int> v;
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (int k = 0; k < (int) n; ++k) {
              v.insert(v.begin(), k);
          }
          bench_do_not_optimize(v.data());
          v.clear();
      },
      [&]() {}
    );
    bench_print_csv_row("ArrayList", "stl", "insert_front", n, med);
}

/* -------------------------------------------------------------------------
 * remove_front — per-iter, capped at n <= 100 000 (O(n) shift per erase)
 * pre: populate n elements; op: erase from begin() n times
 * ---------------------------------------------------------------------- */

static void
bench_vector_remove_front(std::size_t n)
{
    if (n > 100000) {
        return; /* skip large scale points for O(n²) operation */
    }

    std::vector<int> v;
    int64_t med = timed_bench_per_iter(
      [&]() {
          v.clear();
          for (int k = 0; k < (int) n; ++k) {
              v.push_back(k);
          }
          for (std::size_t i = 0; i < n; ++i) {
              v.erase(v.begin());
          }
          bench_do_not_optimize(v.data());
      },
      [&]() {
          v.clear();
          for (int k = 0; k < (int) n; ++k) {
              v.push_back(k);
          }
      },
      [&]() {
          for (std::size_t i = 0; i < n; ++i) {
              v.erase(v.begin());
          }
          bench_do_not_optimize(v.data());
      }
    );
    bench_print_csv_row("ArrayList", "stl", "remove_front", n, med);
}

/* -------------------------------------------------------------------------
 * iterate — Pattern B: range-for over pre-populated vector
 * ---------------------------------------------------------------------- */

static void
bench_vector_iterate(std::size_t n)
{
    std::vector<int> v;
    for (int k = 0; k < (int) n; ++k) {
        v.push_back(k);
    }
    int64_t med = timed_bench(
      [&]() {},
      [&]() {
          for (const auto &val : v) {
              bench_do_not_optimize((void *) &val);
          }
      },
      [&]() {}
    );
    bench_print_csv_row("ArrayList", "stl", "iterate", n, med);
}

/* -------------------------------------------------------------------------
 * clear — per-iter: populate before timing, time only v.clear()
 * ---------------------------------------------------------------------- */

static void
bench_vector_clear(std::size_t n)
{
    std::vector<int> v;
    int64_t med = timed_bench_per_iter(
      [&]() {
          v.clear();
          for (int k = 0; k < (int) n; ++k) {
              v.push_back(k);
          }
          v.clear();
          bench_do_not_optimize(v.data());
      },
      [&]() {
          v.clear();
          for (int k = 0; k < (int) n; ++k) {
              v.push_back(k);
          }
      },
      [&]() {
          v.clear();
          bench_do_not_optimize(v.data());
      }
    );
    bench_print_csv_row("ArrayList", "stl", "clear", n, med);
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
        bench_vector_append(n);
        bench_vector_get(n);
        bench_vector_contains_hit(n);
        bench_vector_insert_front(n);
        bench_vector_remove_front(n);
        bench_vector_iterate(n);
        bench_vector_clear(n);
    }
    return 0;
}
