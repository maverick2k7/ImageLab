# ImageLab — Implementation Plan (DSA Assignment 2)

> Put this file in the repo root next to `PROMPTS.md`. Every prompt tells the agent to read the relevant sections first.
> Tags: **[P1]** = needed in the Phase 1 zip. **[P2]** = Phase 2 only (do NOT put in the P1 zip).
> `DECISION Dn` = ambiguity in the assignment, resolved here. Because the query deadline has passed, these are written into README + reports as "assumptions".

---

## 0. RULES FOR THE CODING AGENT (read before every task)

1. Work **one module at a time**, in the order of Section 5. After each module: compile with **zero warnings**, run the tests, `git commit`, then stop and summarise.
2. **Never** use anything from the forbidden list (Section 2). If a helper is missing, write your own.
3. **Never rename** the public API in Section 6. TAs call these names directly.
4. **Never crash**: no `exit()`, `abort()`, `assert()`, `throw`, no unchecked pointer use. Errors = print a message and return safely.
5. **No recursion anywhere** (not just in bucket fill).
6. The student must **explain every line in a viva**. Write simple, short functions, comments that say *why*, no clever tricks, no macros (except in the test framework), no operator overloading beyond what is required.
7. Do not add features that are not in this plan. Optional items are marked *(optional)*.
8. If something is ambiguous, use the `DECISION` in this file. Do not ask questions.
9. Output text must be plain ASCII (Windows console safe).
10. Keep Phase 1 and Phase 2 separable: everything tagged [P2] must live in its own files or clearly marked blocks so the P1 snapshot stays clean.

---

## 1. Deliverables and dates

| Item | Detail |
|---|---|
| Phase 1 zip | `<ROLL>_<SEC>_A2_P1.zip` -> folder `<ROLL>_<SEC>_A2_P1/` (e.g. `25i-0000_A_A2_P1`) |
| Phase 2 zip | `<ROLL>_<SEC>_A2_P2.zip` -> folder `<ROLL>_<SEC>_A2_P2/` |
| Deadline | **Both phases: 16 Oct 2026, 11:59 PM**, via Google Classroom only |
| Marks | P1 = 50, P2 = 50, bonus up to 10 (ignore bonus until everything mandatory is done) |
| Penalties | Wrong folder structure/naming = **-20% of phase marks**. Corrupted/wrong zip = **0**. Forbidden library feature = 0 for that part. |
| Grading | Only behaviour **demonstrated at run time** counts (output, saved images, Inspector). Student must explain the code in the demo. |

Student fills in: `<ROLL>` and `<SEC>` (placeholders until known).

---

## 2. HARD CONSTRAINTS (violations = 0 marks for that part)

### 2.1 Allowed
- Raw arrays, `new[]`/`delete[]`, pointers, structs, classes, templates.
- Headers: `<iostream>`, `<fstream>`, `<string>`, `<cmath>`, `<cstdlib>`, `<chrono>`, `<ctime>` — **only these**.
- `std::string` (allowed). `std::cout/cin/cerr`, `std::ifstream/ofstream`, `std::chrono::steady_clock`.
- An image library **only** to read a file into a pixel array and write a pixel array to a file: **stb_image.h + stb_image_write.h** (single-file, public domain), used **only inside `image_io.cpp`**.
- GUI: not building one (optional bonus).

### 2.2 Forbidden
- Any standard container: `vector, list, deque, stack, queue, priority_queue, map, set, array, unordered_*`, etc.
- Library functions that do the operation (rotate, flip, resize, crop, blur, brightness, grayscale, flood fill). stb is for file I/O only: never use `stb_image_resize`, never use stb for any processing.
- **Recursion** (anywhere).
- Threads, mutexes, atomics, any concurrency.

### 2.3 Extra-safe rules (not explicitly listed, avoid to be safe)
Do not include: `<algorithm> <utility> <functional> <memory> <cstring> <cstdio> <iomanip> <sstream> <limits> <cstdint> <cassert> <new> <exception> <stdexcept>`.
- Write own `minVal / maxVal / clampInt / swapVals` (in `utils.h`) instead of `std::min/max/swap/clamp`.
- Write own copy loops instead of `memcpy`/`std::copy`.
- Number formatting: use `std::cout.setf(std::ios::fixed); std::cout.precision(3);` (no `<iomanip>`).
- No smart pointers. Use raw pointers with clear ownership.
- Use `unsigned int`, `long long` instead of `<cstdint>` types.

### 2.4 Automated check (run before every commit and before zipping)
```
grep -rnE "#include *<(vector|list|deque|stack|queue|map|set|array|unordered_map|unordered_set|algorithm|utility|functional|memory|cstring|cstdio|iomanip|sstream|limits|cstdint|cassert|new|thread|mutex|atomic|condition_variable|future|exception|stdexcept)>" src tests
grep -rnE "std::(vector|list|deque|stack|queue|priority_queue|map|set|array|min|max|swap|sort|memcpy|unique_ptr|shared_ptr|thread|mutex)" src tests
grep -rnE "\b(assert|exit|abort|throw)\b" src tests
```
All must print nothing (ignore `src/third_party/`). Put these in `src/check_rules.sh`. Then **manually** confirm no function calls itself (list every function that contains a loop where recursion might hide: bucket fill, history cleanup, copy helpers).

---

## 3. Global conventions

- **Language/compiler:** C++11, `g++ -std=c++11 -O2 -Wall -Wextra`. Must build on Windows (MinGW/MSYS2) and Linux. No platform-specific code, no `system()`.
- **Coordinates:** `x` = column in `[0,W)`, `y` = row in `[0,H)`, origin top-left. Pixel index = `y*W + x` (`int`; cap image at 16384 per side and 50,000,000 pixels).
- **Pixel:** `struct Pixel { unsigned char r, g, b; };` (`sizeof(Pixel)==3`, check with `static_assert`). Images are always loaded as RGB (3 channels, alpha dropped by asking stb for 3 channels). Equality helper `pixelEq(a,b)`.
- **Errors:** functions return `bool` (true = success) and print `[ERROR] ...` / `[WARN] ...` / `[INFO] ...` to `std::cerr`/`std::cout`. A failed operation changes nothing and records no history.
- **Templates in headers only** (`.h`, definitions inside the header).
- **Timing:** `Timer` class in `utils.h` wraps `std::chrono::steady_clock`, returns milliseconds as `double`.
- **Randomness (tests/experiments):** own LCG `unsigned int next()` seeded with a constant -> reproducible.
- **Ownership:** history records are heap objects owned by `HistoryManager`; stacks hold raw pointers to them (DECISION D8).

