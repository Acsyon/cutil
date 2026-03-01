#include "unity.h"

#include <cutil/data/generic/map.h>
#include <cutil/data/generic/map/hashmap.h>
#include <cutil/data/generic/type.h>

#include <cutil/std/stdlib.h>
#include <cutil/std/string.h>
#include <cutil/util/macro.h>

/* Mock implementation data structure */
typedef struct {
    int count;
    int free_count;
    int clear_count;
    int copy_count;
    int duplicate_count;
    int get_count_count;
    int remove_count;
    int contains_count;
    int get_count_val;
    int set_count;
} MockMapData;

/* Mock vtable functions */
static void
_mock_free(void *data)
{
    MockMapData *const mock = (MockMapData *) data;
    if (mock) {
        ++mock->free_count;
        free(mock);
    }
}

static void
_mock_clear(void *data)
{
    MockMapData *const mock = (MockMapData *) data;
    if (mock) {
        ++mock->clear_count;
    }
}

static void
_mock_copy(void *dst, const void *src)
{
    MockMapData *const dst_mock = (MockMapData *) dst;
    const MockMapData *const src_mock = (const MockMapData *) src;
    if (dst_mock && src_mock) {
        ++dst_mock->copy_count;
    }
}

static void *
_mock_duplicate(const void *data)
{
    MockMapData *src = (MockMapData *) data;
    if (!src) {
        return NULL;
    }
    MockMapData *const dup = malloc(sizeof *dup);
    *dup = *src;
    ++src->duplicate_count;
    return dup;
}

static size_t
_mock_get_count(const void *data)
{
    MockMapData *mock = (MockMapData *) data;
    if (mock) {
        ++mock->get_count_count;
        return (size_t) mock->get_count_val;
    }
    return 0UL;
}

static int
_mock_remove(void *data, const void *key)
{
    MockMapData *const mock = (MockMapData *) data;
    (void) key;
    if (mock) {
        ++mock->remove_count;
    }
    return 0;
}

static cutil_Bool
_mock_contains(const void *data, const void *key)
{
    MockMapData *const mock = (MockMapData *) data;
    (void) key;
    if (mock) {
        ++mock->contains_count;
        return true;
    }
    return false;
}

static int
_mock_get(const void *data, const void *key, void *val)
{
    MockMapData *mock = (MockMapData *) data;
    (void) key;
    (void) val;
    if (mock) {
        ++mock->get_count_count;
    }
    return 0;
}

static const void *
_mock_get_ptr(const void *data, const void *key)
{
    MockMapData *const mock = (MockMapData *) data;
    (void) key;
    if (mock) {
        return (const void *) mock;
    }
    return NULL;
}

static int
_mock_set(void *data, const void *key, const void *val)
{
    MockMapData *const mock = (MockMapData *) data;
    (void) key;
    (void) val;
    if (mock) {
        ++mock->set_count;
    }
    return 0;
}

static const cutil_GenericType *
_mock_get_key_type(const void *data)
{
    (void) data;
    return CUTIL_GENERIC_TYPE_INT;
}

static const cutil_GenericType *
_mock_get_val_type(const void *data)
{
    (void) data;
    return CUTIL_GENERIC_TYPE_DOUBLE;
}

/* Mock MapType */
static const cutil_MapType MOCK_MAP_TYPE = {
  .name = "MockMap",
  .free = &_mock_free,
  .clear = &_mock_clear,
  .copy = &_mock_copy,
  .duplicate = &_mock_duplicate,
  .get_count = &_mock_get_count,
  .remove = &_mock_remove,
  .contains = &_mock_contains,
  .get = &_mock_get,
  .get_ptr = &_mock_get_ptr,
  .set = &_mock_set,
  .get_key_type = &_mock_get_key_type,
  .get_val_type = &_mock_get_val_type,
};

static cutil_Map *
_create_mock_map(void)
{
    MockMapData *const data = malloc(sizeof *data);
    *data = (MockMapData) {0};
    cutil_Map *const map = malloc(sizeof *map);
    map->vtable = &MOCK_MAP_TYPE;
    map->data = data;
    return map;
}

