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

// Test String ----------------------------------

static MunitResult test_str_copied(const MunitParameter params[],
                                   void *user_data) {
  char *input = "Hello World!";
  snek_object_t *obj = new_snek_string(input);

  munit_assert_int(obj->kind, ==, STRING);

  munit_assert_ptr_not_equal(obj->data.v_string, input);
  munit_assert_string_equal(obj->data.v_string, input);
  // munit_assert_int_equal(boot_alloc_size(), 22);     TODO: FIX THIS

  free(obj->data.v_string);
  free(obj);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

// Test Vector3 -------------------------------

static MunitResult test_returns_null(const MunitParameter params[],
                                     void *user_data) {
    snek_object_t *vec = new_snek_vector3(NULL, NULL, NULL);
    munit_assert_null(vec);
    free(vec);
    munit_assert_true(boot_all_freed());
    return MUNIT_OK;
}

static MunitResult test_vec_multiple_objects(const MunitParameter params[],
                                       void *user_data) {
  snek_object_t *x = new_snek_integer(1);
  snek_object_t *y = new_snek_integer(2);
  snek_object_t *z = new_snek_integer(3);
  snek_object_t *vec = new_snek_vector3(x, y, z);

  munit_assert_ptr_not_null(vec);

  // Vectors should not copy objects, they get reference to the objects
  munit_assert_ptr(x, ==, vec->data.v_vector.x);
  munit_assert_ptr(y, ==, vec->data.v_vector.y);
  munit_assert_ptr(z, ==, vec->data.v_vector.z);

  munit_assert_int(vec->data.v_vector.x->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector.y->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector.z->data.v_int, ==, 3);

  free(x);
  free(y);
  free(z);
  free(vec);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_vec_same_object(const MunitParameter params[],
                                        void *user_data) {
  snek_object_t *i = new_snek_integer(1);
  snek_object_t *vec = new_snek_vector3(i, i, i);

  munit_assert_ptr(i, ==, vec->data.v_vector.x);
  munit_assert_ptr(i, ==, vec->data.v_vector.y);
  munit_assert_ptr(i, ==, vec->data.v_vector.z);

  munit_assert_int(vec->data.v_vector.x->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector.y->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector.z->data.v_int, ==, 1);

  i->data.v_int = 2;
  // Assert we have integer values correct, after update
  munit_assert_int(vec->data.v_vector.y->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector.z->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector.x->data.v_int, ==, 2);

  free(i);
  free(vec);
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
    {"/string/copied", test_str_copied, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/vector3/returns_null", test_returns_null, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/vector3/multiple_objects", test_vec_multiple_objects, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/vector3/same_object", test_vec_same_object, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite snekobject_suite = {"/snekobject", snekobject_tests, NULL, 1,
                               MUNIT_SUITE_OPTION_NONE};
