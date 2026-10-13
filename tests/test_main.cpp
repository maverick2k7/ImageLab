// test_main.cpp — Entry point for the ImageLab test program.
// Calls one function per test group (defined in tests/test_*.cpp files).
// For now the groups are declared but not yet implemented; they will be
// added module by module as the project grows.
//
// Build (from project root):
//   g++ -std=c++11 -O2 -Wall -Wextra -isystem src/third_party \
//       tests/test_main.cpp tests/test_dynArray.cpp tests/test_stack.cpp \
//       tests/test_queue.cpp tests/test_image_ops.cpp              \
//       tests/test_history.cpp tests/test_pipeline.cpp tests/test_fill.cpp \
//       src/image_io.cpp src/Image.cpp src/ImageOps.cpp            \
//       src/BucketFill.cpp src/HistoryManager.cpp src/Pipeline.cpp  \
//       src/ImageLab.cpp src/Inspector.cpp src/Menu.cpp             \
//       -o run_tests

#include "test_framework.h"

// ---------------------------------------------------------------------------
// Forward declarations — each test_*.cpp defines one of these functions.
// They call check() / CHECK() internally and do NOT return a value.
// ---------------------------------------------------------------------------
void testDynArray();      // tests/test_dynArray.cpp
void testStack();         // tests/test_stack.cpp
void testQueue();         // tests/test_queue.cpp
void testImageOps();      // tests/test_image_ops.cpp
void testHistory();       // tests/test_history.cpp
void testPipeline();      // tests/test_pipeline.cpp
void testFill();          // tests/test_fill.cpp

// ---------------------------------------------------------------------------
// main — run all groups, then print summary
// ---------------------------------------------------------------------------
int main() {
    std::cout << "=== ImageLab test suite ===\n\n";

    // --- Phase 1 test groups (uncomment as each module is implemented) ---
    testDynArray();
    testStack();
    testQueue();
    testImageOps();
    testHistory();
    testPipeline();
    testFill();

    // summary() prints totals and returns non-zero if any test failed.
    return summary();
}
