/** bench_arraylist.c
 *
 * Benchmarks for cutil_ArrayList: append, get, contains_hit, insert_front,
 * remove_front, iterate, clear — at every BENCH_SCALES scale point.
 * insert_front and remove_front are capped at n <= 100 000 (O(n) operations).
 * Output: CSV rows to stdout.
 */

#include "../bench_harness.h"
#include <cutil/data/generic/iterator.h>
#include <cutil/data/generic/list.h>
#include <cutil/data/generic/list/arraylist.h>
#include <cutil/data/generic/type.h>

/* -------------------------------------------------------------------------
 * append — Pattern A: populate + clear inside body
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_append(size_t n)
{
    BENCH_RUN(
      "ArrayList", "cutil", "append", n,
      cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
      ,
      {
          for (int k = 0; k < (int) n; ++k) {
              cutil_List_append(list, &k);
          }
          bench_do_not_optimize(list);
          cutil_List_reset(list);
      },
      { cutil_List_free(list); }
    );
}

/* -------------------------------------------------------------------------
 * get — Pattern B: pre-populated, sequential random-access scan
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_get(size_t n)
{
    BENCH_RUN(
      "ArrayList", "cutil", "get", n,
      cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_List_append(list, &k); },
      {
          for (size_t i = 0; i < n; ++i) {
              const void *v = cutil_List_get_ptr(list, i);
              bench_do_not_optimize((void *) v);
          }
      },
      { cutil_List_free(list); }
    );
}

/* -------------------------------------------------------------------------
 * contains_hit — Pattern B: worst-case needle at last position
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_contains_hit(size_t n)
{
    BENCH_RUN(
      "ArrayList", "cutil", "contains_hit", n,
      cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n;
           ++k) { cutil_List_append(list, &k); } int needle = (int) n - 1;
      ,
      {
          cutil_Bool b = cutil_List_contains(list, &needle);
          bench_do_not_optimize(&b);
      },
      { cutil_List_free(list); }
    );
}

/* -------------------------------------------------------------------------
 * insert_front — Pattern A, capped at n <= 100 000 (O(n) array shift)
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_insert_front(size_t n)
{
    if (n > 100000) {
        return;
    } /* skip large scale points for O(n) op */

    BENCH_RUN(
      "ArrayList", "cutil", "insert_front", n,
      cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
      ,
      {
          for (int k = 0; k < (int) n; ++k) {
              cutil_List_insert(list, 0, &k);
          }
          bench_do_not_optimize(list);
          cutil_List_reset(list);
      },
      { cutil_List_free(list); }
    );
}

/* -------------------------------------------------------------------------
 * remove_front — per-iteration alloc/populate/remove/free, capped at 100 000
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_remove_front(size_t n)
{
    if (n > 100000) {
        return;
    } /* skip large scale points for O(n) op */

    int64_t times[BENCH_TIMED_RUNS];

    /* warm-up */
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_List_append(list, &k);
        }
        for (size_t i = 0; i < n; ++i) {
            cutil_List_remove(list, 0);
        }
        bench_do_not_optimize(list);
        cutil_List_free(list);
    }
    /* timed runs */
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_List_append(list, &k);
        }
        int64_t t0 = bench_now_ns();
        for (size_t i = 0; i < n; ++i) {
            cutil_List_remove(list, 0);
        }
        int64_t t1 = bench_now_ns();
        bench_do_not_optimize(list);
        cutil_List_free(list);
        times[t] = bench_elapsed_ns(t0, t1);
    }
    int64_t med = bench_median(times, BENCH_TIMED_RUNS);
    bench_print_csv_row("ArrayList", "cutil", "remove_front", n, med);
}

/* -------------------------------------------------------------------------
 * iterate — Pattern B: alloc+populate in setup, create+free iterator in body
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_iterate(size_t n)
{
    BENCH_RUN(
      "ArrayList", "cutil", "iterate", n,
      cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
      for (int k = 0; k < (int) n; ++k) { cutil_List_append(list, &k); },
      {
          cutil_ConstIterator *it = cutil_List_get_const_iterator(list);
          while (cutil_ConstIterator_next(it)) {
              const void *v = cutil_ConstIterator_get_ptr(it);
              bench_do_not_optimize((void *) v);
          }
          cutil_ConstIterator_free(it);
      },
      { cutil_List_free(list); }
    );
}

/* -------------------------------------------------------------------------
 * clear — per-iteration alloc/populate/clear/free (time only clear)
 * ---------------------------------------------------------------------- */
static void
bench_arraylist_clear(size_t n)
{
    int64_t times[BENCH_TIMED_RUNS];

    /* warm-up */
    for (int w = 0; w < BENCH_WARMUP_RUNS; ++w) {
        cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_List_append(list, &k);
        }
        cutil_List_clear(list);
        bench_do_not_optimize(list);
        cutil_List_free(list);
    }
    /* timed runs */
    for (int t = 0; t < BENCH_TIMED_RUNS; ++t) {
        cutil_List *list = cutil_ArrayList_alloc(CUTIL_GENERIC_TYPE_INT);
        for (int k = 0; k < (int) n; ++k) {
            cutil_List_append(list, &k);
        }
        int64_t t0 = bench_now_ns();
        cutil_List_clear(list);
        int64_t t1 = bench_now_ns();
        bench_do_not_optimize(list);
        cutil_List_free(list);
        times[t] = bench_elapsed_ns(t0, t1);
    }
    int64_t med = bench_median(times, BENCH_TIMED_RUNS);
    bench_print_csv_row("ArrayList", "cutil", "clear", n, med);
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
        bench_arraylist_append(n);
        bench_arraylist_get(n);
        bench_arraylist_contains_hit(n);
        bench_arraylist_insert_front(n);
        bench_arraylist_remove_front(n);
        bench_arraylist_iterate(n);
        bench_arraylist_clear(n);
    }
    return 0;
}