---

## 4. Folder structure and build (follow EXACTLY: -20% otherwise)

```
<ROLL>_<SEC>_A2_P1/
  src/        all source + header files, third_party/, build scripts (Makefile, build.bat, build.sh, check_rules.sh)
  tests/      test program for the data structures (+ make_test_images.cpp)
  images/     small test images used (generated by make_test_images.cpp)
  report/     report.pdf (the report AS A PDF; report.md may stay as source)
  README.txt  how to build/run, libraries needed, assumptions (Decisions log), menu guide
```
- **Nothing else at the top level.** No `.exe`, `.o`, `build/`, `.git/`, `.vscode/`, output PNGs, CSVs in the zip.
- Build commands in README must be **explicit file lists** (no `*.cpp` wildcards, Windows cmd does not expand them). Provide plain `g++` lines, plus `src/Makefile`, `src/build.bat`, `src/build.sh` as conveniences.
- Programs are run **from the project root** (so `images/xxx.png` paths work). Outputs (saved images, CSVs) are written to the current directory with prefix `out_` / `results_`. Never create directories from code.
- `main()` lives only in `src/main.cpp` so that the test program can link all other `.cpp` files.

Build targets:
```
imagelab   = src/main.cpp + all other src/*.cpp
run_tests  = tests/test_main.cpp + tests/test_*.cpp + all src/*.cpp except main.cpp
make_images= tests/make_test_images.cpp + src/image_io.cpp + src/Image.cpp
```

---

## 5. Architecture and build order

```
 utils.h  dynArray.h
     |         |
 stackType.h  queueType.h  [priorityQueueType.h P2]
     |            |
   Pixel/Image  <-- image_io.cpp (stb)
     |
   ImageOps  (crop rotate flip brightness grayscale resize blur)
     |
   BucketFill (uses stackType<int>, queueType<int>)
     |
   HistoryRecord + HistoryManager (two stackType<HistoryRecord*>)
     |
   Job + Pipeline (queueType<Job>; [P2] priorityQueueType<Job> + epochs)
     |
   ImageLab engine (facade: owns Image, History, Pipeline; executes jobs)
     |
   Inspector   Menu   main        [P2] Experiments
```

Build order = top to bottom. Milestones/git tags: `p1-ds`, `p1-image`, `p1-fill`, `p1-history`, `p1-pipeline`, `p1-final`, then `p2-pq`, `p2-batch`, `p2-history`, `p2-coalesce`, `p2-sched`, `p2-exp`, `p2-final`.

---

## 6. MODULE SPECIFICATIONS

### 6.1 `utils.h` [P1]
- `minVal, maxVal` (templates), `clampInt(int v,int lo,int hi)`, `swapVals`.
- `class Timer { void start(); double elapsedMs() const; static double nowMs(); }` using `steady_clock`.
- `class Rng { Rng(unsigned seed); unsigned next(); int range(int lo,int hi); }` simple LCG (`state = state*1664525u + 1013904223u`).
- Logging helpers: `logError(const std::string&)`, `logWarn`, `logInfo`.

### 6.2 `dynArray.h` [P1]
`template<class T> class dynArray` — growable array (doubling) used for lists of changed pixels, deltas, etc. API: `dynArray(int cap=16)`, `~dynArray`, copy ctor, `operator=`, `void push_back(const T&)`, `T& operator[](int)`, `const T& operator[](int) const`, `int size() const`, `void clear()`, `void reserve(int)`, `T* data()`, `const T* data() const`. Out-of-range `operator[]` prints an error and returns a reference to a member dummy element (never crash).

### 6.3 `stackType<Type>` [P1] (+ batch [P2])

**Representation (DECISION D1):** growable contiguous array with a moving *base* index.
Elements live in `data[base .. base+count-1]`; top = `data[base+count-1]`. Justification for report: contiguous = cache friendly, amortised O(1) push, O(1) `removeBottom` (needed for history cleanup), block-copy batch ops possible.

Members: `Type* data; int cap; int base; int count; bool growable; long long pushes, pops; int maxCount;`

| Function | Behaviour |
|---|---|
| `stackType(int stackSize=100)` | `cap = (stackSize<1)?1:stackSize`; `growable = true`; counters 0 |
| copy ctor, `operator=`, `~stackType` | deep copy of live elements; self-assignment safe; `delete[]` |
| `void initializeStack()` | `count=0; base=0;` (capacity kept, statistics NOT reset) |
| `bool isEmptyStack() const` | `count==0` |
| `bool isFullStack() const` | `!growable && count==cap` (growable default => always false, like the linked version) |
| `void push(const Type&)` | if no room at the end: if `base >= cap/2` **compact** (shift live elements to index 0) else if growable **double** capacity (copy to new array at index 0) else print `[ERROR] stack full`, ignore. Then store, `count++`, `pushes++`, update `maxCount` |
| `Type top() const` | empty -> print error, return `Type()` (needs default ctor) |
| `void pop()` | empty -> print error, no-op; else `count--`, `pops++` |
| `int size() const` | `count` |
| `int capacity() const` | `cap` |
| `void setGrowable(bool)` | |
| `Type bottom() const`, `void removeBottom()` | oldest element; `base++, count--` (if `count` becomes 0, `base=0`). Used **only** by history cleanup. Not counted as a pop? -> DECISION: counted separately in `removedFromBottom` |
| `Type peekAt(int depthFromTop) const` | 0 = top; read-only (Inspector). Invalid depth -> error + `Type()` |
| `getPushCount() getPopCount() getMaxSize() resetStats()` | Inspector metrics |
| `void pushBatch(const Type items[], int n)` [P2] | see 6.14 |
| `void popBatch(int n)` [P2] | see 6.14 |

**Complexity (for the report):** push amortised O(1) (doubling; compaction only when base >= cap/2, also amortised), pop/top/size O(1), removeBottom O(1), copy O(n), space O(capacity) <= 2n.

