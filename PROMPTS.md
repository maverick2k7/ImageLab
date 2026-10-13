# ImageLab — Prompt Pack for Antigravity

**How to use**
1. Create an empty project folder, put `IMPLEMENTATION_PLAN.md` and this file in it, open it in Antigravity.
2. Paste prompts **one at a time, in order**. Do not start the next prompt until the current one's acceptance checks pass (compile with zero warnings + tests PASS).
3. After each prompt: run the build yourself, `git commit`. If the agent breaks something, use **Prompt X-3**.
4. Replace `<ROLL>` / `<SEC>` yourself at packaging time.
5. Read the code the agent writes. You must explain it in the demo (see Prompt X-2 for a study guide).

Every prompt starts with the same preamble (already included). Prompts are inside fenced blocks, copy the whole block.

---

## PHASE 1

### Prompt 0 — Scaffold, rules, build system

````
You are building "ImageLab", a C++11 console image-processing program for a university Data Structures assignment. The full specification is in IMPLEMENTATION_PLAN.md in this folder. READ IT COMPLETELY FIRST, especially Section 0 (agent rules), Section 2 (forbidden features: ANY standard container, recursion, threads, library image operations, extra headers), Section 3 (conventions) and Section 4 (folder structure).

TASK: create the project skeleton only (no data structures yet).
1. Run `git init`. Create this layout in the CURRENT folder (we will rename/copy it for submission later): src/, src/third_party/, tests/, images/, report/, README.txt (placeholder).
2. Download stb_image.h and stb_image_write.h into src/third_party/ from https://raw.githubusercontent.com/nothings/stb/master/stb_image.h and .../stb_image_write.h. If you have no internet access, tell me and stop on this item.
3. Create src/utils.h exactly as in plan 6.1 (minVal, maxVal, clampInt, swapVals, Timer on steady_clock, Rng LCG, logError/logWarn/logInfo). Only allowed headers: iostream, fstream, string, cmath, cstdlib, chrono, ctime.
4. Create src/dynArray.h as in plan 6.2 (template, rule of three, never crashes on bad index).
5. Create src/check_rules.sh implementing the three grep commands from plan 2.4 (excluding src/third_party) and make it print "RULES OK" when nothing matches.
6. Create build files inside src/: Makefile, build.bat, build.sh with targets imagelab, run_tests, make_images (see plan Section 4). Use EXPLICIT file lists (no wildcards) and flags `-std=c++11 -O2 -Wall -Wextra -isystem src/third_party`. Build files may reference files that do not exist yet; that is fine.
7. Create tests/test_framework.h: a tiny framework with a global pass/fail counter, a function `check(bool cond, const std::string& name)` printing "PASS: name" or "FAIL: name", and `int summary()` printing totals and returning non-zero on any failure. No std containers.
8. Create tests/test_main.cpp that calls (not yet existing) test groups; for now just print the summary.
9. Add a .gitignore for binaries and generated files. Commit.

ACCEPTANCE: utils.h and dynArray.h compile with no warnings (write a 10-line throwaway check if needed, then delete it). check_rules.sh prints RULES OK. Report what you did and stop.
````

### Prompt 1 — stackType + queueType + their tests

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 3, 6.3, 6.4 and 7.1 (Stack and Queue cases). Do NOT implement batch operations or priorityQueueType (those are Phase 2).

TASK: implement src/stackType.h and src/queueType.h EXACTLY as specified (names, signatures, behaviour, DECISIONS D1, D2, D3):
- stackType<Type>: growable contiguous array with a moving base index; initializeStack, isEmptyStack, isFullStack, push, top, pop, size, capacity, setGrowable, bottom, removeBottom, peekAt(depthFromTop), push/pop statistics (getPushCount, getPopCount, getMaxSize, resetStats). Compaction only when base >= cap/2, otherwise double. Constructor stackType(int stackSize = 100).
- queueType<Type>: CIRCULAR array with head index and count variable, no shifting; initializeQueue, isEmptyQueue, isFullQueue, front, back, addQueue, deleteQueue, size, capacity, setGrowable, itemAt(i) (returns pointer, nullptr if invalid), add/delete statistics and max size. Growth must copy elements in logical order starting from head so wrap-around is preserved. Constructor queueType(int queueSize = 100). Not growable by default.
- Both: destructor, deep-copy copy constructor, overloaded operator= (self-assignment safe). No crash on empty/full: print a clear [ERROR] message and return Type() / do nothing. No assert, no exit, no exceptions, no std containers, no <cstring>/<algorithm>.
- Everything in the header (templates). Every method gets a short comment explaining the logic and its complexity.

