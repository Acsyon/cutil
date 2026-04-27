/** bench_hashset.c
 *
 * Benchmarks for cutil_HashSet: add, contains_hit, contains_miss, remove,
 * iterate, clear — at every BENCH_SCALES scale point.
 * Output: CSV rows to stdout.
 */

#include "../bench_harness.h"
#include <cutil/data/generic/iterator.h>
#include <cutil/data/generic/set.h>
#include <cutil/data/generic/set/hashset.h>
#include <cutil/data/generic/type.h>

/* -------------------------------------------------------------------------
 * add — Pattern A: populate + clear inside body
 * ---------------------------------------------------------------------- */
static void
bench_hashset_add(size_t n)
{
    BENCH_RUN(
      "HashSet", "cutil", "add", n,
      cutil_Set *set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
      ,
      {
          for (int k = 0; k < (int) n; ++k) {
              cutil_Set_add(set, &k);
          }
          bench_do_not_optimize(set);
          cutil_Set_reset(set);
      },
      { cutil_Set_free(set); }
    );
}

/* -------------------------------------------------------------------------
 * contains_hit — Pattern B: all elements present
 * ---------------------------------------------------------------------- */
static void
bench_hashset_contains_hit(size_t n)
{
    BENCH_RUN(
      "HashSet", "cutil", "contains_hit", n,
      cutil_Set *set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Set_add(set, &k); },
      {
          for (int k = 0; k < (int) n; ++k) {
              cutil_Bool b = cutil_Set_contains(set, &k);
              bench_do_not_optimize(&b);
          }
      },
      { cutil_Set_free(set); }
    );
}

/* -------------------------------------------------------------------------
 * contains_miss — Pattern B: query elements [n, 2n) — all absent
 * ---------------------------------------------------------------------- */
static void
bench_hashset_contains_miss(size_t n)
{
    BENCH_RUN(
      "HashSet", "cutil", "contains_miss", n,
      cutil_Set *set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Set_add(set, &k); },
      {
          for (int k = (int) n; k < 2 * (int) n; ++k) {
              cutil_Bool b = cutil_Set_contains(set, &k);
              bench_do_not_optimize(&b);
          }
      },
      { cutil_Set_free(set); }
    );
}

/* -------------------------------------------------------------------------
 * remove — per-iteration alloc/populate/remove/free
 * ---------------------------------------------------------------------- */
static void
bench_hashset_remove(size_t n)
{
    int64_t times[BENCH_TIMED_RUNS];

    /* warm-up */
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        cutil_Set *s = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Set_add(s, &k);
        }
        for (int k = 0; k < (int) n; ++k) {
            cutil_Set_remove(s, &k);
        }
        bench_do_not_optimize(s);
        cutil_Set_free(s);
    }
    /* timed runs */
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        cutil_Set *s = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Set_add(s, &k);
        }
        int64_t t0 = bench_now_ns();
        for (int k = 0; k < (int) n; ++k) {
            cutil_Set_remove(s, &k);
        }
        int64_t t1 = bench_now_ns();
        bench_do_not_optimize(s);
        cutil_Set_free(s);
        times[t] = bench_elapsed_ns(t0, t1);
    }
    int64_t med = bench_median(times, BENCH_TIMED_RUNS);
    bench_print_csv_row("HashSet", "cutil", "remove", n, med);
}

/* -------------------------------------------------------------------------
 * iterate — Pattern B: alloc+populate in setup, create+free iterator in body
 * ---------------------------------------------------------------------- */
static void
bench_hashset_iterate(size_t n)
{
    BENCH_RUN(
      "HashSet", "cutil", "iterate", n,
      cutil_Set *set = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Set_add(set, &k); },
      {
          cutil_ConstIterator *it = cutil_Set_get_const_iterator(set);
          while (cutil_ConstIterator_next(it)) {
              const void *v = cutil_ConstIterator_get_ptr(it);
              bench_do_not_optimize((void *) v);
          }
          cutil_ConstIterator_free(it);
      },
      { cutil_Set_free(set); }
    );
}

/* -------------------------------------------------------------------------
 * clear — per-iteration alloc/populate/clear/free (time only clear)
 * ---------------------------------------------------------------------- */
static void
bench_hashset_clear(size_t n)
{
    int64_t times[BENCH_TIMED_RUNS];

    /* warm-up */
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        cutil_Set *s = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Set_add(s, &k);
        }
        cutil_Set_clear(s);
        bench_do_not_optimize(s);
        cutil_Set_free(s);
    }
    /* timed runs */
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        cutil_Set *s = cutil_HashSet_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Set_add(s, &k);
        }
        int64_t t0 = bench_now_ns();
        cutil_Set_clear(s);
        int64_t t1 = bench_now_ns();
        bench_do_not_optimize(s);
        cutil_Set_free(s);
        times[t] = bench_elapsed_ns(t0, t1);
    }
    int64_t med = bench_median(times, BENCH_TIMED_RUNS);
    bench_print_csv_row("HashSet", "cutil", "clear", n, med);
}

/* -------------------------------------------------------------------------
 * main
 * ---------------------------------------------------------------------- */
int
main(void)
{
    bench_print_csv_header();
    for (size_t si = 0; si < BENCH_SCALES_LEN; ++si) {
        const size_t n = BENCH_SCALES[si];
        bench_hashset_add(n);
        bench_hashset_contains_hit(n);
        bench_hashset_contains_miss(n);
        bench_hashset_remove(n);
        bench_hashset_iterate(n);
        bench_hashset_clear(n);
    }
    return 0;
}
