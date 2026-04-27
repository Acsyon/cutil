/** bench_harness.h
 *
 * Shared benchmark harness: timing, iteration loops, scale points, CSV output.
 * Compatible with C11 and C++17. Include in every benchmark source file.
 */

#ifndef BENCH_HARNESS_H
#define BENCH_HARNESS_H

/* Ensure struct timespec / clock_gettime are declared (POSIX.1b, C11 Annex K)
 */
#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h> /* PRId64 */
#include <stddef.h>   /* size_t */
#include <stdint.h>   /* int64_t */
#include <stdio.h>    /* printf */
#include <time.h>     /* clock_gettime, struct timespec, CLOCK_MONOTONIC */

/* -------------------------------------------------------------------------
 * Tunable constants (overridable via -D at compile time)
 * ---------------------------------------------------------------------- */

/** Number of untimed warm-up iterations before each timed run. */
#ifndef BENCH_WARMUP_RUNS
    #define BENCH_WARMUP_RUNS 3
#endif

/** Number of timed iterations per (operation, scale-point) pair. */
#ifndef BENCH_TIMED_RUNS
    #define BENCH_TIMED_RUNS 5
#endif

/* -------------------------------------------------------------------------
 * Scale-point array
 * static prevents multiple-definition errors when included in multiple TUs.
 * ---------------------------------------------------------------------- */

/** Logarithmic scale points: 10, 100, ..., 10 000 000 */
static const size_t BENCH_SCALES[]
  = {10, 100, 1000, 10000, 100000, 1000000, 10000000};
static const size_t BENCH_SCALES_LEN
  = sizeof(BENCH_SCALES) / sizeof(BENCH_SCALES[0]);

/* -------------------------------------------------------------------------
 * Timing helpers
 * ---------------------------------------------------------------------- */

/** Returns current monotonic time as nanoseconds since some fixed epoch. */
static inline int64_t
bench_now_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t) ts.tv_sec * 1000000000LL + (int64_t) ts.tv_nsec;
}

/** Returns elapsed nanoseconds between two bench_now_ns() samples. */
static inline int64_t
bench_elapsed_ns(int64_t start, int64_t end)
{
    return end - start;
}

/* -------------------------------------------------------------------------
 * Compiler barrier
 * Prevents the compiler from eliding reads/writes to *ptr.
 * GCC/Clang on Linux: inline asm clobber.
 * ---------------------------------------------------------------------- */
static inline void
bench_do_not_optimize(void *ptr)
{
    __asm__ volatile("" : : "r"(ptr) : "memory");
}

/* -------------------------------------------------------------------------
 * Median computation
 * ---------------------------------------------------------------------- */

/**
 * Returns the median of arr[0..len-1] using an in-place insertion sort.
 * len should equal BENCH_TIMED_RUNS (small; insertion sort is fine).
 * The caller's array is sorted in-place.
 */
static inline int64_t
bench_median(int64_t *arr, size_t len)
{
    for (size_t i = 1; i < len; ++i) {
        int64_t key = arr[i];
        size_t j = i;
        while (j > 0 && arr[j - 1] > key) {
            arr[j] = arr[j - 1];
            --j;
        }
        arr[j] = key;
    }
    return arr[len / 2];
}

/* -------------------------------------------------------------------------
 * CSV output helpers
 * ---------------------------------------------------------------------- */

/**
 * Prints the CSV header row to stdout.
 * Call once at the start of main() in each benchmark executable.
 */
static inline void
bench_print_csv_header(void)
{
    printf("container,impl,operation,n,elapsed_ns,ops_per_sec\n");
}

/**
 * Prints one CSV data row to stdout.
 *
 * @param container  e.g. "HashMap", "HashSet", "ArrayList"
 * @param impl       "cutil" or "stl"
 * @param operation  e.g. "insert", "lookup", "iterate"
 * @param n          scale point
 * @param elapsed_ns median elapsed nanoseconds for that (op, n) run
 */
static inline void
bench_print_csv_row(
  const char *container,
  const char *impl,
  const char *operation,
  size_t n,
  int64_t elapsed_ns
)
{
    double ops_per_sec
      = (elapsed_ns > 0) ? ((double) n * 1.0e9 / (double) elapsed_ns) : 0.0;
    printf(
      "%s,%s,%s,%zu,%" PRId64 ",%.2f\n", container, impl, operation, n,
      elapsed_ns, ops_per_sec
    );
}

/* -------------------------------------------------------------------------
 * Iteration loop macro
 * ---------------------------------------------------------------------- */

/**
 * BENCH_RUN(container, impl, operation, n, setup, body, teardown)
 *
 * Executes:
 *   1. setup  — un-timed, once; variables declared here are visible in
 * body/teardown
 *   2. BENCH_WARMUP_RUNS untimed repetitions of body
 *   3. BENCH_TIMED_RUNS  timed   repetitions of body
 *   4. Computes median elapsed, prints one CSV row
 *   5. teardown — un-timed, once
 *
 * All arguments are bare brace-enclosed statement lists ending with ';'.
 *
 * Example:
 *   BENCH_RUN("HashMap", "cutil", "insert", n,
 *       { cutil_Map *m = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT,
 *                                            CUTIL_GENERIC_TYPE_INT); },
 *       { for (int k = 0; k < (int)n; ++k) { cutil_Map_set(m, &k, &k); }
 *         cutil_Map_clear(m); },
 *       { cutil_Map_free(m); }
 *   );
 */
#define BENCH_RUN(container_, impl_, operation_, n_, setup_, body_, teardown_) \
    do {                                                                       \
        int64_t _times[BENCH_TIMED_RUNS];                                      \
        setup_ for (int _w = 0; _w < BENCH_WARMUP_RUNS; ++_w)                  \
        {                                                                      \
            body_                                                              \
        }                                                                      \
        for (int _t = 0; _t < BENCH_TIMED_RUNS; ++_t) {                        \
            int64_t _t0 = bench_now_ns();                                      \
            body_ int64_t _t1 = bench_now_ns();                                \
            _times[_t] = bench_elapsed_ns(_t0, _t1);                           \
        }                                                                      \
        teardown_ int64_t _med = bench_median(_times, BENCH_TIMED_RUNS);       \
        bench_print_csv_row((container_), (impl_), (operation_), (n_), _med);  \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* BENCH_HARNESS_H */
