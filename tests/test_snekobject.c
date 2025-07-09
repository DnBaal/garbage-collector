#include "../munit/munit.h"
#include "../src/bootmem.h"
#include "../src/snekobject.h"
#include "stdlib.h"

// Test Integer --------------------------------

static MunitResult test_integer_positive(const MunitParameter params[],
                                         void *user_data) {
  snek_object_t *obj = new_snek_integer(42);
  munit_assert_int(obj->data.v_int, ==, 42);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_integer_zero(const MunitParameter params[],
                                     void *user_data) {
  snek_object_t *obj = new_snek_integer(0);
  munit_assert_int(obj->kind, ==, INTEGER);
  munit_assert_int(obj->data.v_int, ==, 0);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_integer_negative(const MunitParameter params[],
                                         void *user_data) {
  snek_object_t *obj = new_snek_integer(-5);

  munit_assert_int(obj->kind, ==, INTEGER);
  munit_assert_int(obj->data.v_int, ==, -5);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

// Test Float -----------------------------------

static MunitResult test_float_positive(const MunitParameter params[],
                                       void *user_data) {
  snek_object_t *obj = new_snek_float(42.0);

  munit_assert_float(obj->data.v_float, ==, 42.0);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_float_zero(const MunitParameter params[],
                                   void *user_data) {
  snek_object_t *obj = new_snek_float(0.0);

  munit_assert_float(obj->kind, ==, FLOAT);
  munit_assert_float(obj->data.v_float, ==, 0.0);
  free(obj);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_float_negative(const MunitParameter params[],
                                       void *user_data) {
  snek_object_t *obj = new_snek_float(-5.0);

  munit_assert_float(obj->data.v_float, ==, -5.0);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

// Test Registration ----------------------------

static MunitTest snekobject_tests[] = {
    {"/integer/positive", test_integer_positive, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/integer/zero", test_integer_zero, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/integer/negative", test_integer_negative, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/float/positive", test_float_positive, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/float/zero", test_float_zero, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/float/negative", test_float_negative, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite snekobject_suite = {"/snekobject", snekobject_tests, NULL, 1,
                               MUNIT_SUITE_OPTION_NONE};
