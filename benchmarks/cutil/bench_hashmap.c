/** bench_hashmap.c
 *
 * Benchmarks for cutil_HashMap: insert, lookup, contains_hit, contains_miss,
 * remove, iterate, clear — at every BENCH_SCALES scale point.
 * Output: CSV rows to stdout.
 */

#include "../bench_harness.h"
#include <cutil/data/generic/iterator.h>
#include <cutil/data/generic/map.h>
#include <cutil/data/generic/map/hashmap.h>
#include <cutil/data/generic/type.h>

/* -------------------------------------------------------------------------
 * insert — Pattern A: populate + clear inside body
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_insert(size_t n)
{
    BENCH_RUN(
      "HashMap", "cutil", "insert", n,
      cutil_Map *map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
      ,
      {
          for (int k = 0; k < (int) n; ++k) {
              cutil_Map_set(map, &k, &k);
          }
          bench_do_not_optimize(map);
          cutil_Map_reset(map);
      },
      { cutil_Map_free(map); }
    );
}

/* -------------------------------------------------------------------------
 * lookup — Pattern B: pre-populated, look up all keys
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_lookup(size_t n)
{
    BENCH_RUN(
      "HashMap", "cutil", "lookup", n,
      cutil_Map *map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Map_set(map, &k, &k); },
      {
          for (int k = 0; k < (int) n; ++k) {
              const void *v = cutil_Map_get_ptr(map, &k);
              bench_do_not_optimize((void *) v);
          }
      },
      { cutil_Map_free(map); }
    );
}

/* -------------------------------------------------------------------------
 * contains_hit — Pattern B: all keys present
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_contains_hit(size_t n)
{
    BENCH_RUN(
      "HashMap", "cutil", "contains_hit", n,
      cutil_Map *map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Map_set(map, &k, &k); },
      {
          for (int k = 0; k < (int) n; ++k) {
              cutil_Bool b = cutil_Map_contains(map, &k);
              bench_do_not_optimize(&b);
          }
      },
      { cutil_Map_free(map); }
    );
}

/* -------------------------------------------------------------------------
 * contains_miss — Pattern B: query keys [n, 2n) — all absent
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_contains_miss(size_t n)
{
    BENCH_RUN(
      "HashMap", "cutil", "contains_miss", n,
      cutil_Map *map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Map_set(map, &k, &k); },
      {
          for (int k = (int) n; k < 2 * (int) n; ++k) {
              cutil_Bool b = cutil_Map_contains(map, &k);
              bench_do_not_optimize(&b);
          }
      },
      { cutil_Map_free(map); }
    );
}

/* -------------------------------------------------------------------------
 * remove — per-iteration alloc/populate/remove/free
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_remove(size_t n)
{
    int64_t times[BENCH_TIMED_RUNS];

    /* warm-up */
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        cutil_Map *m
          = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Map_set(m, &k, &k);
        }
        for (int k = 0; k < (int) n; ++k) {
            cutil_Map_remove(m, &k);
        }
        bench_do_not_optimize(m);
        cutil_Map_free(m);
    }
    /* timed runs */
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        cutil_Map *m
          = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Map_set(m, &k, &k);
        }
        int64_t t0 = bench_now_ns();
        for (int k = 0; k < (int) n; ++k) {
            cutil_Map_remove(m, &k);
        }
        int64_t t1 = bench_now_ns();
        bench_do_not_optimize(m);
        cutil_Map_free(m);
        times[t] = bench_elapsed_ns(t0, t1);
    }
    int64_t med = bench_median(times, BENCH_TIMED_RUNS);
    bench_print_csv_row("HashMap", "cutil", "remove", n, med);
}

/* -------------------------------------------------------------------------
 * iterate — Pattern B: alloc+populate in setup, create+free iterator in body
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_iterate(size_t n)
{
    BENCH_RUN(
      "HashMap", "cutil", "iterate", n,
      cutil_Map *map
      = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_Map_set(map, &k, &k); },
      {
          cutil_ConstIterator *it = cutil_Map_get_const_iterator(map);
          while (cutil_ConstIterator_next(it)) {
              const void *v = cutil_ConstIterator_get_ptr(it);
              bench_do_not_optimize((void *) v);
          }
          cutil_ConstIterator_free(it);
      },
      { cutil_Map_free(map); }
    );
}

/* -------------------------------------------------------------------------
 * clear — per-iteration alloc/populate/clear/free (time only clear)
 * ---------------------------------------------------------------------- */
static void
bench_hashmap_clear(size_t n)
{
    int64_t times[BENCH_TIMED_RUNS];

    /* warm-up */
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        cutil_Map *m
          = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Map_set(m, &k, &k);
        }
        cutil_Map_clear(m);
        bench_do_not_optimize(m);
        cutil_Map_free(m);
    }
    /* timed runs */
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        cutil_Map *m
          = cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_Map_set(m, &k, &k);
        }
        int64_t t0 = bench_now_ns();
        cutil_Map_clear(m);
        int64_t t1 = bench_now_ns();
        bench_do_not_optimize(m);
        cutil_Map_free(m);
        times[t] = bench_elapsed_ns(t0, t1);
    }
    int64_t med = bench_median(times, BENCH_TIMED_RUNS);
    bench_print_csv_row("HashMap", "cutil", "clear", n, med);
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
        bench_hashmap_insert(n);
        bench_hashmap_lookup(n);
        bench_hashmap_contains_hit(n);
        bench_hashmap_contains_miss(n);
        bench_hashmap_remove(n);
        bench_hashmap_iterate(n);
        bench_hashmap_clear(n);
    }
    return 0;
}