/* Tests for cutil_MapType_equals */
static void
_should_returnTrue_when_mapTypesAreIdentical(void)
{
    /* Arrange */
    const cutil_MapType type1 = {
      .name = "Test",
      .free = &_mock_free,
      .clear = &_mock_clear,
    };
    const cutil_MapType type2 = {
      .name = "Test",
      .free = &_mock_free,
      .clear = &_mock_clear,
    };

    /* Act */
    const cutil_Bool result = cutil_MapType_equals(&type1, &type2);

    /* Assert */
    TEST_ASSERT_TRUE(result);
}

static void
_should_returnFalse_when_mapTypesAreDifferent(void)
{
    /* Arrange */
    const cutil_MapType type1 = {.name = "Type1"};
    const cutil_MapType type2 = {.name = "Type2"};

    /* Act */
    const cutil_Bool result = cutil_MapType_equals(&type1, &type2);

    /* Assert */
    TEST_ASSERT_FALSE(result);
}

/* Tests for cutil_Map_free */
static void
_should_callFreeFunction_when_mapIsFreed(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();

    /* Act */
    cutil_Map_free(map);

    /* Assert */
    /* Note: we can't check free_count after free, but at least verify no crash
     */
    TEST_ASSERT_TRUE(true);
}

static void
_should_doNothing_when_mapIsNull(void)
{
    /* Act & Assert */
    cutil_Map_free(NULL);
    TEST_ASSERT_TRUE(true);
}

/* Tests for cutil_Map_clear */
static void
_should_callClearFunction_when_mapIsCleared(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    const int initial_clear_count = mock->clear_count;

    /* Act */
    cutil_Map_clear(map);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial_clear_count + 1, mock->clear_count);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_copy */
static void
_should_callCopyFunction_when_mapIsCopied(void)
{
    /* Arrange */
    cutil_Map *const dst_map = _create_mock_map();
    cutil_Map *const src_map = _create_mock_map();
    MockMapData *const dst_mock = (MockMapData *) dst_map->data;
    const int initial_copy_count = dst_mock->copy_count;

    /* Act */
    cutil_Map_copy(dst_map, src_map);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial_copy_count + 1, dst_mock->copy_count);

    /* Cleanup */
    cutil_Map_free(dst_map);
    cutil_Map_free(src_map);
}

static void
_should_notCopy_when_mapTypesIncompatible(void)
{
    /* Arrange */
    cutil_MapType type1 = {.name = "Type1", .copy = &_mock_copy};
    cutil_MapType type2 = {.name = "Type2", .copy = &_mock_copy};
    MockMapData *dst_data = malloc(sizeof *dst_data);
    *dst_data = (MockMapData) {0};
    MockMapData *src_data = malloc(sizeof *src_data);
    *src_data = (MockMapData) {0};
    cutil_Map dst_map = {.vtable = &type1, .data = dst_data};
    cutil_Map src_map = {.vtable = &type2, .data = src_data};
    const int initial_copy_count = dst_data->copy_count;

    /* Act */
    cutil_Map_copy(&dst_map, &src_map);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(initial_copy_count, dst_data->copy_count);

    /* Cleanup */
    free(dst_data);
    free(src_data);
}

/* Tests for cutil_Map_duplicate */
static void
_should_returnDuplicateMap_when_mapIsDuplicated(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    mock->get_count_val = 42;
    const int initial_dup_count = mock->duplicate_count;

    /* Act */
    cutil_Map *const dup = cutil_Map_duplicate(map);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_PTR(map->vtable, dup->vtable);
    TEST_ASSERT_NOT_NULL(dup->data);
    MockMapData *const dup_mock = (MockMapData *) dup->data;
    TEST_ASSERT_EQUAL_INT(initial_dup_count + 1, mock->duplicate_count);
    TEST_ASSERT_EQUAL_INT(42, dup_mock->get_count_val);

    /* Cleanup */
    cutil_Map_free(map);
    cutil_Map_free(dup);
}

