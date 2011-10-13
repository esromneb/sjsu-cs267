// Include definition of T1 test suite
#include "testparse1.h"
#include "testweb1.h"


// This struct contains all test suites
TEST_SUITES {
    TEST_SUITE_ADD(PARSE_T1), // add parse.h/c test suite
    TEST_SUITE_ADD(WEB_T1), // add web.h/c test suite
    TEST_SUITES_CLOSURE
};

int main(int argc, char *argv[])
{
    // Set up directory where are stored files with outputs from test
    // suites
    CU_SET_OUT_PREFIX("regressions/");

    // Run all test suites
    CU_RUN(argc, argv);

    return 0;
}
