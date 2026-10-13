#!/usr/bin/env bash
# check_rules.sh — run before every commit and before zipping.
# Searches src/ and tests/ but EXCLUDES src/third_party/.
# All three grep commands must produce no output for "RULES OK" to print.
# Run from the project root: bash src/check_rules.sh

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC="$REPO_ROOT/src"
TESTS="$REPO_ROOT/tests"

FAIL=0

echo "=== ImageLab rule check ==="

# ---- Rule 1: forbidden #include <...> headers ----------------------------
echo "Checking forbidden headers..."
RESULT=$(grep -rnE \
  "#include *<(vector|list|deque|stack|queue|map|set|array|unordered_map|unordered_set|algorithm|utility|functional|memory|cstring|cstdio|iomanip|sstream|limits|cstdint|cassert|new|thread|mutex|atomic|condition_variable|future|exception|stdexcept)>" \
  --include="*.h" --include="*.cpp" \
  --exclude-dir="third_party" \
  "$SRC" "$TESTS" \
  2>/dev/null || true)

if [ -n "$RESULT" ]; then
    echo "[FAIL] Forbidden headers found:"
    echo "$RESULT"
    FAIL=1
else
    echo "  headers: OK"
fi

# ---- Rule 2: forbidden std:: identifiers ---------------------------------
echo "Checking forbidden std:: identifiers..."
RESULT=$(grep -rnE \
  "std::(vector|list|deque|stack|queue|priority_queue|map|set|array|min|max|swap|sort|memcpy|unique_ptr|shared_ptr|thread|mutex)" \
  --include="*.h" --include="*.cpp" \
  --exclude-dir="third_party" \
  "$SRC" "$TESTS" \
  2>/dev/null || true)

if [ -n "$RESULT" ]; then
    echo "[FAIL] Forbidden std:: identifiers found:"
    echo "$RESULT"
    FAIL=1
else
    echo "  std:: identifiers: OK"
fi

# ---- Rule 3: forbidden keywords ------------------------------------------
echo "Checking forbidden keywords (assert/exit/abort/throw)..."
RESULT=$(grep -rnE \
  "\b(assert|exit|abort|throw)\b" \
  --include="*.h" --include="*.cpp" \
  --exclude-dir="third_party" \
  "$SRC" "$TESTS" \
  2>/dev/null || true)

if [ -n "$RESULT" ]; then
    echo "[FAIL] Forbidden keywords found:"
    echo "$RESULT"
    FAIL=1
else
    echo "  keywords: OK"
fi

# ---- Summary -------------------------------------------------------------
if [ "$FAIL" -eq 0 ]; then
    echo ""
    echo "RULES OK"
else
    echo ""
    echo "RULES FAILED -- fix the items above before committing."
    exit 1
fi
