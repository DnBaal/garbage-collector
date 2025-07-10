#include "../munit/munit.h"
#include "../src/bootmem.h"
#include "../src/snekobject.h"
#include "stdlib.h"

// Test Integer --------------------------------

static MunitResult test_integer_positive(const MunitParameter params[],
                                         void *user_data)
{
  snek_object_t *obj = new_snek_integer(42);
  munit_assert_int(obj->data.v_int, ==, 42);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_integer_zero(const MunitParameter params[],
                                     void *user_data)
{
  snek_object_t *obj = new_snek_integer(0);
  munit_assert_int(obj->kind, ==, INTEGER);
  munit_assert_int(obj->data.v_int, ==, 0);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_integer_negative(const MunitParameter params[],
                                         void *user_data)
{
  snek_object_t *obj = new_snek_integer(-5);

  munit_assert_int(obj->kind, ==, INTEGER);
  munit_assert_int(obj->data.v_int, ==, -5);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

// Test Float -----------------------------------

static MunitResult test_float_positive(const MunitParameter params[],
                                       void *user_data)
{
  snek_object_t *obj = new_snek_float(42.0);

  munit_assert_float(obj->data.v_float, ==, 42.0);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_float_zero(const MunitParameter params[],
                                   void *user_data)
{
  snek_object_t *obj = new_snek_float(0.0);

  munit_assert_float(obj->kind, ==, FLOAT);
  munit_assert_float(obj->data.v_float, ==, 0.0);
  free(obj);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_float_negative(const MunitParameter params[],
                                       void *user_data)
{
  snek_object_t *obj = new_snek_float(-5.0);

  munit_assert_float(obj->data.v_float, ==, -5.0);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

// Test String ----------------------------------

static MunitResult test_str_copied(const MunitParameter params[],
                                   void *user_data)
{
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
                                     void *user_data)
{
  snek_object_t *vec = new_snek_vector3(NULL, NULL, NULL);
  munit_assert_null(vec);
  free(vec);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_vec_multiple_objects(const MunitParameter params[],
                                             void *user_data)
{
  snek_object_t *x = new_snek_integer(1);
  snek_object_t *y = new_snek_integer(2);
  snek_object_t *z = new_snek_integer(3);
  snek_object_t *vec = new_snek_vector3(x, y, z);

  munit_assert_ptr_not_null(vec);

  // Vectors should not copy objects, they get reference to the objects
  munit_assert_ptr(x, ==, vec->data.v_vector3.x);
  munit_assert_ptr(y, ==, vec->data.v_vector3.y);
  munit_assert_ptr(z, ==, vec->data.v_vector3.z);

  munit_assert_int(vec->data.v_vector3.x->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector3.y->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector3.z->data.v_int, ==, 3);

  free(x);
  free(y);
  free(z);
  free(vec);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

static MunitResult test_vec_same_object(const MunitParameter params[],
                                        void *user_data)
{
  snek_object_t *i = new_snek_integer(1);
  snek_object_t *vec = new_snek_vector3(i, i, i);

  munit_assert_ptr(i, ==, vec->data.v_vector3.x);
  munit_assert_ptr(i, ==, vec->data.v_vector3.y);
  munit_assert_ptr(i, ==, vec->data.v_vector3.z);

  munit_assert_int(vec->data.v_vector3.x->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector3.y->data.v_int, ==, 1);
  munit_assert_int(vec->data.v_vector3.z->data.v_int, ==, 1);

  i->data.v_int = 2;
  // Assert we have integer values correct, after update
  munit_assert_int(vec->data.v_vector3.y->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector3.z->data.v_int, ==, 2);
  munit_assert_int(vec->data.v_vector3.x->data.v_int, ==, 2);

  free(i);
  free(vec);
  munit_assert_true(boot_all_freed());

  return MUNIT_OK;
}

// Test Array -----------------------------------

static MunitResult test_create_empty_array(const MunitParameter params[],
                                           void *user_data)
{
  snek_object_t *obj = new_snek_array(3);
  munit_assert_int(obj->kind, ==, ARRAY);
  munit_assert_int(obj->data.v_array.size, ==, 3);

  free(obj->data.v_array.elements);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_used_calloc(const MunitParameter params[],
                                    void *user_data)
{
  snek_object_t *obj = new_snek_array(2);

  munit_assert_ptr_null(obj->data.v_array.elements[0]);
  munit_assert_ptr_null(obj->data.v_array.elements[1]);

  free(obj->data.v_array.elements);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_set_outside_bounds(const MunitParameter params[],
                                           void *user_data)
{
  snek_object_t *obj = new_snek_array(2);

  snek_object_t *outside = new_snek_string("First");

  munit_assert_true(snek_array_set(obj, 1, outside));

  munit_assert_false(snek_array_set(obj, 100, outside));

  free(outside->data.v_string);
  free(outside);
  free(obj->data.v_array.elements);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_get_outside_bounds(const MunitParameter params[],
                                           void *user_data)
{
  snek_object_t *obj = new_snek_array(1);
  snek_object_t *first = new_snek_string("First");
  munit_assert_true(snek_array_set(obj, 0, first));

  munit_assert_null(snek_array_get(obj, 1));

  free(first->data.v_string);
  free(first);
  free(obj->data.v_array.elements);
  free(obj);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

// Test SnekAdd --------------------------------

static MunitResult test_integer_add(const MunitParameter params[],
                                    void *user_data)
{
  snek_object_t *one = new_snek_integer(1);
  snek_object_t *three = new_snek_integer(3);
  snek_object_t *four = snek_add(one, three);

  munit_assert_not_null(four);
  munit_assert_int(four->kind, ==, INTEGER);
  munit_assert_int(four->data.v_int, ==, 4);

  free(one);
  free(three);
  free(four);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_float_add(const MunitParameter params[],
                                  void *user_data)
{
  snek_object_t *one = new_snek_float(1.5);
  snek_object_t *three = new_snek_float(3.5);
  snek_object_t *five = snek_add(one, three);

  munit_assert_not_null(five);
  munit_assert_int(five->kind, ==, FLOAT);
  munit_assert_float(five->data.v_float, ==, 5.0);

  free(one);
  free(three);
  free(five);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_string_add(const MunitParameter params[],
                                   void *user_data)
{
  snek_object_t *hello = new_snek_string("Hello ");
  snek_object_t *world = new_snek_string("World!");
  snek_object_t *greeting = snek_add(hello, world);

  munit_assert_not_null(greeting);
  munit_assert_int(greeting->kind, ==, STRING);
  munit_assert_string_equal(greeting->data.v_string, "Hello World!");

  free(hello->data.v_string);
  free(hello);
  free(world->data.v_string);
  free(world);
  free(greeting->data.v_string);
  free(greeting);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_string_add_self(const MunitParameter params[],
                                        void *user_data)
{
  snek_object_t *repeated = new_snek_string("(repeated)");
  snek_object_t *result = snek_add(repeated, repeated);

  munit_assert_not_null(result);
  munit_assert_int(result->kind, ==, STRING);
  munit_assert_string_equal(result->data.v_string, "(repeated)(repeated)");

  free(repeated->data.v_string);
  free(repeated);
  free(result->data.v_string);
  free(result);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

static MunitResult test_vector3_add(const MunitParameter params[],
                                    void *user_data)
{
  snek_object_t *one = new_snek_float(1.0);
  snek_object_t *two = new_snek_float(2.0);
  snek_object_t *three = new_snek_float(3.0);
  snek_object_t *four = new_snek_float(4.0);
  snek_object_t *five = new_snek_float(5.0);
  snek_object_t *six = new_snek_float(6.0);

  snek_object_t *vec1 = new_snek_vector3(one, two, three);
  snek_object_t *vec2 = new_snek_vector3(four, five, six);
  snek_object_t *result = snek_add(vec1, vec2);

  munit_assert_not_null(result);
  munit_assert_int(result->kind, ==, VECTOR3);
  munit_assert_float(result->data.v_vector3.x->data.v_float, ==, 5.0);
  munit_assert_float(result->data.v_vector3.y->data.v_float, ==, 7.0);
  munit_assert_float(result->data.v_vector3.z->data.v_float, ==, 9.0);

  free(vec1->data.v_vector3.x);
  free(vec1->data.v_vector3.y);
  free(vec1->data.v_vector3.z);
  free(vec1);

  free(vec2->data.v_vector3.x);
  free(vec2->data.v_vector3.y);
  free(vec2->data.v_vector3.z);
  free(vec2);

  free(result->data.v_vector3.x);
  free(result->data.v_vector3.y);
  free(result->data.v_vector3.z);
  free(result);
  munit_assert_true(boot_all_freed());
  return MUNIT_OK;
}

MunitResult test_array_add(const MunitParameter params[],
                           void *user_data)
{
  snek_object_t *one = new_snek_integer(1);
  snek_object_t *ones = new_snek_array(2);
  munit_assert_true(snek_array_set(ones, 0, one));
  munit_assert_true(snek_array_set(ones, 1, one));

  snek_object_t *hi = new_snek_string("hi");
  snek_object_t *hellos = new_snek_array(3);
  munit_assert_true(snek_array_set(hellos, 0, hi));
  munit_assert_true(snek_array_set(hellos, 1, hi));
  munit_assert_true(snek_array_set(hellos, 2, hi));

  snek_object_t *result = snek_add(ones, hellos);

  munit_assert_not_null(result);
  munit_assert_int(result->kind, ==, ARRAY);

  snek_object_t *first = snek_array_get(result, 0);
  munit_assert_not_null(first);
  munit_assert_int(first->data.v_int, ==, 1);

  snek_object_t *third = snek_array_get(result, 2);
  munit_assert_not_null(third);
  munit_assert_string_equal(third->data.v_string, "hi");

  free(one);
  free(ones->data.v_array.elements);
  free(ones);

  free(hi->data.v_string);
  free(hi);
  free(hellos->data.v_array.elements);
  free(hellos);
  free(result->data.v_array.elements);
  free(result);

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
    {"/array/create_empty_array", test_create_empty_array, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/array/used_calloc", test_used_calloc, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/array/set_outside_bounds", test_set_outside_bounds, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/array/get_outside_bounds", test_get_outside_bounds, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/add/integer", test_integer_add, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/add/float", test_float_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/add/string", test_string_add, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/add/string_self", test_string_add_self, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/add/vector3", test_vector3_add, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/add/array", test_array_add, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL}};

MunitSuite snekobject_suite = {"/snekobject", snekobject_tests, NULL, 1,
                               MUNIT_SUITE_OPTION_NONE};
