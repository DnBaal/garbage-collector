#include "../munit/munit.h"


extern MunitSuite snekobject_suite;

int main(int argc, char *argv[]) {
    const MunitSuite all_suites[] = {snekobject_suite};

    return munit_suite_main(all_suites, NULL, argc, argv);

}
