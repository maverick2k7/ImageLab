// utils.h — ImageLab utility helpers
// Allowed headers: iostream, fstream, string, cmath, cstdlib, chrono, ctime
// No standard containers. No recursion. No forbidden keywords.

#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <chrono>

// ---------------------------------------------------------------------------
// minVal / maxVal — template helpers (own implementations; no std containers)
// ---------------------------------------------------------------------------
template<class T>
T minVal(T a, T b) {
    // Return the smaller of the two values
    return (a < b) ? a : b;
}

template<class T>
T maxVal(T a, T b) {
    // Return the larger of the two values
    return (a > b) ? a : b;
}

// ---------------------------------------------------------------------------
// clampInt — keep v inside [lo, hi]
// Using int throughout so callers avoid unsigned-char arithmetic pitfalls.
// ---------------------------------------------------------------------------
inline int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

// ---------------------------------------------------------------------------
// swapVals — template three-step swap (no extra headers)
// ---------------------------------------------------------------------------
template<class T>
void swapVals(T& a, T& b) {
    // Three-step swap using a temporary; safe for any assignable type
    T tmp = a;
    a = b;
    b = tmp;
}

// ---------------------------------------------------------------------------
// Timer — wraps steady_clock; returns milliseconds as double
// ---------------------------------------------------------------------------
class Timer {
public:
    Timer() : _running(false), _startMs(0.0) {}

    // Start (or restart) the timer
    void start() {
        _startMs  = nowMs();
        _running  = true;
    }

    // Elapsed milliseconds since start().  Returns 0 if never started.
    double elapsedMs() const {
        if (!_running) {
            return 0.0;
        }
        return nowMs() - _startMs;
    }

    // Static helper: absolute time in ms from the steady_clock epoch
    static double nowMs() {
        // Use steady_clock (monotonic); convert nanoseconds -> milliseconds
        long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                           std::chrono::steady_clock::now().time_since_epoch())
                           .count();
        return static_cast<double>(ns) / 1000000.0;
    }

private:
    bool   _running;
    double _startMs;
};

// ---------------------------------------------------------------------------
// Rng — simple 32-bit LCG (Knuth/Numerical Recipes constants)
// Reproducible: always seed with a constant in tests.
// ---------------------------------------------------------------------------
class Rng {
public:
    // Seed the generator.  Pass any constant for reproducible sequences.
    explicit Rng(unsigned int seed) : _state(seed) {}

    // Return next pseudo-random unsigned int (full 32-bit range)
    unsigned int next() {
        // LCG: state = state * 1664525 + 1013904223  (mod 2^32 implicit)
        _state = _state * 1664525u + 1013904223u;
        return _state;
    }

    // Return a value in [lo, hi] (inclusive).
    // lo and hi must satisfy lo <= hi; caller's responsibility.
    int range(int lo, int hi) {
        if (lo >= hi) {
            return lo;   // degenerate range; safe fallback
        }
        // Cast to unsigned to avoid signed-overflow UB, then map to [0, span)
        unsigned int span   = static_cast<unsigned int>(hi - lo + 1);
        unsigned int sample = next() % span;
        return lo + static_cast<int>(sample);
    }

private:
    unsigned int _state;
};

// ---------------------------------------------------------------------------
// Logging helpers — write prefixed messages to cerr / cout
// Plain ASCII only (Windows console safe).
// ---------------------------------------------------------------------------
inline void logError(const std::string& msg) {
    std::cerr << "[ERROR] " << msg << "\n";
}

inline void logWarn(const std::string& msg) {
    std::cerr << "[WARN] " << msg << "\n";
}

inline void logInfo(const std::string& msg) {
    std::cout << "[INFO] " << msg << "\n";
}

#endif // UTILS_H
