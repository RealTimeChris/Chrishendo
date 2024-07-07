// The code in this header was sampled from the SMHasher test suite: https://github.com/aappleby/smhasher
#pragma once

#include "Types.hpp"

//-----------------------------------------------------------------------------
// Xorshift RNG based on code by George Marsaglia
// http://en.wikipedia.org/wiki/Xorshift

struct random_num {

	constexpr random_num() {
		reseed<0>();
	}

	inline random_num(uint32_t seed) {
		reseed(seed);
	}

	inline uint32_t rand_u32() {
		return operator()();
	}

	inline uint64_t rand_u64() {
		return operator()();
	}

	inline void rand_p(void* blob, int32_t bytes) {
		uint32_t* blocks = reinterpret_cast<uint32_t*>(blob);

		while (bytes >= 4) {
			blocks[0] = rand_u32();
			blocks++;
			bytes -= 4;
		}

		uint8_t* tail = reinterpret_cast<uint8_t*>(blocks);

		for (int32_t i = 0; i < bytes; i++) {
			tail[i] = ( uint8_t )rand_u32();
		}
	}

protected:
	size_t state[4]{};

	constexpr size_t rotl(const size_t x, size_t k) const {
		return (x << k) | (x >> (64ull - k));
	}

	constexpr size_t splitmix64(size_t& seed64) const {
		size_t result = seed64 += 0x9E3779B97F4A7C15ull;
		result = (result ^ (result >> 30ull)) * 0xBF58476D1CE4E5B9ull;
		result = (result ^ (result >> 27ull)) * 0x94D049BB133111EBull;
		return result ^ (result >> 31ull);
	}

	constexpr size_t operator()() {
		const size_t result = rotl(state[1ull] * 5ull, 7ull) * 9ull;

		const size_t t = state[1ull] << 17ull;

		state[2ull] ^= state[0ull];
		state[3ull] ^= state[1ull];
		state[1ull] ^= state[2ull];
		state[0ull] ^= state[3ull];

		state[2ull] ^= t;

		state[3ull] = rotl(state[3ull], 45ull);

		return result;
	}

	template<uint64_t newSeed> constexpr void reseed() {
		constexpr auto x = newSeed >> 12ull;
		constexpr auto x01 = x ^ x << 25ull;
		constexpr auto x02 = x01 ^ x01 >> 27ull;
		size_t s = x02 * 0x2545F4914F6CDD1Dull;
		for (uint64_t y = 0; y < 4; ++y) {
			state[y] = splitmix64(s);
		}
	}

	void reseed(uint64_t newSeed) {
		auto x = newSeed >> 12ull;
		auto x01 = x ^ x << 25ull;
		auto x02 = x01 ^ x01 >> 27ull;
		size_t s = x02 * 0x2545F4914F6CDD1Dull;
		for (uint64_t y = 0; y < 4; ++y) {
			state[y] = splitmix64(s);
		}
	}
};

inline struct random_num g_rand1(1);
inline struct random_num g_rand2(2);
inline struct random_num g_rand3(3);
inline struct random_num g_rand4(4);

//-----------------------------------------------------------------------------

inline uint32_t rand_u32(void) {
	return g_rand1.rand_u32();
}
inline uint64_t rand_u64(void) {
	return g_rand1.rand_u64();
}

inline void rand_p(void* blob, int32_t bytes) {
	uint32_t* blocks = ( uint32_t* )blob;

	while (bytes >= 4) {
		*blocks++ = rand_u32();
		bytes -= 4;
	}

	uint8_t* tail = ( uint8_t* )blocks;

	for (int32_t i = 0; i < bytes; i++) {
		tail[i] = ( uint8_t )rand_u32();
	}
}