TESTS: create tests/test_stack.cpp and tests/test_queue.cpp implementing every case in plan 7.1 for Stack and Queue (use int and a small struct as Type). Hook them into tests/test_main.cpp. Build run_tests and run it.

ACCEPTANCE: zero compiler warnings; every test prints PASS; src/check_rules.sh prints RULES OK. Then `git commit` and tag p1-ds. In your reply list the complexity of each function (I need it for the report) and any deviation from the plan.
````

### Prompt 2 — Image, I/O, image operations, test images

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 3, 6.5, 6.6, 7.1 (Image ops cases) and 8.

TASK:
1. src/Image.h/.cpp: Pixel struct (static_assert sizeof==3), pixelEq, Image class with rule of three, create, at (checked), inBounds, equals, fillAll, raw, byteSize, width, height, isEmpty.
2. src/image_io.h/.cpp: loadImage (stb, force 3 channels) and saveImage (png/bmp/jpg/tga by extension, default png). This is the ONLY file allowed to include stb headers and define STB_IMAGE_IMPLEMENTATION / STB_IMAGE_WRITE_IMPLEMENTATION. stb may be used ONLY for reading/writing files.
3. src/ImageOps.h/.cpp (namespace ops): crop, rotate, flip, brightness, grayscale, resize (bilinear), blur (separable box, in-bounds averaging) with the exact formulas, validation and ranges from plan 6.6. Everything implemented by hand on the pixel array. No library does any operation. Use int arithmetic for brightness. All functions validate input and never crash.
4. tests/make_test_images.cpp: generates the images listed in plan Section 8 into images/ (run it once and keep the generated small PNGs).
5. tests/test_image_ops.cpp with the Image-ops cases from plan 7.1 (rotate 4x90 identity, 90 then 270 identity, non-square dims, flips, clipping, grayscale, crop validation including border, resize dims, blur edge cases). Hook into test_main.cpp.

ACCEPTANCE: zero warnings, all tests PASS, RULES OK, a load -> rotate -> save round trip works on images/gradient_64.png (write a tiny temporary driver, then delete it). Commit and tag p1-image. Reply with a table of each op and its time complexity.
````

### Prompt 3 — Bucket fill (stack and queue)

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.7 and the Bucket-fill cases in 7.1.

TASK: implement src/BucketFill.h/.cpp with
  bool bucketFill(Image&, int sx, int sy, Pixel newColor, bool useStack, FillStats*, dynArray<unsigned int>* changedOut, int* visitOrderOut)