### 6.4 `queueType<Type>` [P1] (+ batch [P2])

**Representation (DECISION D2):** **circular** array with `head` index and `count` variable. Empty/full test = `count` variable (state this in the report; no reserved slot). Never shifts elements.

Members: `Type* data; int cap; int head; int count; bool growable; long long adds, deletes; int maxCount;`
- `tailIndex = (head+count) % cap` (use conditional subtract instead of `%` if you like).

| Function | Behaviour |
|---|---|
| `queueType(int queueSize=100)` | `cap = max(1, queueSize)`; `growable=false` (bounded by default) |
| copy ctor, `operator=`, dtor | deep copy in **logical order** (front first), self-assign safe |
| `initializeQueue()` | `head=0; count=0` |
| `isEmptyQueue()` | `count==0` |
| `isFullQueue()` | `!growable && count==cap` |
| `addQueue(const Type&)` | full & not growable -> print `[ERROR] queue full`, reject. full & growable -> **grow**: new array of `2*cap`, copy elements in logical order starting at 0, `head=0`. Then store at tail, `count++`, `adds++`, `maxCount` update |
| `front()`, `back()` | empty -> error, return `Type()` |
| `deleteQueue()` | empty -> error, no-op; else `head=(head+1)%cap; count--; deletes++` |
| `size()`, `capacity()` | `count`, `cap` |
| `setGrowable(bool)` | bucket fill queue uses `true` |
| `Type* itemAt(int i)` / `const Type* itemAt(int i) const` | logical index from front; `nullptr` if invalid (used for list and cancel) |
| stats getters | `getAddCount() getDeleteCount() getMaxSize() resetStats()` |
| `addQueueBatch / deleteQueueBatch` [P2] | see 6.14 |

**Complexity:** add/delete/front/back/size O(1) (index arithmetic, no shifting), growth O(n) amortised O(1), space O(cap).

### 6.5 Image, Pixel, image I/O [P1]
- `Image` class: `int w,h; Pixel* px;` Rule of three (deep copy). `Image()`, `Image(int w,int h)` (zero-filled), `create(w,h)`, `width()`, `height()`, `at(x,y)` (checked, returns dummy on bad coords), `raw()`, `isEmpty()`, `equals(const Image&)`, `fillAll(Pixel)`, `inBounds(x,y)`, `byteSize()`.
- `image_io.h/.cpp`: `bool loadImage(const std::string& path, Image& out)` (stb with `req_comp = 3`), `bool saveImage(const std::string& path, const Image& img)` (extension picks png/bmp/jpg/tga, default png). **Only file defining the stb implementation macros.** stb headers in `src/third_party/` (download from `https://raw.githubusercontent.com/nothings/stb/master/stb_image.h` and `stb_image_write.h`; compile with `-isystem src/third_party` to hide their warnings). If no internet: write a tiny 24-bit BMP reader/writer by hand instead (fallback).
- Bad file / unsupported -> error message, `Image` unchanged.

### 6.6 `ImageOps` [P1] — all hand-written, namespace `ops`

| Op | Signature | Rules |
|---|---|---|
| Crop | `bool crop(Image&, int x,int y,int w,int h)` | valid iff `w>=1,h>=1,x>=0,y>=0,x<W,y<H, w<=W-x, h<=H-y` (no overflow). New image = region copy |
| Rotate | `bool rotate(Image&, int angle)` | angle in {90,180,270} clockwise else error. 90: new size `HxW`, `dst(H-1-y, x) = src(x,y)`. 180: `dst(W-1-x,H-1-y)=src(x,y)`. 270: new size `HxW`, `dst(y, W-1-x) = src(x,y)`. Inverse of 90 is 270, of 180 is 180 |
| Flip | `bool flip(Image&, bool horizontal)` | horizontal: `dst(W-1-x,y)=src(x,y)`; vertical: `dst(x,H-1-y)=src(x,y)`. Self-inverse |
| Brightness | `void brightness(Image&, int delta)` | per channel `v = clampInt(v+delta,0,255)` using `int` math. Delta any int |
| Grayscale | `void grayscale(Image&)` | `g = (299*r + 587*g + 114*b + 500) / 1000`; set r=g=b |
| Resize | `bool resize(Image&, int nw, int nh)` | `nw,nh` in [1,16384], `nw*nh <= 50M` (use `long long`). **Bilinear** (DECISION D18): `sx=(i+0.5)*W/nw-0.5` clamped to `[0,W-1]`, same for y; blend 4 neighbours, round |
| Blur | `bool blur(Image&, int radius)` | `radius` in [1,50]; box blur over `(2r+1)^2` window, **only in-bounds pixels averaged**. Implement as two passes (horizontal then vertical), integer sums, rounding. Deterministic |

[P2] adds recording variants (see 6.9): `brightnessRecord(Image&, int delta, dynArray<ClipEntry>& clipped)` where `struct ClipEntry { unsigned int idx; Pixel orig; }` lists, in ascending `idx`, every pixel for which **any channel** was clipped, with its value **before** this op.

### 6.7 `BucketFill` [P1]
```
struct FillStats { long long insertions; int maxSize; int changed; bool usedStack; double ms; };
bool bucketFill(Image& img, int sx, int sy, Pixel newColor, bool useStack,
                FillStats* stats,                 // may be null
                dynArray<unsigned int>* changedOut, // may be null: indices changed, in discovery order
                int* visitOrderOut);              // may be null: size W*H, caller pre-fills -1; set to visit number when popped
```
**Iterative algorithm (DECISION D7: recolour-on-insert, 4-connectivity, neighbour order Up, Down, Left, Right):**
```
if (!inBounds(sx,sy)) { error; return false; }
target = img.at(sx,sy)
if (pixelEq(target,newColor)) { stats zeroed; info "nothing to do"; return true; }   // no infinite loop
start = sy*W+sx;  img[start] = newColor; changedOut.push(start); ds.insert(start);
order = 0
while (!ds.empty()):
    cur = ds.remove()            // stack: top()+pop(); queue: front()+deleteQueue()
    visit[cur] = order++         // if visitOrderOut
    x = cur % W; y = cur / W
    for each neighbour (nx,ny) in order U,D,L,R:
        if inBounds(nx,ny) and pixelEq(img[nidx], target):
            img[nidx] = newColor; changedOut.push(nidx); ds.insert(nidx)
```
- `ds` is `stackType<int>` (growable) or `queueType<int>` with `setGrowable(true)`. Initial capacity 1024.
- Because pixels are recoloured when inserted, each pixel is inserted at most once -> `insertions == changed == region size`; `maxSize` differs (stack vs queue) and so does visit order.
- Both versions must produce the **same image**. No recursion. No access outside the image.
- Complexity: time O(R) (R = region size, 4 neighbour checks each), auxiliary space O(R) worst case (queue frontier ~ O(perimeter) typically, stack can hold O(R)).

