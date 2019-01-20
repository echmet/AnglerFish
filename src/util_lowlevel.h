#ifndef UTIL_LOWLEVEL_H
#define UTIL_LOWLEVEL_H

#include <cassert>
#include <cstdio>
#include <cstdlib>

[[noreturn]] inline
void __IMPOSSIBLE_PATH() {
#ifdef NDEBUG
    fputs("Impossible execution path", stderr);
    abort();
#else
    assert(false);
#endif // NDEBUG
}
#define IMPOSSIBLE_PATH __IMPOSSIBLE_PATH()

#endif // UTIL_LOWLEVEL_H