following the iterative algorithm in plan 6.7 EXACTLY (DECISION D7): recolour on insert, 4-connectivity only, neighbour order Up, Down, Left, Right, early exit if replacement colour equals original colour, start pixel validated, never access outside the image, NO RECURSION, queueType<int>/stackType<int> from my own headers (growable, initial capacity 1024; queue via setGrowable(true)). Fill FillStats (insertions, maxSize from the structure's max-size statistic, changed, usedStack, ms via Timer).

TESTS: tests/test_fill.cpp. Write an independent ORACLE inside the test using an iterative sweep (repeat full passes marking pixels reachable from the start until nothing changes; no recursion, no containers). Implement every case from 7.1: single-pixel region (checkerboard), whole image, irregular region (hand drawn pattern), region touching the border, several separate same-colour regions (only the start's region changes), diagonal-only neighbours not filled, same-colour replacement terminates, out-of-bounds start rejected, 1x1 and 1xN images, 1024x1024 uniform image does not crash. On EVERY case assert: stack result == queue result == oracle result, and changed-pixel count equals the oracle's region size.
Also print (not assert) for a 16x16 uniform image the first 20 visited pixel coordinates for each version, to see the different orders.

ACCEPTANCE: zero warnings, all tests PASS, RULES OK. Manually confirm in your reply that no function in BucketFill calls itself. Commit, tag p1-fill.
````

### Prompt 4 — History, Undo/Redo (Phase 1: full snapshots)

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.8.1, 6.8.2 and the Undo/Redo cases in 7.1. This is PHASE 1: records store a full snapshot copy of the previous image (allowed). Do NOT implement the memory budget, compact records, or coalescing (Phase 2) but keep the code easy to extend (record fields memBytes, regionX/Y/W/H, seqNo, timeMs, coalescedCount exist already, set sensibly).

TASK:
1. src/HistoryRecord.h/.cpp: OpType enum, opName(), HistoryRecord class (copy deleted with `= delete`, destructor deletes snapshot) with fields from plan 6.8.1.
2. src/HistoryManager.h/.cpp: two stackType<HistoryRecord*> (undo, redo); recordOperation (takes ownership, clears and DELETES redo records), undo, redo, clearAll, undoSize, redoSize, undoAt/redoAt (use stackType::peekAt), destructor deletes all remaining records.
3. A shared function `performOp(Image&, OpType, const int p[4], Pixel color, bool useStack)` (put it in a small file, e.g. src/OpRunner.h/.cpp) that executes an operation from its parameters; both normal execution and redo use it (no duplicated logic). Fill redo re-runs bucketFill.
4. Undo = restore snapshot (deep assignment). Redo = performOp from params. Undo/redo with empty stacks print a message and return false.
5. tests/test_history.cpp: several undos restore each earlier image exactly (keep copies of the image after every step and compare with Image::equals); several redos; new action after undo clears redo (check redoSize()==0 and that records are freed, e.g. via a static live-record counter in HistoryRecord used only for tests); the Rotate, Crop, Blur example from the assignment (undo stack top-first Blur, Crop, Rotate; after one undo: Crop, Rotate / redo Blur) checked through undoAt/redoAt names; undo/redo on empty; no memory leak (live-record counter returns to 0 after the manager is destroyed).

ACCEPTANCE: zero warnings, all tests PASS, RULES OK. Commit, tag p1-history.
````

### Prompt 5 — Jobs, FIFO pipeline, engine

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.9 (6.9.1 only; skip 6.9.2-6.9.4 which are Phase 2 but keep the Job fields), 6.10, and the Pipeline cases in 7.1.

TASK:
1. src/Job.h/.cpp: JobType, Job struct (all fields from 6.9, initialised in constructor; leave operator< and epoch logic for Phase 2), jobName(job), jobPriority(type) giving 1/2/2/3/4/4 per the table (set it already, unused in FIFO).
2. src/Pipeline.h/.cpp: queueType<Job> FIFO, bounded with configurable maxCapacity (default 20), submit (reject when full with the exact policy message, DECISION D4), listPending (uses itemAt, does not remove), cancel(id) (mark cancelled, NO shifting), clearAll (loop deleteQueue), processNext/processAll (skip cancelled at the front; measure startMs/endMs, waiting-time statistics), pendingCount/activeCount, setCapacity (only when empty).
3. src/ImageLab.h/.cpp (engine, plan 6.10): owns current Image, HistoryManager, Pipeline. Instant ops: cropOp, rotateOp, flipOp, brightnessOp, grayscaleOp (validate -> create record with snapshot -> apply -> record; failure records nothing). Queued: submitResize, submitBlur, submitFill, submitSave, submitExport. requestUndo/requestRedo: run immediately if pipeline.activeCount()==0 else submit as job. executeJob(Job&) switches on type; queued ops are recorded in history WHEN EXECUTED; parameters validated at execution (DECISION D6); Save writes imagelab_project.png, Export writes the given name. Save/Export create no history records. Operations before any image is loaded print an error.
4. tests/test_pipeline.cpp covering: FIFO order via observable results (e.g. job names recorded in an execution log array in the engine), list does not remove, cancel is skipped at the front and queue is not shifted, clear, full-queue rejection (capacity 3), undo requested while jobs are pending becomes a job and executes in sequence, history is recorded at execution not at submission, invalid queued job (e.g. crop-like invalid fill coordinate after a resize) fails gracefully.

ACCEPTANCE: zero warnings, all tests PASS, RULES OK. Commit, tag p1-pipeline.
````

### Prompt 6 — Inspector, console menu, main (Phase 1)

````
Read IMPLEMENTATION_PLAN.md sections 0, 3, 6.11 and 6.12 (Phase 1 parts only).

TASK:
1. src/Inspector.h/.cpp: prints the Phase 1 metrics in the format of plan 6.11: undo/redo stack sizes and contents (operation names, top first), push and pop counts of both stacks, current queue size and maximum recorded size, addQueue and deleteQueue counts, contents of pending jobs (names + PENDING/CANCELLED), last bucket-fill structure/insertions/maxSize.
2. src/Menu.h/.cpp and src/main.cpp: the console menu with the FIXED numbering from plan 6.12 (items for Phase 2 may print "Not available in Phase 1" or be omitted but keep numbers stable). Robust input: validate every read, clear the stream on bad input, handle EOF by exiting cleanly (never loop forever). Bucket fill asks: x y, replacement r g b, and stack or queue. Export asks the file name.
3. Ensure `imagelab` builds and works end-to-end. Create tests/scripts/demo_p1.txt: a text file of menu inputs (one per line) that demonstrates the full demo flow from plan Section 10 (Phase 1 part), runnable via `imagelab < tests/scripts/demo_p1.txt`.
4. Update the Makefile/build.bat/build.sh so `imagelab`, `run_tests`, `make_images` all build.

ACCEPTANCE: run the demo script and show me the output; every saved image opens correctly (check the files exist and sizes are sensible). Zero warnings, tests PASS, RULES OK. Commit, tag p1-final-candidate.
````

### Prompt 7 — Phase 1 audit, README, report, packaging

````
Read IMPLEMENTATION_PLAN.md sections 2, 4, 9.1, 11, 12.

TASK (no new features):
1. AUDIT: run src/check_rules.sh. Then review ALL source files for: forbidden headers/functions, any recursion, any use of std containers, wrong class/function names versus plan Section 6, memory leaks or double frees (build the tests once with `-g -fsanitize=address,undefined` if the platform supports it and fix findings), uninitialised members, missing validation. Fix everything you find; list the findings.
2. Make sure that nothing tagged [P2] exists in the code (no priorityQueueType, batch functions, budgets, coalescing, scheduling modes, experiments). If any exists, remove it from the Phase 1 tree.
3. Write README.txt: requirements (g++ C++11, stb only for file I/O), exact build and run commands for Windows and Linux with explicit file lists, how to run tests, menu guide, the Decisions log (Section 12, Phase 1 entries only), known limitations.
4. Write report/report.md for the 3-page Phase 1 report following plan 9.1 (representations + justification, complexity table with one-line reasons for every stack and queue operation, empty/full policy, full-queue policy, test results including the actual PASS output summary). Keep it to about 3 pages (roughly 1200-1500 words with one or two compact tables).
5. Create a clean packaging folder named <ROLL>_<SEC>_A2_P1 (leave the placeholder; I will rename) containing ONLY: src/ (sources + third_party + build scripts), tests/ (test sources + scripts), images/, report/ (md only; I will add the PDF), README.txt. Verify no binaries or git files are inside. Then in a temp directory build from that clean copy using only README steps, run the tests, and run the demo script. Report the result.

Finally print a checklist from plan Section 11 with each item marked done / needs-me.
````

---

## PHASE 2

> Before starting: `git tag p1-final` (the exact tree you zipped for Phase 1). Phase 2 continues in the same repo.

### Prompt P2-0 — Start Phase 2 (priority queue)

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.13 and the Priority-queue cases in 7.2. We are now in PHASE 2 (the Phase 1 code is tagged p1-final and must keep working).

TASK: implement src/priorityQueueType.h exactly per plan 6.13 (DECISION D12): ordered singly linked list, head = next to serve, template requiring Type::operator<(const Type&) meaning "served before", insertion BEFORE the first node n where item < n.info (stable for ties), optional tail pointer fast path, front, deleteQueue, size, isEmptyQueue, isFullQueue (false), initializeQueue, itemAt(i), statistics (adds, deletes, max size), deep copy constructor, operator=, destructor; empty/error handling as in D3.
Add the Job::operator< (epoch, then priority, then seq) in Job.h/.cpp now.
TESTS: tests/test_pq.cpp: priorityQueueType<int> (smaller first), same-priority arrival order, mixed priorities, 1,000-element sorted drain, tie handling with a struct, copy/assign, empty cases. Job-level finalization tests come in Prompt P2-5.
ACCEPTANCE: zero warnings, all tests (old and new) PASS, RULES OK. Commit, tag p2-pq. Give me the complexity of addQueue/deleteQueue with the reason.
````

### Prompt P2-1 — Batch operations

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.14 and the Batch cases in 7.2.

TASK: add pushBatch/popBatch to stackType and addQueueBatch/deleteQueueBatch to queueType with the exact signatures from the assignment:
  void pushBatch(const Type items[], int n); void popBatch(int n); void addQueueBatch(const Type items[], int n); void deleteQueueBatch(int n);
Semantics (DECISION D15): equivalent to n single operations; n<=0 does nothing; popBatch/deleteQueueBatch clamp n to size with one warning; growable structures grow ONCE to max(2*cap, count+n); fixed-capacity structures add only as many items as fit and print one warning; queue copies into at most two contiguous segments across the wrap-around; deleteQueueBatch is O(1) index arithmetic; statistics (pushes/pops/adds/deletes/max size) updated correctly by n. Do not call the single operations in a loop (that defeats the purpose). Comment each function with why it is faster.
TESTS: tests/test_batch.cpp: batch larger than free space (growable and fixed), batch across the wrap-around (check order afterwards), popBatch/deleteQueueBatch with n larger than size, n=0 and negative n, and a randomized equivalence test: run 200 random mixes of single and batch operations on two structures (one using only single ops, one using batch where applicable) and assert identical contents and statistics.
ACCEPTANCE: zero warnings, all tests PASS, RULES OK. Commit, tag p2-batch.
````

### Prompt P2-2 — Compact memory-aware history

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.8.3, 6.8.4 and the Memory-aware-history cases in 7.2. Skip coalescing (next prompt) but leave a clean hook.

TASK:
1. Extend HistoryRecord with the compact reverse data per plan 6.8.4: crop (outside pixels in raster order + prevW/prevH), brightness (net delta, deltas[], sorted clip list of {idx, orig Pixel}), bucket fill (origColor + changed index list + replacement colour), snapshot only for grayscale/blur/resize. No full-image snapshot for rotate/flip/brightness/crop/fill any more. Compute memBytes with the formula in 6.8.3. Set regionX/Y/W/H as in 6.8.4.
2. Add ops::brightnessRecord (clip list sorted ascending, any-channel-clipped pixels with their pre-op value) while keeping ops::brightness for plain use.
3. Update the engine to build compact records and HistoryManager to reverse/re-apply them per the table in 6.8.4. Undo of Crop and Resize must restore dimensions.
4. HistoryManager budget: setBudgetBytes/getBudgetBytes/getUsedBytes/getDiscardedCount, enforceBudget (oldest first from undo bottom using stackType::bottom()/removeBottom(), then redo bottom), oversized-record rule (DECISION D9: do not store, clear both stacks, warn). Used bytes must always equal the sum over both stacks (add an internal consistency check function used by tests).
5. TESTS tests/test_budget.cpp: for EVERY op type (rotate 90/180/270, flip h/v, brightness with heavy clipping, crop, fill, grayscale, blur, resize) do op -> undo and assert the image equals the saved copy exactly, then redo and assert equals the post-op copy; budget reached (small budget such as 200 KB with a 128x128 image and many ops) and oldest discarded first (check undoSize and that the newest records still undo correctly); undo after cleanup; oversized record clears history with warning; used-bytes consistency after every operation in a random sequence of 300 ops.
ACCEPTANCE: zero warnings; ALL tests (including Phase 1 ones, adjusted only if they relied on snapshots) PASS; RULES OK. Commit, tag p2-history. Summarise the memory per record for each op type.
````

### Prompt P2-3 — Command coalescing

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.8.5 and the Coalescing cases in 7.2. This is the trickiest correctness part: follow the merge algorithm in 6.8.5 exactly.

TASK:
1. HistoryManager::setCoalescing(bool) (default true). When recording a brightness op: if the Undo-stack top is an OP_BRIGHTNESS record and coalescing is on, merge instead of pushing: the new op has already been applied to the image with clamping and produced its own clip list (idx, preValue-before-this-op). Merge via a two-pointer walk of the two sorted lists: keep all existing entries; for each new entry whose idx is not already present add (idx, preValue - S) per channel where S is the record's net delta BEFORE this op. Then net += delta, deltas.push_back(delta), coalescedCount++, update seqNo/timeMs, recompute memBytes, adjust used bytes, enforceBudget (oversized rule applies), clear the Redo stack as for any new op.
2. Undo of a merged record: for every pixel subtract net from each channel (int math), then overwrite every clip pixel with its orig. Redo: apply the deltas in order with clamping (if all deltas share a sign, apply net once).
3. Only ADJACENT top-of-stack records merge: Brightness +2, Crop, Brightness +3 stays three records.
4. Inspector/engine counter of coalesced operations.
5. TESTS tests/test_coalesce.cpp: +1 four times -> exactly one record with net +4 and coalescedCount 4; +2, crop, +3 -> three records; undo/redo of a combined record; clipping cases (+200 then -100; -200 then +100; +100,+100,-250; values starting at 0, 100, 250); RANDOMIZED PROPERTY TEST: 100 random images (size up to 20x20, random pixels incl. 0 and 255 extremes) x random brightness sequences of length 1-12 with deltas in -300..300: (a) final image equals naive sequential application of the same deltas, (b) one undo restores the original exactly and leaves an empty undo stack, (c) one redo reproduces the final image exactly. Use the Rng from utils.h with fixed seeds.
ACCEPTANCE: zero warnings, ALL tests PASS, RULES OK. Commit, tag p2-coalesce. Explain in your reply, in plain language, why +200 then -100 is handled correctly (I must explain it in the viva).
````

### Prompt P2-4 — Scheduler: FIFO vs Priority, barriers, costs

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.9 (all of it incl. 6.9.2-6.9.4), and the Finalization cases in 7.2.

TASK: extend Pipeline/ImageLab:
1. SchedMode {MODE_FIFO, MODE_PRIORITY}. Pipeline holds queueType<Job> fifo and priorityQueueType<Job> pq; use only the active one. setMode only when no jobs are pending (DECISION D13). Capacity limit applies identically; cancel/list/clear/processNext/processAll work in both modes (list prints in service order).
2. Epoch logic (6.9.3): currentEpoch counter; on submit job.epoch = currentEpoch; if the job is Save/Export then currentEpoch++ afterwards. Priorities: Undo/Redo 1, Blur 2, Fill 2, Resize 3, Save 4, Export 4. Undo/Redo submitted while jobs pending get priority 1.
3. estCost per 6.9.4 set at submission; measured start/end times; waiting time = start - submit, completion = end - submit. Provide a Metrics struct computed over completed (non-cancelled) jobs: average waiting time, average completion time, average waiting time of priority-1 jobs, number of jobs processed. Also provide `simulateSchedule(jobsInSubmissionOrder, mode)` computing the same metrics from estCost only (virtual clock, no execution).
4. TESTS tests/test_sched.cpp: the assignment example Resize(3) Blur(2) Export(4) Blur(2) Undo(1) -> service order Blur, Resize, Export, Undo, Blur (check via an execution-order log); two barriers; same-priority arrival order; jobs after a barrier never overtake it; cancelled barrier job; capacity rejection in priority mode; mode switch rejected when jobs pending; 1,000-job synthetic drain keeps (epoch, priority, seq) order; FIFO mode order unchanged from Phase 1.
ACCEPTANCE: zero warnings, ALL tests PASS, RULES OK. Commit, tag p2-sched.
````

### Prompt P2-5 — Inspector + menu for Phase 2

````
Read IMPLEMENTATION_PLAN.md sections 0, 6.11, 6.12.

TASK: extend Inspector and Menu for Phase 2 with the fixed numbering: 18 scheduling mode (show current mode, switch only when idle), 21 Inspector now also shows history memory used vs budget, records discarded, coalesced operation count, current scheduling mode, average queue waiting time (recommended metric), 23 experiments entry (stub calling Experiments::runMenu, implemented next), 24 set history memory budget (accept value + unit MB or KB), 25 toggle coalescing. Job list shows priority and epoch in priority mode. Create tests/scripts/demo_p2.txt: a scripted demo of the Phase 2 flow in plan Section 10 (budget 200 KB, many ops until records are discarded, Inspector, priority mode with a mixed workload, finalization barrier visible in the execution order).
ACCEPTANCE: run the demo script and show the output; zero warnings; all tests PASS; RULES OK. Commit.
````

### Prompt P2-6 — Experiments

````
Read IMPLEMENTATION_PLAN.md sections 0, 2, 6.15 and 9.2 (experimental results part).

TASK: implement src/Experiments.h/.cpp and menu 23 with a submenu: 1 bucket fill, 2 history, 3 FIFO vs priority, 4 single vs batch, 5 run all. Each repeats timings 3 times, prints a table (average, min, max) and writes results_<name>.csv in the current directory. Use only allowed headers (chrono timing, fstream for CSV, cout.setf/precision for formatting). Keep a checksum so the optimiser cannot delete work.
- E1 bucket fill: uniform images 512, 1024, 2048 (4096 only if the user enters "big"), fill from (0,0), stack vs queue; metrics ms, insertions, maxSize, changed. Also a striped/maze image of 512. For a 16x16 uniform image print the visit order grid for both versions, and write out_visit_stack.png/out_visit_queue.png (256x256, grey level = visit index scaled to 0..255).
- E2 history: 100 and 1,000 seeded random operations on a 256x256 generated image under a fixed 5 MB budget with coalescing ON and OFF: used bytes, records discarded, number of records, undo-all time, and the bytes a full-snapshot history would have needed. The random mix must keep every operation valid.
- E3 scheduling: a deterministic workload of 36 jobs containing all job types and at least 2 Save/Export barriers on a 512x512 generated image (fill at (1,1), resizes between 384 and 640, pre-seed history with 8 non-brightness ops so Undo jobs find records); run to completion under FIFO and under Priority from identical starting state; print the four metrics (average waiting, average completion, average waiting of priority-1 jobs, jobs processed) from measured times, plus the estimated-cost simulation table. Capacity must be large enough (>=40).
- E4 batch: 1,000 / 10,000 / 100,000 items for stack push/pop and queue add/delete, single vs batch, speed-up column. Start containers with small capacity so growth cost appears for singles.
ACCEPTANCE: running "5 run all" completes in reasonable time (<2 min at -O2 without the 4096 option), CSVs are written, no crashes, RULES OK. Commit, tag p2-exp. Paste the printed tables in your reply and add 2-3 factual observations per experiment (I will put them in the report).
````

### Prompt P2-7 — Final audit, README, report, packaging (Phase 2)

````
Read IMPLEMENTATION_PLAN.md sections 2, 4, 7, 9.2, 11, 12, 13.

TASK:
1. AUDIT everything as in Prompt 7 (rules grep, manual recursion review, forbidden headers, names versus plan Section 6, sanitizer build, leaks). Also verify EVERY test case listed in plan 7.1 and 7.2 exists and prints PASS/FAIL, and that the Inspector shows every Phase 2 metric. Fix findings and list them.
2. README.txt for Phase 2: build/run, libraries, menu guide, full Decisions log D1-D20, how to run tests and experiments.
3. report/report.md: Phase 2 report (max 10 pages) with the 7 sections in plan 9.2: history record design and cleanup policy with justification; coalescing rules and the clipping-correctness argument; scheduler design (ordered linked list, tie handling, epoch-based finalization rule with the assignment example); experimental results E1-E4 using the REAL numbers from the CSVs (tables, say measured vs estimated) with short discussions; complexity analysis table covering all 8 rows of assignment 7.6 with a 1-2 line "why" for each; testing evidence (paste summary of PASS output); known limitations. Also produce results/graph data as simple tables I can chart (the report may use tables only if no graph is available).
4. Create the packaging folder <ROLL>_<SEC>_A2_P2 (placeholder) with ONLY src/, tests/, images/, report/, README.txt. Rebuild from this clean copy in a temp directory using only README steps; run tests and both demo scripts; report results.
5. Print the Section 11 checklist with done / needs-me for each line.
````

---

## EXTRA PROMPTS (use any time)

### Prompt X-1 — Independent bug hunt (run once per phase before packaging)

````
Act as a strict TA grading this project with the assignment rubric in IMPLEMENTATION_PLAN.md. Do not add features. 1) Try to CRASH the program: empty/full structures, bad menu input, EOF on stdin, huge coordinates, negative sizes, non-numeric input, loading a missing file, operating before loading an image, cancelling a non-existent job id, undo/redo with nothing to undo, resizing to 0x0 or huge sizes, queue capacity 0 or negative. 2) Check marks-relevant behaviour: every requirement in assignment sections 5, 6, 7 (use the plan's tables) is DEMONSTRABLE at run time through the menu/Inspector or the test program. 3) Search for forbidden items (containers, extra headers, recursion, threads). 4) Check class and function names against plan Section 6. Report every issue with file:line and fix them, then re-run all tests.
````

### Prompt X-2 — Study guide for the demo/viva

````
I must explain this code to a TA in a live demo. For each source file produce a short study note (put them in docs-study/ which will NOT be shipped): purpose in one sentence, the key functions in plain English, the invariants, the complexity with reasons, and 3 likely TA questions with short answers. Cover especially: circular queue wrap-around and growth, stack base-index compaction, bucket fill (why recolour-on-insert, stack vs queue visit order), undo/redo flow, history records and what each op stores, budget cleanup and the oversized-record rule, brightness coalescing with clipping (walk through +200 then -100 with real numbers), the priority queue insertion and the epoch-based finalization rule, batch operations. Finish with a 10-minute demo script and the 20 hardest questions with answers.
````

### Prompt X-3 — Repair prompt (when the agent breaks something)

````
Something is broken: <describe, paste the compiler/test output>. Do NOT rewrite unrelated files. 1) Reproduce the failure and show the exact failing test or output. 2) Find the root cause (explain it in 2-3 sentences). 3) Make the smallest possible fix. 4) Re-run ALL tests and src/check_rules.sh. 5) Show `git diff --stat`. If the cause is a mismatch with IMPLEMENTATION_PLAN.md, follow the plan.
````

### Prompt X-4 — Report to PDF (if you have no export tool)

````
Convert report/report.md into a clean, compact document that I can export to PDF within the page limit (P1: 3 pages, P2: 10 pages). Reduce wording, use compact tables, 10.5-11 pt equivalent, narrow margins. Tell me an estimated page count. Do not invent numbers; only use results that exist in the test output and results_*.csv files.
````