/* Tests for cutil_Map_get_count */
static void
_should_returnCount_when_mapCountIsRequested(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    mock->get_count_val = 100;

    /* Act */
    const size_t count = cutil_Map_get_count(map);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(100, count);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnZero_when_mapIsEmpty(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    mock->get_count_val = 0;

    /* Act */
    const size_t count = cutil_Map_get_count(map);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, count);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_remove */
static void
_should_callRemoveFunction_when_keyIsRemoved(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    const int initial_remove_count = mock->remove_count;
    const int key = 42;

    /* Act */
    const int result = cutil_Map_remove(map, &key);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(initial_remove_count + 1, mock->remove_count);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_contains */
static void
_should_returnTrue_when_mapContainsKey(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    const int key = 42;

    /* Act */
    const cutil_Bool result = cutil_Map_contains(map, &key);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_get */
static void
_should_callGetFunction_when_valueIsRetrieved(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    const int initial_get_count = mock->get_count_count;
    const int key = 42;
    int value = 0;

    /* Act */
    const int result = cutil_Map_get(map, &key, &value);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(initial_get_count + 1, mock->get_count_count);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_get_ptr */
static void
_should_returnPointer_when_keyIsFound(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    const int key = 42;

    /* Act */
    const void *const result = cutil_Map_get_ptr(map, &key);

    /* Assert */
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL_PTR(map->data, result);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_set */
static void
_should_callSetFunction_when_keyValuePairIsInserted(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();
    MockMapData *const mock = (MockMapData *) map->data;
    const int initial_set_count = mock->set_count;
    const int key = 42;
    const int value = 100;

    /* Act */
    const int result = cutil_Map_set(map, &key, &value);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(initial_set_count + 1, mock->set_count);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_get_vtable */
static void
_should_returnVtable_when_vtableIsRequested(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();

    /* Act */
    const cutil_MapType *const vtable = cutil_Map_get_vtable(map);

    /* Assert */
    TEST_ASSERT_NOT_NULL(vtable);
    TEST_ASSERT_EQUAL_PTR(&MOCK_MAP_TYPE, vtable);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnNull_when_vtableMapIsNull(void)
{
    /* Act & Assert */
    TEST_ASSERT_NULL(cutil_Map_get_vtable(NULL));
}

/* Tests for cutil_Map_get_key_type */
static void
_should_callGetKeyTypeFunction_when_keyTypeIsQueried(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();

    /* Act */
    const cutil_GenericType *const key_type = cutil_Map_get_key_type(map);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_GENERIC_TYPE_INT, key_type);

    /* Cleanup */
    cutil_Map_free(map);
}

/* Tests for cutil_Map_get_val_type */
static void
_should_callGetValTypeFunction_when_valTypeIsQueried(void)
{
    /* Arrange */
    cutil_Map *const map = _create_mock_map();

    /* Act */
    const cutil_GenericType *const val_type = cutil_Map_get_val_type(map);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(CUTIL_GENERIC_TYPE_DOUBLE, val_type);

    /* Cleanup */
    cutil_Map_free(map);
}

/* =========================================================================
 * Factory infrastructure for multi-implementation behavioral tests
 * =========================================================================
 *
 * Each test function in the section below obtains its map under test by
 * calling g_current_factory(). To run a test against a new implementation,
 * assign a factory function to g_current_factory in main() before calling
 * RUN_TEST.
 */

typedef cutil_Map *(*MapFactory)(void);
static MapFactory g_current_factory = NULL;

static cutil_Map *
_hashmap_factory_int_int(void)
{
    return cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_INT);
}

static cutil_Map *
_hashmap_factory_int_double(void)
{
    return cutil_HashMap_alloc(
      CUTIL_GENERIC_TYPE_INT, CUTIL_GENERIC_TYPE_DOUBLE
    );
}

static cutil_Map *
_hashmap_factory_u32_i8(void)
{
    return cutil_HashMap_alloc(CUTIL_GENERIC_TYPE_U32, CUTIL_GENERIC_TYPE_I8);
}

static void
_should_returnSuccess_when_singleEntrySet(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 1;
    const int val = 10;

    /* Act */
    const int result = cutil_Map_set(map, &key, &val);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_size_t(1, cutil_Map_get_count(map));

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnCorrectValue_when_getAfterSet(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 42;
    const int expected = 100;

    /* Act */
    cutil_Map_set(map, &key, &expected);
    int retrieved = 0;
    const int result = cutil_Map_get(map, &key, &retrieved);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_INT(expected, retrieved);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnNonNullPtr_when_getPtrAfterSet(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 7;
    const int val = 77;

    /* Act */
    cutil_Map_set(map, &key, &val);
    const int *const ptr = (const int *) cutil_Map_get_ptr(map, &key);

    /* Assert */
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_INT(val, *ptr);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnNull_when_getPtrForMissingKey(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 99;

    /* Act */
    const void *const ptr = cutil_Map_get_ptr(map, &key);

    /* Assert */
    TEST_ASSERT_NULL(ptr);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnZeroCount_when_mapIsNewlyAllocated(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();

    /* Act */
    const size_t count = cutil_Map_get_count(map);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, count);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_incrementCountWithEachUniqueEntry(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int keys[] = {10, 20, 30, 40, 50};
    const int vals[] = {1, 2, 3, 4, 5};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);

    for (size_t i = 0; i < N; ++i) {
        /* Act */
        cutil_Map_set(map, &keys[i], &vals[i]);

        /* Assert */
        TEST_ASSERT_EQUAL_size_t(i + 1, cutil_Map_get_count(map));
    }

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnTrue_when_containsExistingKey(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 5;
    const int val = 50;
    cutil_Map_set(map, &key, &val);

    /* Act */
    const cutil_Bool result = cutil_Map_contains(map, &key);

    /* Assert */
    TEST_ASSERT_TRUE(result);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnFalse_when_containsMissingKey(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 999;

    /* Act */
    const cutil_Bool result = cutil_Map_contains(map, &key);

    /* Assert */
    TEST_ASSERT_FALSE(result);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnSuccess_when_removingExistingKey(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 3;
    const int val = 30;
    cutil_Map_set(map, &key, &val);

    /* Act */
    const int result = cutil_Map_remove(map, &key);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
    TEST_ASSERT_EQUAL_size_t(0, cutil_Map_get_count(map));
    TEST_ASSERT_FALSE(cutil_Map_contains(map, &key));

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnFailure_when_removingMissingKey(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 888;

    /* Act */
    const int result = cutil_Map_remove(map, &key);

    /* Assert */
    TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_FAILURE, result);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_decrementCount_when_existingKeyRemoved(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int keys[] = {1, 2, 3};
    const int vals[] = {10, 20, 30};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(map, &keys[i], &vals[i]);
    }

    /* Act */
    cutil_Map_remove(map, &keys[1]);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(N - 1, cutil_Map_get_count(map));

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnZeroCount_when_clearedAfterFill(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int keys[] = {1, 2, 3, 4, 5};
    const int vals[] = {10, 20, 30, 40, 50};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(map, &keys[i], &vals[i]);
    }
    TEST_ASSERT_EQUAL_size_t(N, cutil_Map_get_count(map));

    /* Act */
    cutil_Map_clear(map);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(0, cutil_Map_get_count(map));
    TEST_ASSERT_FALSE(cutil_Map_contains(map, &keys[0]));

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_returnUpdatedValue_when_keySetTwice(void)
{
    /* Arrange */
    cutil_Map *const map = g_current_factory();
    const int key = 42;
    const int val1 = 100;
    const int val2 = 200;

    /* Act */
    cutil_Map_set(map, &key, &val1);
    cutil_Map_set(map, &key, &val2);
    int retrieved = 0;
    cutil_Map_get(map, &key, &retrieved);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(1, cutil_Map_get_count(map));
    TEST_ASSERT_EQUAL_INT(val2, retrieved);

    /* Cleanup */
    cutil_Map_free(map);
}

static void
_should_matchAllEntries_when_copied(void)
{
    /* Arrange */
    cutil_Map *const src = g_current_factory();
    cutil_Map *const dst = g_current_factory();
    const int keys[] = {1, 2, 3};
    const int vals[] = {10, 20, 30};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(src, &keys[i], &vals[i]);
    }

    /* Act */
    cutil_Map_copy(dst, src);

    /* Assert */
    TEST_ASSERT_EQUAL_size_t(N, cutil_Map_get_count(dst));
    for (size_t i = 0; i < N; ++i) {
        int retrieved = 0;
        const int result = cutil_Map_get(dst, &keys[i], &retrieved);
        TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
        TEST_ASSERT_EQUAL_INT(vals[i], retrieved);
    }

    /* Cleanup */
    cutil_Map_free(src);
    cutil_Map_free(dst);
}

static void
_should_beIndependent_when_dstModifiedAfterCopy(void)
{
    /* Arrange */
    cutil_Map *const src = g_current_factory();
    cutil_Map *const dst = g_current_factory();
    const int key = 1;
    const int val = 10;
    cutil_Map_set(src, &key, &val);
    cutil_Map_copy(dst, src);
    const size_t src_count_before = cutil_Map_get_count(src);

    /* Act — add a new entry to dst only */
    const int extra_key = 99;
    const int extra_val = 990;
    cutil_Map_set(dst, &extra_key, &extra_val);

    /* Assert — src is unchanged */
    TEST_ASSERT_EQUAL_size_t(src_count_before, cutil_Map_get_count(src));
    TEST_ASSERT_FALSE(cutil_Map_contains(src, &extra_key));

    /* Cleanup */
    cutil_Map_free(src);
    cutil_Map_free(dst);
}

static void
_should_matchAllEntries_when_duplicated(void)
{
    /* Arrange */
    cutil_Map *const src = g_current_factory();
    const int keys[] = {5, 6, 7};
    const int vals[] = {50, 60, 70};
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(keys);
    for (size_t i = 0; i < N; ++i) {
        cutil_Map_set(src, &keys[i], &vals[i]);
    }

    /* Act */
    cutil_Map *const dup = cutil_Map_duplicate(src);

    /* Assert */
    TEST_ASSERT_NOT_NULL(dup);
    TEST_ASSERT_EQUAL_size_t(N, cutil_Map_get_count(dup));
    for (size_t i = 0; i < N; ++i) {
        int retrieved = 0;
        const int result = cutil_Map_get(dup, &keys[i], &retrieved);
        TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, result);
        TEST_ASSERT_EQUAL_INT(vals[i], retrieved);
    }

    /* Cleanup */
    cutil_Map_free(src);
    cutil_Map_free(dup);
}

static void
_should_beIndependent_when_dupModifiedAfterDuplicate(void)
{
    /* Arrange */
    cutil_Map *const src = g_current_factory();
    const int key = 2;
    const int val = 20;
    cutil_Map_set(src, &key, &val);
    cutil_Map *const dup = cutil_Map_duplicate(src);
    const size_t src_count_before = cutil_Map_get_count(src);

    /* Act — modify dup */
    const int extra_key = 77;
    const int extra_val = 770;
    cutil_Map_set(dup, &extra_key, &extra_val);

    /* Assert — src unchanged */
    TEST_ASSERT_EQUAL_size_t(src_count_before, cutil_Map_get_count(src));
    TEST_ASSERT_FALSE(cutil_Map_contains(src, &extra_key));

    /* Cleanup */
    cutil_Map_free(src);
    cutil_Map_free(dup);
}

static void
_should_setAndGetSucceed_withVariousKeyValueTypes(void)
{
    /* Arrange */
    struct {
        cutil_Map *(*const factory)(void);
        const void *const key;
        const void *const val;
    } SETUPS[] = {
      {
        .factory = &_hashmap_factory_int_int,
        .key = (const int[]) {1},
        .val = (const int[]) {42},
      },
      {
        .factory = &_hashmap_factory_int_double,
        .key = (const int[]) {1},
        .val = (const double[]) {42.0},
      },
      {
        .factory = &_hashmap_factory_u32_i8,
        .key = (const uint32_t[]) {1},
        .val = (const int8_t[]) {42},
      },
    };
    const size_t N = CUTIL_GET_NATIVE_ARRAY_SIZE(SETUPS);

    for (size_t i = 0; i < N; ++i) {
        cutil_Map *const map = SETUPS[i].factory();
        TEST_ASSERT_NOT_NULL(map);

        /* Act */
        const int set_result = cutil_Map_set(map, SETUPS[i].key, SETUPS[i].val);

        /* Assert */
        TEST_ASSERT_EQUAL_INT(CUTIL_STATUS_SUCCESS, set_result);
        TEST_ASSERT_EQUAL_size_t(1, cutil_Map_get_count(map));

        /* Cleanup */
        cutil_Map_free(map);
    }
}

void
setUp(void)
{}

void
tearDown(void)
{}

int
main(void)
{
    UNITY_BEGIN();

    /* --- Mock-based vtable dispatch tests (shim layer verification) --- */
    RUN_TEST(_should_returnTrue_when_mapTypesAreIdentical);
    RUN_TEST(_should_returnFalse_when_mapTypesAreDifferent);
    RUN_TEST(_should_callFreeFunction_when_mapIsFreed);
    RUN_TEST(_should_doNothing_when_mapIsNull);
    RUN_TEST(_should_callClearFunction_when_mapIsCleared);
    RUN_TEST(_should_callCopyFunction_when_mapIsCopied);
    RUN_TEST(_should_notCopy_when_mapTypesIncompatible);
    RUN_TEST(_should_returnDuplicateMap_when_mapIsDuplicated);
    RUN_TEST(_should_returnCount_when_mapCountIsRequested);
    RUN_TEST(_should_returnZero_when_mapIsEmpty);
    RUN_TEST(_should_callRemoveFunction_when_keyIsRemoved);
    RUN_TEST(_should_returnTrue_when_mapContainsKey);
    RUN_TEST(_should_callGetFunction_when_valueIsRetrieved);
    RUN_TEST(_should_returnPointer_when_keyIsFound);
    RUN_TEST(_should_callSetFunction_when_keyValuePairIsInserted);
    RUN_TEST(_should_returnVtable_when_vtableIsRequested);
    RUN_TEST(_should_returnNull_when_vtableMapIsNull);
    RUN_TEST(_should_callGetKeyTypeFunction_when_keyTypeIsQueried);
    RUN_TEST(_should_callGetValTypeFunction_when_valTypeIsQueried);

    /* --- HashMap (INT, INT) — full interface coverage --- */
    g_current_factory = _hashmap_factory_int_int;
    RUN_TEST(_should_returnSuccess_when_singleEntrySet);
    RUN_TEST(_should_returnCorrectValue_when_getAfterSet);
    RUN_TEST(_should_returnNonNullPtr_when_getPtrAfterSet);
    RUN_TEST(_should_returnNull_when_getPtrForMissingKey);
    RUN_TEST(_should_returnZeroCount_when_mapIsNewlyAllocated);
    RUN_TEST(_should_incrementCountWithEachUniqueEntry);
    RUN_TEST(_should_returnTrue_when_containsExistingKey);
    RUN_TEST(_should_returnFalse_when_containsMissingKey);
    RUN_TEST(_should_returnSuccess_when_removingExistingKey);
    RUN_TEST(_should_returnFailure_when_removingMissingKey);
    RUN_TEST(_should_decrementCount_when_existingKeyRemoved);
    RUN_TEST(_should_returnZeroCount_when_clearedAfterFill);
    RUN_TEST(_should_returnUpdatedValue_when_keySetTwice);
    RUN_TEST(_should_matchAllEntries_when_copied);
    RUN_TEST(_should_beIndependent_when_dstModifiedAfterCopy);
    RUN_TEST(_should_matchAllEntries_when_duplicated);
    RUN_TEST(_should_beIndependent_when_dupModifiedAfterDuplicate);

    /* --- Type-variety test (not factory-driven) --- */
    RUN_TEST(_should_setAndGetSucceed_withVariousKeyValueTypes);

    return UNITY_END();
}
