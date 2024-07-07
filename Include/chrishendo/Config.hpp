/*
	MIT License

	Copyright (c) 2024 RealTimeChris

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify, merge,
	publish, distribute, sublicense, and/or sell copies of the Software, and to permit
	persons to whom the Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all copies or
	substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
	PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
	FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
	OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	DEALINGS IN THE SOFTWARE.
*/
/// https://github.com/RealTimeChris/chrishendo
#pragma once

#include <cstdint>

#if !defined(__GNUC__)
	#pragma warning(disable : 4371)
	#pragma warning(disable : 4514)
	#pragma warning(disable : 4625)
	#pragma warning(disable : 4706)
	#pragma warning(disable : 4710)
	#pragma warning(disable : 4711)
	#pragma warning(disable : 4820)
	#pragma warning(disable : 5045)
	#pragma warning(disable : 5246)
#endif

#if defined(__clang__) || (defined(__GNUC__) && defined(__llvm__)) || (defined(__APPLE__) && defined(__clang__))
	#define CHRISHENDO_CLANG 1
#elif defined(_MSC_VER)
	#define CHRISHENDO_MSVC 1
#elif defined(__GNUC__) && !defined(__clang__)
	#define CHRISHENDO_GNUCXX 1
#endif

#if defined(__has_builtin)
	#define CHRISHENDO_HAS_BUILTIN(x) __has_builtin(x)
#else
	#define CHRISHENDO_HAS_BUILTIN(x) 0
#endif

#define CHRISHENDO_GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)

#if defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
	#define CHRISHENDO_MAC 1
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
	#define CHRISHENDO_LINUX 1
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
	#define CHRISHENDO_WIN 1
#endif

#if defined(CHRISHENDO_GNUCXX) || defined(CHRISHENDO_CLANG)
	#define LIKELY(x) (__builtin_expect(!!(x), 1))
	#define UNLIKELY(x) (__builtin_expect(!!(x), 0))
	#define ASSUME(x) \
		do { \
			if (!(x)) \
				__builtin_unreachable(); \
		} while (0)
#elif defined(CHRISHENDO_MSVC)
	#include <intrin.h>
	#define LIKELY(x) (x)
	#define UNLIKELY(x) (x)
	#define ASSUME(x) __assume(x)
#else
	#define LIKELY(x) (x)
	#define UNLIKELY(x) (x)
	#define ASSUME(x) (( void )0)
#endif

#if defined(CHRISHENDO_GNUCXX) || defined(CHRISHENDO_CLANG)
	#define CHRISHENDO_ALWAYS_INLINE __inline__ __attribute__((__always_inline__, __unused__))
	#define CHRISHENDO_NO_INLINE __attribute__((__noinline__))
	#define CHRISHENDO_INLINE __attribute__((__inline__))
#elif defined(CHRISHENDO_MSVC) /* Visual Studio */
	#define CHRISHENDO_ALWAYS_INLINE __forceinline
	#define CHRISHENDO_NO_INLINE __declspec(noinline)
	#define CHRISHENDO_INLINE inline
#elif defined(__cplusplus) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)) /* C99 */
	#define CHRISHENDO_ALWAYS_INLINE inline
	#define CHRISHENDO_INLINE inline
	#define CHRISHENDO_NO_INLINE 
#else
	#define CHRISHENDO_ALWAYS_INLINE 
	#define CHRISHENDO_INLINE 
	#define CHRISHENDO_NO_INLINE 
#endif

#if !defined(CHRISHENDO_CPU_INSTRUCTIONS)
	#define CHRISHENDO_CPU_INSTRUCTIONS 0
#endif

#if !defined CHRISHENDO_ALIGN
	#define CHRISHENDO_ALIGN alignas(chrishendoBytesPerStep)
#endif

#if !defined(CHRISHENDO_CHECK_FOR_INSTRUCTION)
	#define CHRISHENDO_CHECK_FOR_INSTRUCTION(x) (CHRISHENDO_CPU_INSTRUCTIONS & x)
#endif

#if !defined(CHRISHENDO_CHECK_FOR_AVX)
	#define CHRISHENDO_CHECK_FOR_AVX(x) (CHRISHENDO_CPU_INSTRUCTIONS >= x)
#endif

#if !defined(CHRISHENDO_POPCNT)
	#define CHRISHENDO_POPCNT (1 << 0)