### 6.8 History

#### 6.8.1 Types
```
enum OpType { OP_CROP, OP_ROTATE, OP_FLIP, OP_BRIGHTNESS, OP_GRAYSCALE, OP_BLUR, OP_RESIZE, OP_FILL };
std::string opName(OpType)   // "Crop","Rotate","Flip","Brightness","Grayscale","Blur","Resize","Bucket fill"
```
`HistoryRecord` (heap object, **copy disabled with `= delete`**, destructor frees its buffers):
- common: `OpType type; int p[4]; Pixel color; long long seqNo; double timeMs; int regionX,regionY,regionW,regionH; size_t memBytes; int prevW, prevH; int coalescedCount;`
- params meaning: Crop `x,y,w,h`; Rotate `angle`; Flip `horizontal(0/1)`; Brightness `net delta` (p[0]); Blur `radius`; Resize `nw,nh`; Fill `x,y,useStack`, `color` = replacement.
- [P1] reverse info: `Image* snapshot` = full copy of the image **before** the op (allowed in Phase 1).
- [P2] compact reverse info (replaces snapshot for rotate/flip/brightness/crop/fill; snapshot kept only for grayscale/blur/resize = "previous pixels of the affected region"): see 6.8.4.

#### 6.8.2 `HistoryManager` [P1]
```
stackType<HistoryRecord*> undoStack, redoStack;      // growable
bool recordOperation(HistoryRecord* rec);   // takes ownership. clears redo stack (deleting records), pushes on undo
bool undo(Image& img);   // pop undo -> reverse -> push redo.  empty => message, false
bool redo(Image& img);   // pop redo -> re-apply -> push undo.  empty => message, false
void clearAll(); int undoSize() const; int redoSize() const;
const HistoryRecord* undoAt(int depthFromTop) const; const HistoryRecord* redoAt(int depthFromTop) const;
~HistoryManager()  // deletes every record still on either stack
```
Rules (assignment 6.3): a new operation after undo **clears the Redo stack**; undo moves top of Undo to Redo; redo moves back; example `Rotate,Crop,Blur` -> Undo (top first) `Blur,Crop,Rotate`, Redo empty; after one undo: Undo `Crop,Rotate`, Redo `Blur`.

Applying/reversing:
- **Forward re-apply (redo)** and normal execution share one function `performOp(Image&, OpType, params, ...)` in the engine so there is no duplicated logic. Redo of a fill/blur/resize re-runs the deterministic op from `params`.
- **[P1] reverse:** `img = *rec->snapshot` (deep copy assignment).
- Engine flow for every state-changing op: validate -> `rec = new HistoryRecord(...)`; (P1: `rec->snapshot = new Image(img)`) -> apply -> if apply failed `delete rec` (no history) else `recordOperation(rec)`.
- Save and Export never touch the image -> **no history**.

#### 6.8.3 Phase 2 budget + cleanup [P2] (DECISIONS D9, D10)
- `setBudgetBytes(size_t)`, `getBudgetBytes()`, `getUsedBytes()` (sum of `memBytes` of **both** stacks, maintained incrementally), `getDiscardedCount()`.
- `memBytes` estimate = `sizeof(HistoryRecord) + snapshotBytes + savedPixels*sizeof(Pixel) + clip*(sizeof(unsigned)+sizeof(Pixel)) + changed*sizeof(unsigned) + deltas*sizeof(int)`.
- **`enforceBudget()`** after every record/merge: `while used > budget`: if undo stack non-empty -> take `bottom()`, `removeBottom()`, delete record, `used -= mem`, `discarded++` (**oldest first**); else if redo stack non-empty -> same on the redo stack bottom (farthest-future entry); else break.
- **Oversized record rule:** if a single new record's `memBytes > budget`, do **not** store it: delete it, **clear both stacks** (older records are invalid because the image changed without a record), print `[WARN] <Op> cannot be undone within the memory budget; history cleared`, count it. (Mention in report: an undo of an older record is only valid if the image is in exactly the state right after it.)
- Test with a small budget (e.g. 5 MB, or 200 KB in unit tests).

