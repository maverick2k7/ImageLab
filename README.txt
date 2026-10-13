ImageLab — DSA Assignment 2
============================

[PLACEHOLDER — fill in after all modules are complete]

--- BUILD ---

Requirements: g++ with C++11 support (MinGW/MSYS2 on Windows, or GCC on Linux).
No external libraries beyond stb_image.h / stb_image_write.h (included in src/third_party/).

From the project root:

  Windows (cmd):
    src\build.bat

  Linux / MSYS2 bash:
    bash src/build.sh

  Or use make (from src/):
    cd src && make

Explicit g++ build lines (run from the project root):

  Main program:
    g++ -std=c++11 -O2 -Wall -Wextra -isystem src/third_party \
        src/main.cpp src/image_io.cpp src/Image.cpp src/ImageOps.cpp \
        src/BucketFill.cpp src/HistoryManager.cpp src/Pipeline.cpp \
        src/ImageLab.cpp src/Inspector.cpp src/Menu.cpp \
        -o imagelab

  Test program:
    g++ -std=c++11 -O2 -Wall -Wextra -isystem src/third_party \
        tests/test_main.cpp tests/test_dynArray.cpp tests/test_stack.cpp \
        tests/test_queue.cpp tests/test_image_ops.cpp \
        tests/test_history.cpp tests/test_pipeline.cpp tests/test_fill.cpp \
        src/image_io.cpp src/Image.cpp src/ImageOps.cpp src/BucketFill.cpp \
        src/HistoryManager.cpp src/Pipeline.cpp src/ImageLab.cpp \
        -o run_tests

  Test-image generator:
    g++ -std=c++11 -O2 -Wall -Wextra -isystem src/third_party \
        tests/make_test_images.cpp src/image_io.cpp src/Image.cpp \
        -o make_images

--- RUN ---

Run all programs from the project root so that images/ paths work:

  imagelab          (or imagelab.exe on Windows)
  run_tests         (or run_tests.exe)
  make_images       (or make_images.exe)

Outputs (saved images, CSVs) are written to the current directory with
prefix out_ or results_. Do not create directories from code.

--- LIBRARIES ---

stb_image.h and stb_image_write.h (public domain, Sean T. Barrett)
Used ONLY in src/image_io.cpp for file I/O (loading/saving images).
Source: https://github.com/nothings/stb

--- ASSUMPTIONS (Decisions log) ---

D1  Stack = growable array + base index; isFullStack() false while growable.
D2  Queue = circular array, count variable, fixed by default, growable on demand.
D3  Empty/full cases: print message, no-op or return Type(); never crash.
D4  Full pipeline queue: reject new job with message; cancelled jobs hold slot.
D5  Instant ops run immediately even if jobs are pending.
D6  Queued job parameters validated at execution; failures not recorded.
D7  Fill: recolour on insert, 4-connectivity, neighbour order U,D,L,R.
D8  History records stored by pointer, owned by HistoryManager.
D9  Oversized record -> clear history, warn.
D10 Cleanup oldest-first from Undo bottom, then Redo bottom (removeBottom()).
D11 Brightness coalescing (any signs) with exact clip-list merge.
D12 Priority queue = ordered linked list, ties by arrival, barrier via epochs.
D13 Scheduling mode switch only when idle.
D14 Waiting time measured (steady_clock) + deterministic simulation.
D15 Batch ops equivalent to n singles; clamp n; fixed-capacity adds only what fits.
D16 Save -> imagelab_project.png; Export extension chooses format.
D17 stb used only in image_io.cpp for file I/O.
D18 Grayscale: integer BT.601 weights; box blur radius r; bilinear resize.
D19 Undo/Redo on empty stack = reported no-op (also as a job).
D20 Experiment results -> results_*.csv in the working directory.

--- MENU GUIDE ---

 1  Load image
 -- Instant edits --
 2  Crop      3  Rotate      4  Flip      5  Brightness      6  Grayscale
 -- Queued jobs (go to pipeline) --
 7  Resize    8  Blur        9  Bucket fill   10 Save project   11 Export
 -- Pipeline --
12  List pending jobs        13 Process next job      14 Process all jobs
15  Cancel a job (by id)     16 Clear all jobs        17 Set queue capacity
18  Scheduling mode (FIFO/Priority) [P2]
 -- History --
19  Undo      20  Redo
 -- Tools --
21  Data Structure Inspector 22  Show image info
23  Run experiments [P2]     24  Set history memory budget [P2]
25  Toggle coalescing [P2]
 0  Exit