#endif
#if !defined(CHRISHENDO_LZCNT)
	#define CHRISHENDO_LZCNT (1 << 1)
#endif
#if !defined(CHRISHENDO_BMI)
	#define CHRISHENDO_BMI (1 << 2)
#endif
#if !defined(CHRISHENDO_BMI2)
	#define CHRISHENDO_BMI2 (1 << 3)
#endif
#if !defined(CHRISHENDO_NEON)
	#define CHRISHENDO_NEON (1 << 4)
#endif
#if !defined(CHRISHENDO_AVX)
	#define CHRISHENDO_AVX (1 << 5)
#endif
#if !defined(CHRISHENDO_AVX2)
	#define CHRISHENDO_AVX2 (1 << 6)
#endif
#if !defined(CHRISHENDO_AVX512)
	#define CHRISHENDO_AVX512 (1 << 7)
#endif

#if !defined(CHRISHENDO_ANY)
	#define CHRISHENDO_ANY (CHRISHENDO_AVX | CHRISHENDO_AVX2 | CHRISHENDO_AVX512 | CHRISHENDO_POPCNT | CHRISHENDO_BMI | CHRISHENDO_BMI2 | CHRISHENDO_LZCNT)
#endif

#if !defined(CHRISHENDO_ANY_AVX)
	#define CHRISHENDO_ANY_AVX (CHRISHENDO_AVX | CHRISHENDO_AVX2 | CHRISHENDO_AVX512)
#endif

#if defined(CHRISHENDO_MSVC)
	#define CHRISHENDO_VISUAL_STUDIO 1
	#if defined(CHRISHENDO_CLANG)
		#define CHRISHENDO_CLANG_VISUAL_STUDIO 1
	#else
		#define CHRISHENDO_REGULAR_VISUAL_STUDIO 1
	#endif
#endif

#if CHRISHENDO_CHECK_FOR_INSTRUCTION(CHRISHENDO_ANY)

	#include <immintrin.h>

#endif

constexpr uint32_t mmShuffle(uint32_t fp3, uint32_t fp2, uint32_t fp1, uint32_t fp0) {
	return ((fp3 & 0x3) << 6) | ((fp2 & 0x3) << 4) | ((fp1 & 0x3) << 2) | (fp0 & 0x3);
}

#include <chrishendo/ISA/CTimeSimdTypes.hpp>

#if CHRISHENDO_CHECK_FOR_INSTRUCTION(CHRISHENDO_ANY_AVX)

using chrishendo_simd_int_128 = __m128i;
using chrishendo_simd_int_256 = __m256i;
using chrishendo_simd_int_512 = __m512i;

#if CHRISHENDO_CHECK_FOR_INSTRUCTION(CHRISHENDO_AVX512)
using chrishendo_simd_int_t = __m512i;
constexpr uint64_t chrishendoBitsPerStep{ 512 };
using chrishendo_simd_fb_type		  = chrishendo_internal::__m512x;
#elif CHRISHENDO_CHECK_FOR_INSTRUCTION(CHRISHENDO_AVX2)
using chrishendo_simd_int_t = __m256i;
constexpr uint64_t chrishendoBitsPerStep{ 256 };
using chrishendo_simd_fb_type		  = chrishendo_internal::__m256x;
#elif CHRISHENDO_CHECK_FOR_INSTRUCTION(CHRISHENDO_AVX)
using chrishendo_simd_int_t = __m128i;
constexpr uint64_t chrishendoBitsPerStep{ 128 };
using chrishendo_simd_fb_type		  = chrishendo_internal::__m128x;
#endif
#elif CHRISHENDO_CHECK_FOR_INSTRUCTION(CHRISHENDO_NEON)

#include <arm_neon.h>

using chrishendo_simd_int_128 = uint8x16_t;
using chrishendo_simd_int_256 = uint32_t;
using chrishendo_simd_int_512 = uint64_t;

using chrishendo_simd_int_t = uint8x16_t;
constexpr uint64_t chrishendoBitsPerStep{ 128 };
using string_parsing_type = uint16_t;
using chrishendo_simd_fb_type		  = chrishendo_internal::__m128x;
#else
using chrishendo_simd_int_128 = chrishendo_internal::__m128x;
using chrishendo_simd_int_256 = uint32_t;
using chrishendo_simd_int_512 = uint64_t;