#### 6.8.4 Phase 2 compact records [P2] — what each stores and how reverse works
| Op | Stored | Undo (reverse) | Redo |
|---|---|---|---|
| Rotate | angle | rotate by `360-angle` (90<->270, 180) | rotate by angle |
| Flip | direction | flip again | flip again |
| Brightness | `net` delta, `deltas[]` (the individual deltas applied, for exact redo), `clip[]` = sorted list of `(idx, origPixel)` for pixels where any channel was clipped | for every pixel subtract `net` from each channel (int math, store clamped), **then** overwrite every `clip[]` pixel with its `orig` | apply each stored delta in order with clamping (if all deltas have the same sign, apply `net` once) |
| Crop | crop rect, `prevW, prevH`, `saved[]` = all pixels **outside** the rect in raster order | allocate `prevW x prevH`; fill rect area from the current image; fill the rest from `saved[]` in raster order | crop again with same rect |
| Bucket fill | `origColor`, `changedIdx[]` (unsigned), replacement colour | set every `changedIdx` pixel to `origColor` | set every `changedIdx` pixel to replacement colour (no re-traversal) |
| Grayscale | `snapshot` = previous pixels (whole image = affected region) | restore snapshot | grayscale again |
| Blur | snapshot | restore | blur again (deterministic) |
| Resize | snapshot (also `prevW,prevH`) | restore snapshot (image takes snapshot's size) | resize again |

`regionX/Y/W/H` = affected region (crop: the rect; fill: bounding box of changed pixels; others: whole image).

#### 6.8.5 Coalescing [P2] (DECISION D11)
- Only **adjacent** records: the new op is merged only into the record currently on top of the Undo stack, and only if both are `OP_BRIGHTNESS` and coalescing is ON (`setCoalescing(bool)` for experiments).
- Example required: `+1,+1,+1,+1` -> 1 record `+4` (`coalescedCount=4`); `+2, Crop, +3` -> 3 records.
- **Correctness with clipping** (report this): `clamp(clamp(v+a)+b) != clamp(v+a+b)` (e.g. +200 then -100 vs +100), so the merged record never "re-derives" the result from the net delta alone. Instead:
  1. The new brightness op is **actually applied to the image** normally (with clamping), producing its own clip list `(idx, preValue)` (preValue = pixel before THIS op).
  2. Merge into the top record `R` (net so far = `S`): walk `R.clip` and `new.clip` (both sorted by idx) with two pointers and build a merged list: keep all of `R.clip` as is; for every `new.clip` entry whose idx is **not** in `R.clip`, add `(idx, preValue - S)` per channel. Why: that pixel was never clipped before this op, so its pre-op value is exactly `orig + S` on all channels, hence `orig = preValue - S` (always within 0..255).
  3. `R.net += delta; R.deltas.push_back(delta); R.coalescedCount++; R.seqNo = new seq; recompute memBytes; adjust used; enforceBudget()`.
  4. Undo (one step reverses all merged ops): subtract `R.net` from every pixel, then restore `clip` pixels from `orig`. Pixels never clipped at any step are exact (`v_final = v_orig + net`), clipped ones come from the list.
  5. Redo re-applies deltas in order with clamping -> identical final image.
- Optional: coalesce two Rotates / two Flips (not required; skip unless time allows).

### 6.9 Jobs and Pipeline

```
enum JobType { J_RESIZE, J_BLUR, J_FILL, J_SAVE, J_EXPORT, J_UNDO, J_REDO };
struct Job {
  int id; JobType type; int priority;      // priority: Undo/Redo 1, Blur 2, Fill 2, Resize 3, Save 4, Export 4 (set in BOTH modes)
  long long seq;                           // arrival order, global counter
  int epoch;                               // finalization epoch (see 6.9.3)  [P2 logic; field present in P1 harmlessly]
  double submitMs, startMs, endMs;         // steady clock, relative to program start
  double estCost, measuredMs;              // est. = pixel-operation estimate (see 6.9.4)
  bool cancelled;
  int a,b,c,d;                             // resize nw,nh | blur radius | fill x,y
  Pixel color; bool useStack;              // fill
  std::string filename;                    // export path
  bool operator<(const Job& o) const;      // [P2] "served before": (epoch, priority, seq)
};
```
Display name via `jobName(job)` -> "Resize 800x600", "Blur r=3", "Bucket fill (1,1) stack", "Save project", "Export out.png", "Undo", "Redo".

#### 6.9.1 Pipeline [P1]
`Pipeline` owns `queueType<Job> fifo` (bounded, `maxCapacity` configurable, default 20, menu option to change when queue empty).
- `bool submit(Job&)`: assigns `id, seq, submitMs`; if `fifo.size() >= maxCapacity` -> **reject**: print `[WARN] Queue full (N/N). Process or clear pending jobs first.` (DECISION D4). Cancelled jobs still occupy a slot until they reach the front.
- `listPending()` -> prints position, id, name, status (PENDING / CANCELLED), using `itemAt(i)` (does not remove).
- `bool cancel(int id)`: find with `itemAt` loop; if found and not cancelled -> `cancelled = true` (**no shifting**); else message.
- `clearAll()`: loop `deleteQueue()` until empty.
- `processNext(ImageLab&)`: while front exists and is cancelled -> `deleteQueue()` (skipped++); then if a job exists: record `startMs`, execute via engine, record `endMs`, `measuredMs`, update waiting-time sums, `deleteQueue()`. Return status.
- `processAll()` repeats `processNext` until empty.
- `pendingCount()` (all entries incl. cancelled), `activeCount()` (not cancelled).
- Stats: `completed`, `cancelledSkipped`, `totalWaitMs`, `totalTurnaroundMs` -> `avgWaitMs()`.
- **Undo/Redo requests:** engine method `requestUndo()`: if `pipeline.activeCount()==0` run immediately; else `submit(Job{J_UNDO})` (DECISION: a request to the queue counts toward capacity and can be rejected the same way).
- **Instant ops** (crop, rotate, flip, brightness, grayscale) always run immediately, even if jobs are pending (DECISION D5; they act on the current image).
- **Queued job parameters are validated when executed** (the image may have changed since submission); an invalid job prints `[ERROR] job #id failed: ...`, counts as processed-failed, records no history (DECISION D6).

#### 6.9.2 Scheduling modes [P2]
`enum SchedMode { MODE_FIFO, MODE_PRIORITY }`. `Pipeline` holds both `queueType<Job> fifo` and `priorityQueueType<Job> pq`; only the active one is used. Mode may be switched **only when no jobs are pending** (DECISION D13), otherwise message. Capacity limit applies identically in both modes.

#### 6.9.3 Priority + finalization rule via epochs [P2] (DECISION D12)
Pipeline keeps `int currentEpoch=0`. On submit: `job.epoch = currentEpoch;` and **if the job is Save or Export, then `currentEpoch++` afterwards**. `Job::operator<` ("served earlier than") compares `epoch`, then `priority` (smaller = earlier), then `seq`. The ordered priority queue therefore yields: all jobs before a barrier first (by priority, then arrival), then the barrier itself (priority 4 sorts last within its epoch), then the next epoch. Check against the assignment example:
`Resize(3) Blur(2) Export(4) Blur(2) Undo(1)` -> epochs 0,0,0,1,1 -> order `Blur, Resize, Export, Undo, Blur`. ✔
Cancelled jobs are skipped at the front exactly as in FIFO mode.

#### 6.9.4 Costs and waiting time [P2] (DECISION D14)
- `estCost` (units of "pixel operations"): resize `nw*nh*4`; blur `W*H*2*(2r+1)`; fill `W*H`; save/export `W*H`; undo/redo `W*H/4`. Set at submission from the current image size.
- Measured: `startMs`, `endMs` from `Timer::nowMs()`. **Waiting time = startMs - submitMs. Completion (turnaround) time = endMs - submitMs.** Report states "measured with steady_clock". Additionally provide a deterministic **simulation** using `estCost` (no execution) so results are reproducible; show both tables.

### 6.10 Engine (`ImageLab` class) [P1]
Owns `Image current`, `HistoryManager history`, `Pipeline pipeline`, counters. Public methods (one per menu action): `load(path)`, `cropOp(x,y,w,h)`, `rotateOp(angle)`, `flipOp(horizontal)`, `brightnessOp(delta)`, `grayscaleOp()`, `submitResize(w,h)`, `submitBlur(r)`, `submitFill(x,y,color,useStack)`, `submitSave()`, `submitExport(name)`, `requestUndo()`, `requestRedo()`, `executeJob(Job&)` (switch on type; used by pipeline), `saveSnapshot(name)` for demo, and accessors for the Inspector.
- Save project writes `imagelab_project.png`; Export writes the user-chosen name (extension decides format) (DECISION D16).
- Operations on an empty (not loaded) engine -> `[ERROR] load an image first`.

### 6.11 Data Structure Inspector [P1 + P2 additions]
Menu option prints (plain text):
```
===== DATA STRUCTURE INSPECTOR =====
Undo stack : size=3   [top] Blur | Crop | Rotate [bottom]
Redo stack : size=0   (empty)
Undo stack ops: push=3 pop=0 | Redo stack ops: push=0 pop=0 | removeBottom=0   (totals: push=3 pop=0)
Pending jobs (mode FIFO/Priority, capacity 20): size=2, max recorded=4
   1. #7 Resize 800x600   PENDING
   2. #8 Blur r=2         CANCELLED
Queue ops: addQueue=9 deleteQueue=7
Last bucket fill: structure=queue insertions=12345 maxSize=412 region=12345
-- Phase 2 --
History memory: 3.2 MB used / 5.0 MB budget | records discarded: 17 | coalesced ops: 40
Average queue waiting time: 12.4 ms (n=9 jobs)
```
Must list: undo/redo sizes, queue current + max size, push/pop counts, addQueue/deleteQueue counts, contents (operation names) of undo, redo and pending jobs; [P2] history bytes + budget, average waiting time.

### 6.12 Console menu [P1; P2 adds items] — fixed numbering (demo scripts depend on it)
```
=== ImageLab ===
 1  Load image
 -- Instant edits --
 2  Crop      3  Rotate      4  Flip      5  Brightness      6  Grayscale
 -- Queued jobs (go to pipeline) --
 7  Resize    8  Blur        9  Bucket fill (asks stack/queue)   10 Save project   11 Export
 -- Pipeline --
12  List pending jobs        13 Process next job      14 Process all jobs
15  Cancel a job (by id)     16 Clear all jobs        17 Set queue capacity
18  Scheduling mode (FIFO/Priority) [P2]
 -- History --
19  Undo      20  Redo
 -- Tools --
21  Data Structure Inspector 22  Show image info
23  Run experiments [P2]     24  Set history memory budget (MB/KB) [P2]   25  Toggle coalescing [P2]
 0  Exit
```
All input read with `cin >>` and **validated** (non-numeric input clears the stream and re-prompts; never loops forever on EOF — on EOF exit cleanly).

### 6.13 `priorityQueueType<Type>` [P2] (DECISION D12)
**Ordered singly linked list**, head = next to serve. `Type` must define `bool operator<(const Type&) const` meaning "served before". `int` works too (smaller first).
- `addQueue(item)`: insert **before the first node `n` where `item < n.info`**; therefore equal-priority items stay behind earlier ones (stable, FIFO among ties). Optional `tail` pointer: if `!(item < tail->info)` append in O(1).
- `front()` (empty -> error, `Type()`), `deleteQueue()` (empty -> error), `size()`, `isEmptyQueue()`, `isFullQueue()` (always false), `initializeQueue()`, `Type* itemAt(int i)` (service order, O(i)), stats (`adds, deletes, maxCount`), copy ctor/assignment/dtor (deep).
- Complexity: addQueue O(n) worst (O(1) best with tail), front/deleteQueue O(1), space O(n).

### 6.14 Batch operations [P2] (DECISION D15)
| Function | Behaviour | Why faster than n singles |
|---|---|---|
| `stack::pushBatch(const Type items[], int n)` | `n<=0` -> return. One capacity check; if growable and not enough room: **one** compaction/reallocation to `max(2*cap, count+n)`; if not growable: push only as many as fit + warning. Then one tight copy loop; `pushes += pushed`, one `maxCount` update | 1 check + 1 resize instead of n checks and ~log n reallocations |
| `stack::popBatch(int n)` | `n<=0` return; `n>count` -> warn, `n=count`; `count -= n; pops += n` | **O(1)** instead of O(n) |
| `queue::addQueueBatch(items,n)` | one free-space check; growable -> grow once; fixed -> add only `free` items + warning; copy into **at most two contiguous segments** (tail..end, then 0..) | 1 check, <=2 block loops |
| `queue::deleteQueueBatch(int n)` | clamp `n` to `count`; `head=(head+n)%cap; count-=n; deletes+=n` | **O(1)** |
Behaviour must be equivalent to n single operations (including ordering, wrap-around). Tests in 7.2.

### 6.15 Experiments [P2] (menu 23; writes CSV to `results_*.csv` and prints tables)
Repeat each timing **3 times**, report average (and print min/max). Use `-O2`. Prevent dead-code elimination (accumulate a checksum and print it).
| ID | Workload | Metrics |
|---|---|---|
| E1 fill | uniform-colour images 512, 1024, 2048 (4096 behind a flag), fill from (0,0), plus a "maze/stripes" image; stack vs queue | ms, insertions, maxSize, changed; visit order for a 16x16 demo printed + `out_visit_stack.png` / `out_visit_queue.png` (grey level = visit index) for 256x256 |
| E2 history | 100 and 1000 random ops (seeded; brightness mostly, plus rotate/flip/gray/blur(1)/resize between 256 and 320/fill) on a 256x256 image, budget fixed (e.g. 5 MB) | used bytes, records discarded, undo time (undo-all), record count with coalescing ON vs OFF; also what full-copy history would have cost |
| E3 scheduling | workload of >= 30 jobs (e.g. 36) with all types, incl. >= 2 barriers; same workload under FIFO and Priority | avg waiting, avg completion, avg waiting of priority-1 jobs, jobs processed (measured + estimated tables) |
| E4 batch | 1,000 / 10,000 / 100,000 items, stack push/pop and queue add/delete, single vs batch | ms, speed-up |
Workload design for E3: image 512x512 (generated pattern); fill coordinates (1,1); resizes between 384 and 640; seed history with >= 8 non-brightness ops first so Undo jobs find records.

---

## 7. TEST PLAN (test program prints `PASS: <name>` / `FAIL: <name>`, summary, non-zero exit on any fail)

Framework: `tests/test_framework.h` with `CHECK(cond, "name")`-style function/macro and counters. No std containers in tests either. Oracle for fill tests = **iterative** reachability by repeated sweeps (no recursion).

### 7.1 Phase 1 cases [P1]
- **Stack:** pop/top on empty (no crash, message); 100,000 pushes then verify LIFO; repeated push/pop interleaved; `initializeStack` then reuse; copy ctor and `operator=` independent (modify copy, original unchanged); self-assignment; `isFullStack` with `setGrowable(false)`; growth beyond initial capacity; `removeBottom` + push pattern (compaction path); `peekAt`.
- **Queue:** `deleteQueue/front/back` on empty; fill to capacity -> `isFullQueue` true, extra `addQueue` rejected without crash; wrap-around (fill, delete half, add more, check order); 100,000 add/delete cycles; growable queue grows **mid-wrap** and keeps order; copy ctor/assignment; `size/capacity`.
- **Image ops:** crop valid/invalid/border; rotate 90x4 = identity, 90 then 270 = identity, 180 twice; non-square rotation dims; flip twice = identity; brightness clipping at 0 and 255; grayscale r=g=b; resize to same size = identity (or nearly) and dims correct; blur on 1x1, 1xN, uniform image unchanged.
- **Undo/Redo:** several undos restore each earlier image exactly; several redos; new action after undo clears redo; the `Rotate, Crop, Blur` example stack contents; undo/redo on empty -> no crash.
- **Pipeline:** FIFO order; list does not remove; cancel (skipped when it reaches the front, queue not shifted); clear; full-queue rejection; undo while jobs pending is queued; history recorded at execution time, not submission.
- **Bucket fill:** single-pixel region (checkerboard); whole image; irregular region; region touching border; several separate regions of same colour (only one changes); diagonal pixels not connected; same colour replacement terminates; out-of-bounds start rejected; 1x1 and 1xN images; stack and queue results **identical** on every case; 1024x1024 uniform fill without crash.

### 7.2 Phase 2 cases [P2]
- **Priority queue:** same priority -> arrival order; mixed; 1,000-job workload sorted correctly; ties; **finalization rule** incl. the assignment example `Resize(3) Blur(2) Export(4) Blur(2) Undo(1)` -> `Blur, Resize, Export, Undo, Blur`; multiple barriers; cancelled job skipped; copy ctor/assignment.
- **Memory-aware history:** budget reached; oldest discarded first; undo after cleanup still correct for remaining records; oversized record clears history with warning; **undo of each op type restores the exact previous image** (compare to saved copy): rotate(90/180/270), flip(h/v), brightness (with heavy clipping), crop, fill, grayscale, blur, resize.
- **Coalescing:** `+1 x4` -> 1 record `+4`; `+2, crop, +3` -> 3 records; undo/redo of a combined record; **clipping cases** `+200,-100`, `-200,+100`, `+100,+100,-250`; randomized property test (50 random sequences): final image == naive sequential application, undo == original, redo == final.
- **Batch:** batch larger than free space (growable and fixed); batch across wrap-around; `popBatch/deleteQueueBatch` with n > size; n = 0 and negative; result equals n single ops.

---

## 8. Test images (`tests/make_test_images.cpp` -> `images/`)
Generate (using `saveImage` only): `checker_16.png` (16x16), `regions_32.png` (several disjoint same-colour blobs + a ring), `border_32.png` (region touching all borders), `gradient_64.png`, `noise_128.png` (LCG noise), `bars_256.png`. Keep all small (< 100 KB total). The user may add one real photo (< 1 MB).

---

## 9. Reports (PDF in `report/`; source `report.md` -> export via VS Code Markdown PDF / Word / Google Docs)

### 9.1 Phase 1 report (**max 3 pages**)
1. Representations & justification: stack (growable array + base index), queue (circular, count variable). Why not linked list.
2. Complexity table: each stack and queue operation with a one-line reason.
3. Policies: empty/full handling (message + safe default, never crash), pipeline full-queue policy (reject + message, cancelled jobs hold a slot until they reach the front), instant-vs-queued decision, history in P1 = full snapshot.
4. Test results: paste summary of the PASS/FAIL output and the list of cases.

### 9.2 Phase 2 report (**max 10 pages**), sections exactly as assignment 7.8
1. History record design + cleanup policy (oldest first, oversized-record rule, justification).
2. Coalescing rules + correctness under clipping (the +200/-100 argument and the merge-with-`S` proof).
3. Scheduler: ordered linked-list priority queue, tie handling (stable insertion), finalization rule via epochs (with the assignment example).
4. Experimental results (E1-E4) with tables/graphs and 3-5 sentences of discussion each. Say whether time is measured or estimated. Graphs can be made in Excel/Sheets/Python from the CSVs (not part of the C++ code).
5. Complexity analysis (table, with the "why" for each of the 8 rows in assignment 7.6).
6. Testing evidence.
7. Known limitations (e.g. brightness clip list can exceed a full copy on saturated images; instant ops bypass the queue; mode switch only when idle; bilinear resize rounding).

---

## 10. Demo script (5-8 minutes) and viva cheat sheet

**Demo order:** load `images/gradient_64.png` -> instant ops (rotate, flip, brightness +50 x3) -> Inspector -> undo x2, redo x1 -> new op (shows redo cleared) -> submit Resize, Blur, Fill(stack), Fill(queue), Export -> list -> cancel one -> process next/all -> show outputs -> try to overflow queue (capacity 3) -> [P2] set budget 200 KB, do 30 ops, Inspector shows discards -> [P2] switch to priority mode, submit mixed workload, show order -> run `run_tests`.

**Likely viva questions (the student must be able to answer):**
1. Why circular queue? How do you tell full from empty? (count variable)
2. What happens on pop of empty stack? (message, no crash; why not assert)
3. Complexity of push/pop/removeBottom and why? (amortised O(1); base index)
4. How does your queue grow while wrapped? (copy in logical order from head)
5. Why can't bucket fill use recursion? How do you avoid infinite loops? (own stack; recolour-on-insert; same-colour early exit)
6. Stack fill vs queue fill: visit order, max size, same output? Why? (DFS-like vs BFS layers)
7. How is Undo/Redo implemented? Why does a new op clear Redo?
8. Why record at execution time for queued jobs?
9. How does cancel work without shifting? 
10. Why store a pointer in the history stacks? Who deletes records?
11. What does each op store in Phase 2? Why can't grayscale be reversed from parameters?
12. Why does brightness clipping break the naive reverse? How do you handle it? How does coalescing stay correct?
13. What happens when one record exceeds the budget? Why clear the whole history?
14. How does the priority queue keep ties in arrival order? Complexity?
15. How does the finalization rule work? (epochs)
16. What do the batch operations save compared to n singles? (checks, reallocations, O(1) pop)
17. Which numbers in your experiments surprised you and why?

---

## 11. Pre-submission checklist (run for EACH phase)
1. `src/check_rules.sh` prints nothing; manual recursion review done.
2. Clean build from a **fresh copy** using only README steps; `run_tests` prints PASS for every required case and no FAIL.
3. Inspector shows every metric required for that phase.
4. Folder name `<ROLL>_<SEC>_A2_P<n>` and structure exactly as Section 4; no binaries, no `.git`, no stray files.
5. `report/report.pdf` exists, within page limit (P1 <= 3, P2 <= 10).
6. README.txt: build/run, libraries (stb only for file I/O), Decisions log (Section 12), menu guide.
7. Zip it, **extract the zip into a new empty folder, rebuild and run tests from there** (GCR post explicitly tells you to verify), then upload. Check the uploaded file name on GCR.
8. Submit **P1 as soon as it is ready** (same deadline as P2 but bank the marks early).

---

## 12. Decisions log (copy into README "Assumptions" and the reports)
- **D1** Stack = growable array + base index; `isFullStack()` false while growable. **D2** Queue = circular array, count variable, fixed by default, growable on demand.
- **D3** Empty/full cases: print message, no-op or return `Type()`; never crash.
- **D4** Full pipeline queue: reject new job with message; cancelled jobs occupy a slot until they reach the front.
- **D5** Instant ops run immediately even if jobs are pending. **D6** Queued job parameters validated at execution; failures are reported and not recorded.
- **D7** Fill: recolour on insert, 4-connectivity, neighbour order U,D,L,R; identical results for stack and queue.
- **D8** History records stored by pointer, owned by HistoryManager. **D9** Oversized record -> clear history, warn. **D10** Cleanup oldest-first from Undo bottom, then Redo bottom, via `removeBottom()`.
- **D11** Brightness coalescing (any signs) with exact clip-list merge. **D12** Priority queue = ordered linked list, ties by arrival, barrier via epochs.
- **D13** Scheduling mode switch only when idle. **D14** Waiting time measured (steady_clock) + deterministic simulation with estimated cost.
- **D15** Batch ops equivalent to n singles; clamp n; fixed-capacity structures add only what fits.
- **D16** Save -> `imagelab_project.png`; Export extension chooses format. **D17** stb used only in `image_io.cpp` for file I/O. **D18** Grayscale integer BT.601 weights; box blur radius r; bilinear resize.
- **D19** Undo/Redo on empty stack = reported no-op (also as a job). **D20** Experiment results go to `results_*.csv` in the working directory.

---

## 13. Common pitfalls (check each)
- Rotation formulas wrong or dims not swapped for 90/270; inverse of 90 is 270.
- Crop validation overflow (`x+w` > int) / off-by-one at the border.
- Bucket fill: infinite loop when new colour == old colour; reading neighbours out of bounds; recursion sneaking in.
- Queue growth while wrapped (copy from `head`, not from index 0).
- Stack `compact` vs `grow` logic (base >= cap/2 rule); stale `base` after `initializeStack()`.
- Copy constructor shallow copies (double free). Self-assignment.
- History: leaking records on clear/destroy; deleting a record still referenced by a stack; forgetting to clear redo on new op; recording failed ops.
- Undo of Crop/Resize must restore dimensions too.
- Brightness `unsigned char` overflow (use int math).
- stb: always ask for 3 channels; `sizeof(Pixel)==3` assumption checked.
- Windows: no `%zu`-style printf (we use iostream); `\` vs `/` in paths (use `/`).
- Timing loops optimised away by `-O2` -> keep a checksum.
- Uninitialised `Job`/record fields -> initialise everything in constructors.
- Menu input: non-numeric input must not cause an infinite loop.

---

## 14. Time plan (today = Mon 12 Oct 2026; deadline Fri 16 Oct 11:59 PM)
| When | Goal |
|---|---|
| Mon 12 (tonight) | Prompts 0-2: scaffold, stack/queue + tests, Image + ops |
| Tue 13 | Prompts 3-6: fill, history, pipeline, inspector/menu. Then Prompt 7 -> **submit P1** (target: Tue night / Wed morning) |
| Wed 14 | Prompts P2-0..P2-4: priority queue, batch, compact history, coalescing |
| Thu 15 | Prompts P2-5..P2-7: scheduler, inspector/menu, experiments; start report |
| Fri 16 (morning) | P2-8: audit, report PDF, README, zip + extract-and-rebuild check, **submit by ~6 PM** (leave buffer) |
Bonus (only if everything is done): adaptive queue capacity is already free via the growable queue; a "detailed performance dashboard" is just a richer Inspector.
