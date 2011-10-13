#include "cu/cu.h"
#include "../parse.h"

// Declarations of tests
TEST(testweb1);
//TEST(test2);


// Collect tests into test suite
TEST_SUITE(WEB_T1) {
    TEST_ADD(testweb1),
    TEST_SUITE_CLOSURE
};
