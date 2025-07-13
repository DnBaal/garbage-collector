#include "../munit/munit.h"

// extern MunitSuite snekobject_suite;
extern MunitSuite stack_suite;
extern MunitSuite vm_suite;

int main(int argc, char *argv[])
{
    int result = 0;
    // result |= munit_suite_main(&snekobject_suite, NULL, argc, argv);
    result |= munit_suite_main(&stack_suite, NULL, argc, argv);
    result |= munit_suite_main(&vm_suite, NULL, argc, argv);
    return result;
}
