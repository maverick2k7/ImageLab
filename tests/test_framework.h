// test_framework.h — Tiny test framework for ImageLab
// No std containers. No recursion. No forbidden keywords.
// Allowed headers: iostream, string.
//
// Usage:
//   check(some_bool_expr, "my test name");
//   return summary();   // in main(); returns non-zero if any FAIL

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Global counters — plain ints, no containers
// ---------------------------------------------------------------------------
static int _tf_pass = 0;
static int _tf_fail = 0;

// ---------------------------------------------------------------------------
// check() — evaluate one test condition and record the result
// ---------------------------------------------------------------------------
inline void check(bool cond, const std::string& name) {
    if (cond) {
        std::cout << "PASS: " << name << "\n";
        _tf_pass++;
    } else {
        std::cout << "FAIL: " << name << "\n";
        _tf_fail++;
    }
}

// ---------------------------------------------------------------------------
// summary() — print totals and return non-zero if any test failed
// Call once at the end of the test program (in main()).
// ---------------------------------------------------------------------------
inline int summary() {
    int total = _tf_pass + _tf_fail;
    std::cout << "\n=== Test summary ===\n";
    std::cout << "  Passed: " << _tf_pass << " / " << total << "\n";
    std::cout << "  Failed: " << _tf_fail << " / " << total << "\n";

    if (_tf_fail == 0) {
        std::cout << "  ALL TESTS PASSED\n";
    } else {
        std::cout << "  SOME TESTS FAILED\n";
    }

    // Non-zero return signals failure to the shell / CI system
    return (_tf_fail > 0) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Optional convenience macro (avoids repeating the expression as a string)
// The plan permits macros in the test framework specifically.
// ---------------------------------------------------------------------------
#define CHECK(expr) check((expr), #expr)

#endif // TEST_FRAMEWORK_H
