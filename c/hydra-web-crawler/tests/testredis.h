#include "cu/cu.h"
#include "../parse.h"

// Declarations of tests
TEST(testredisTTL);
//TEST(test2);


// Collect tests into test suite
TEST_SUITE(REDIS_T1) {
    TEST_ADD(testredisttl),
    TEST_SUITE_CLOSURE
};
