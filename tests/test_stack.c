#include "../munit/munit.h"
#include "../src/bootmem.h"
#include "../src/stack.h"
#include "stdlib.h"

static MunitResult test_create_stack_small(const MunitParameter params[],
                                           void *user_data)
{
    stack_t *stack = stack_new(2);
    munit_assert_int(stack->capacity, ==, 2);
    munit_assert_int(stack->count, ==, 0);
    munit_assert_ptr_not_null(stack->data);

    stack_free(stack);
    munit_assert_true(boot_all_freed());
    return MUNIT_OK;
}

static MunitResult test_create_stack_large(const MunitParameter params[],
                                           void *user_data)
{
    stack_t *stack = stack_new(100);
    munit_assert_int(stack->capacity, ==, 100);
    munit_assert_int(stack->count, ==, 0);
    munit_assert_ptr_not_null(stack->data);

    stack_free(stack);
    munit_assert_true(boot_all_freed());
    return MUNIT_OK;
}

static MunitResult test_push_stack(const MunitParameter params[],
                                   void *user_data)
{
    stack_t *stack = stack_new(2);
    munit_assert_ptr_not_null(stack);

    munit_assert_int(stack->count, ==, 0);
    munit_assert_int(stack->capacity, ==, 2);
    munit_assert_ptr_not_null(stack->data);

    int a = 1;

    stack_push(stack, &a);
    stack_push(stack, &a);

    munit_assert_int(stack->count, ==, 2);
    munit_assert_int(stack->capacity, ==, 2);
    munit_assert_ptr_equal(stack->data[0], &a);

    stack_free(stack);
    munit_assert_true(boot_all_freed());

    return MUNIT_OK;
}

static MunitResult test_push_double_capacity(const MunitParameter params[],
                                             void *user_data)
{
    stack_t *stack = stack_new(2);
    munit_assert_ptr_not_null(stack);

    munit_assert_int(stack->count, ==, 0);
    munit_assert_int(stack->capacity, ==, 2);
    munit_assert_ptr_not_null(stack->data);

    int a = 1;

    stack_push(stack, &a);
    stack_push(stack, &a);

    munit_assert_int(stack->count, ==, 2);
    munit_assert_int(stack->capacity, ==, 2);

    stack_push(stack, &a);
    munit_assert_int(stack->count, ==, 3);
    munit_assert_int(stack->capacity, ==, 4);

    // TODO: Implement boot_realloc_count()
    // for this munit_assert_int(boot_realloc_count(), ==, 1);

    stack_free(stack);
    munit_assert_true(boot_all_freed());
    return MUNIT_OK;
}

static MunitResult test_pop_stack(const MunitParameter params[],
                                  void *user_data)
{
    stack_t *stack = stack_new(2);
    munit_assert_ptr_not_null(stack);

    munit_assert_int(stack->count, ==, 0);
    munit_assert_int(stack->capacity, ==, 2);
    munit_assert_ptr_not_null(stack->data);

    int one = 1;
    int two = 2;
    int three = 3;

    stack_push(stack, &one);
    stack_push(stack, &two);

    munit_assert_int(stack->count, ==, 2);
    munit_assert_int(stack->capacity, ==, 2);

    stack_push(stack, &three);
    munit_assert_int(stack->count, ==, 3);
    munit_assert_int(stack->capacity, ==, 4);

    int *popped = stack_pop(stack);
    munit_assert_int(*popped, ==, three);

    popped = stack_pop(stack);
    munit_assert_int(*popped, ==, two);

    popped = stack_pop(stack);
    munit_assert_int(*popped, ==, one);

    popped = stack_pop(stack);
    munit_assert_null(popped);

    stack_free(stack);
    munit_assert_true(boot_all_freed());
    return MUNIT_OK;
}

static MunitResult test_pop_empty_stack(const MunitParameter params[],
                                        void *user_data)
{
    stack_t *stack = stack_new(2);
    munit_assert_ptr_not_null(stack);

    munit_assert_int(stack->count, ==, 0);
    munit_assert_int(stack->capacity, ==, 2);
    munit_assert_ptr_not_null(stack->data);

    int *popped = stack_pop(stack);
    munit_assert_null(popped);

    stack_free(stack);
    munit_assert_true(boot_all_freed());
    return MUNIT_OK;
}

static MunitTest stack_tests[] = {
    {"/create_stack_small", test_create_stack_small, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/create_stack_large", test_create_stack_large, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/push_stack", test_push_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/push_double_capacity", test_push_double_capacity, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/pop_stack", test_pop_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/pop_empty_stack", test_pop_empty_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite stack_suite = {
    "/stack",
    stack_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE};