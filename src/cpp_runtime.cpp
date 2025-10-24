//
//  allocator.cpp
//  toybox
//
//  Created by Fredrik on 2024-02-01.
//

#include "cincludes.hpp"
#include "memory.hpp"

extern "C" {
    FILE *log_file() {
        static FILE *log = nullptr;
        if (log == nullptr) {
            log = fopen("log.txt", "w+");
        }
        return log;
    }
}
 
void *operator new (size_t n) {
    return _malloc(n);
}
void* operator new[] (size_t n) {
    return _malloc(n);
}

#if __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-exception-spec-mismatch"
#endif
void operator delete (void* p) {
    _free(p);
}
void operator delete (void* p, size_t) { // ≥C++14
    _free(p);
}
void operator delete[] (void* p) {
    _free(p);
}
void operator delete[] (void* p, size_t) { // ≥C++14
    _free(p);
}
#if __clang__
#pragma GCC diagnostic pop
#endif

#ifdef __M68000__

extern "C" void __cxa_pure_virtual() {
    hard_assert(0);
}

// WARNING: These init guards are NOT threadsafe, never init statics on timer callbacks.
#define GUARD_DONE      0x01
#define GUARD_PENDING   0x02
using __guard = uint8_t;
extern "C" int __cxa_guard_acquire(__guard* g) {
    if (*g & GUARD_DONE) {
        return 0;
    }
    *g = GUARD_PENDING;
    return 1;
}
extern "C" void __cxa_guard_release(__guard* g) {
    *g = GUARD_DONE;
}
extern "C" void __cxa_guard_abort(__guard* g) {
    *g = 0;
}

#endif

#ifndef TOYBOX_HOST
void* operator new (size_t count, void *p) {
    return p;
}
void* operator new[] (size_t count, void *p) {
    return p;
}
#endif
