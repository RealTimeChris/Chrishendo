// The code in this header was sampled from the SMHasher test suite: https://github.com/aappleby/smhasher
//-----------------------------------------------------------------------------
// Platform-specific functions and macros
#pragma once

#include <chrono>
#include <chrishendo/Index.hpp>
#include <thread>
#include <fstream>
#include <iostream>
#include <chrono>

void setAffinity(uint32_t cpu);

#if defined(CHRISHENDO_MSVC)

	#define ROTL32(x, y) _rotl(x, y)
	#define ROTL64(x, y) _rotl64(x, y)
	#define ROTR32(x, y) _rotr(x, y)
	#define ROTR64(x, y) _rotr64(x, y)

	#if !defined(NOMINMAX)
		#define NOMINMAX
	#endif

	#pragma warning(disable : 4127)
	#pragma warning(disable : 4100)
	#pragma warning(disable : 4702)

	#define BIG_CONSTANT(x) (x)

	#define FORCE_INLINE __forceinline
	#define NEVER_INLINE __declspec(noinline)

	#define rdtsc() __rdtsc()

#else

	#include <stdint.h>

	#define FORCE_INLINE inline __attribute__((always_inline))
	#define NEVER_INLINE __attribute__((noinline);

inline uint32_t rotl32(uint32_t x, int8_t r) {
	return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64(uint64_t x, int8_t r) {
	return (x << r) | (x >> (64 - r));
}

inline uint32_t rotr32(uint32_t x, int8_t r) {
	return (x >> r) | (x << (32 - r));
}

inline uint64_t rotr64(uint64_t x, int8_t r) {
	return (x >> r) | (x << (64 - r));
}

	#define ROTL32(x, y) rotl32(x, y)
	#define ROTL64(x, y) rotl64(x, y)
	#define ROTR32(x, y) rotr32(x, y)
	#define ROTR64(x, y) rotr64(x, y)

	#define BIG_CONSTANT(x) (x##LLU)

#endif

#if defined(small)
	#undef small
#endif

#include <stdio.h>

#if defined(_MSC_VER)

	#include <windows.h>

inline void setAffinity(uint32_t cpu) {
	SetProcessAffinityMask(GetCurrentProcess(), cpu);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

#else

	#include <sched.h>

inline void setAffinity(int32_t /*cpu*/) {
	#if !defined(__CYGWIN__) && !defined(__APPLE__)
	cpu_set_t mask;

	CPU_ZERO(&mask);

	CPU_SET(2, &mask);

	if (sched_setaffinity(0, sizeof(mask), &mask) == -1) {
		printf("WARNING: Could not set CPU affinity\n");
	}
	#endif
}

#endif