using chrishendo_simd_int_t = chrishendo_internal::__m128x;
constexpr uint64_t chrishendoBitsPerStep{ 128 };
using string_parsing_type = uint16_t;
using chrishendo_simd_fb_type		  = chrishendo_internal::__m128x;
#endif

constexpr uint64_t chrishendoBytesPerStep{ chrishendoBitsPerStep / 8 };
constexpr uint64_t chrishendoSixtyFourBitsPerStep{ chrishendoBitsPerStep / 64 };
constexpr uint64_t chrishendoStridesPerStep{ chrishendoBitsPerStep / chrishendoBytesPerStep };

using string_view_ptr	= const char*;
using structural_index	= const char*;
using string_buffer_ptr = char*;

#if defined(__APPLE__) && defined(__arm64__)
#define PREFETCH(ptr) __builtin_prefetch(ptr, 0, 0);
#elif defined(CHRISHENDO_MSVC)
#include <intrin.h>
#define PREFETCH(ptr) _mm_prefetch(static_cast<const char*>(ptr), _MM_HINT_T0);
#elif defined(CHRISHENDO_GNUCXX) || defined(CHRISHENDO_CLANG)
#include <xmmintrin.h>
#define PREFETCH(ptr) _mm_prefetch(static_cast<const char*>(ptr), _MM_HINT_T0);
#else
#error "Compiler or architecture not supported for prefetching"
#endif

CHRISHENDO_INLINE void chrishendoPrefetchInternal(const void* ptr) {
PREFETCH(ptr)
}

CHRISHENDO_INLINE constexpr uint32_t CHRISHENDO_rotl32Ct(uint32_t x, uint32_t r) {
	return (((x) << (r)) | ((x) >> (32 - (r))));
};

CHRISHENDO_INLINE constexpr uint64_t CHRISHENDO_rotl64Ct(uint64_t x, uint64_t r) {
	return (((x) << (r)) | ((x) >> (64 - (r))));
};

#if !defined(NO_CLANG_BUILTIN) && CHRISHENDO_HAS_BUILTIN(__builtin_rotateleft32) && CHRISHENDO_HAS_BUILTIN(__builtin_rotateleft64)
	#define CHRISHENDO_rotl32Rt __builtin_rotateleft32
	#define CHRISHENDO_rotl64Rt __builtin_rotateleft64
#elif defined(CHRISHENDO_MSVC)
#  define CHRISHENDO_rotl32Rt(x,r) _rotl(x,r)
#  define CHRISHENDO_rotl64Rt(x,r) _rotl64(x,r)
#else
	#define CHRISHENDO_rotl32Rt(x, r) CHRISHENDO_rotl32Ct(x, r)
	#define CHRISHENDO_rotl64Rt(x, r) CHRISHENDO_rotl64Ct(x, r)
#endif

CHRISHENDO_INLINE constexpr uint32_t CHRISHENDO_swap32Ct(uint32_t x) {
	return ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
}

#if defined(CHRISHENDO_MSVC)
	#  define CHRISHENDO_swap32Rt _byteswap_ulong
#elif CHRISHENDO_GCC_VERSION >= 403
#  define CHRISHENDO_swap32Rt __builtin_bswap32
#else
	#define CHRISHENDO_swap32Rt CHRISHENDO_swap32Ct
#endif

CHRISHENDO_INLINE constexpr uint64_t CHRISHENDO_swap64Ct(uint64_t x) {
	return ((x << 56) & 0xff00000000000000ULL) | ((x << 40) & 0x00ff000000000000ULL) | ((x << 24) & 0x0000ff0000000000ULL) | ((x << 8) & 0x000000ff00000000ULL) |
		((x >> 8) & 0x00000000ff000000ULL) | ((x >> 24) & 0x0000000000ff0000ULL) | ((x >> 40) & 0x000000000000ff00ULL) | ((x >> 56) & 0x00000000000000ffULL);
}

#if defined(CHRISHENDO_MSVC)
	#define CHRISHENDO_swap64Rt _byteswap_uint64
#elif CHRISHENDO_GCC_VERSION >= 403
	#define CHRISHENDO_swap64Rt __builtin_bswap64
#else
	#define CHRISHENDO_swap64Rt CHRISHENDO_swap64Ct
#endif