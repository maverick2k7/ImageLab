// dynArray.h — ImageLab growable array (doubling strategy)
// Template definitions live entirely in this header (no .cpp needed).
// Allowed headers: iostream only (for error messages).
// No std containers. No recursion. No forbidden keywords.

#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// dynArray<T>
// Growable, heap-backed array that doubles capacity when full.
// Rule of Three: copy constructor, copy assignment, destructor.
// Never crashes on bad index: returns a reference to _dummy instead.
// ---------------------------------------------------------------------------
template<class T>
class dynArray {
public:
    // ----- Construction / destruction --------------------------------------

    // Construct with an optional initial capacity (clamped to at least 1)
    explicit dynArray(int cap = 16)
        : _data(0), _size(0), _cap(0) {
        int c = (cap < 1) ? 1 : cap;
        _data = new T[c];
        _cap  = c;
        // _dummy stays value-initialised via its own default constructor
    }

    // Copy constructor — deep copy
    dynArray(const dynArray<T>& other)
        : _data(0), _size(0), _cap(0) {
        _copyFrom(other);
    }

    // Destructor
    ~dynArray() {
        delete[] _data;
        _data = 0;
    }

    // Copy assignment — deep copy, self-assignment safe
    dynArray<T>& operator=(const dynArray<T>& other) {
        if (this == &other) {
            return *this;   // self-assignment: nothing to do
        }
        delete[] _data;
        _data = 0;
        _size = 0;
        _cap  = 0;
        _copyFrom(other);
        return *this;
    }

    // ----- Core operations -------------------------------------------------

    // Append one element; grows (doubles) if needed
    void push_back(const T& val) {
        if (_size == _cap) {
            _grow(_cap * 2);   // double capacity
        }
        _data[_size] = val;
        _size++;
    }

    // Indexed access — non-const version.
    // Out-of-range: prints an error and returns _dummy (never crashes).
    T& operator[](int idx) {
        if (idx < 0 || idx >= _size) {
            std::cerr << "[ERROR] dynArray: index " << idx
                      << " out of range (size=" << _size << ")\n";
            return _dummy;
        }
        return _data[idx];
    }

    // Indexed access — const version.
    const T& operator[](int idx) const {
        if (idx < 0 || idx >= _size) {
            std::cerr << "[ERROR] dynArray: index " << idx
                      << " out of range (size=" << _size << ")\n";
            return _dummy;
        }
        return _data[idx];
    }

    // Number of stored elements
    int size() const { return _size; }

    // Reset to zero elements (keeps allocated memory)
    void clear() { _size = 0; }

    // Ensure capacity is at least newCap (no-op if already large enough)
    void reserve(int newCap) {
        if (newCap > _cap) {
            _grow(newCap);
        }
    }

    // Raw pointer to underlying array (nullptr if empty)
    T* data() { return _data; }
    const T* data() const { return _data; }

private:
    T*  _data;      // heap buffer
    int _size;      // number of live elements
    int _cap;       // allocated capacity
    T   _dummy;     // returned on bad-index access; value-initialised

    // Internal: grow to exactly newCap slots, copying existing elements.
    // Callers guarantee newCap > _cap.
    void _grow(int newCap) {
        if (newCap < 1) {
            newCap = 1;
        }
        T* fresh = new T[newCap];
        // Manual copy loop (no memcpy / std::copy allowed)
        for (int i = 0; i < _size; i++) {
            fresh[i] = _data[i];
        }
        delete[] _data;
        _data = fresh;
        _cap  = newCap;
    }

    // Internal: copy all fields from another dynArray (used by ctor and =)
    void _copyFrom(const dynArray<T>& other) {
        int c = (other._cap < 1) ? 1 : other._cap;
        _data = new T[c];
        _cap  = c;
        _size = other._size;
        for (int i = 0; i < _size; i++) {
            _data[i] = other._data[i];
        }
    }
};

#endif // DYNARRAY_H
