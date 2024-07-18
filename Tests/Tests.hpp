#include "Platform.hpp"
#include "Types.hpp"
#include "AvalancheTest.hpp"
#include "DifferentialTest.hpp"
#include <xxhash.h>
#include "SpeedTest.hpp"
#include <jsonifier/Index.hpp>
#include <chrishendo/Index.hpp>

chrishendo::key_hasher hasherNew{};

template<bool isItSeeded> inline void chrishendoTestNew(const void* key, int32_t len, uint32_t seed, void* out) {
	if constexpr (isItSeeded) {
		hasherNew.setSeedRt(seed);
	}*(uint64_t*)out = hasherNew.hashKeyRt((const char*)key, len);
}

template<bool isItSeeded> inline void chrishendoTestNewCt(const void* key, int32_t len, uint32_t seed, void* out) {
	if constexpr (isItSeeded) {
		hasherNew.setSeedRt(seed);
	}
	*( uint64_t* )out = hasherNew.hashKeyCt(( const char* )key, len);
}

template<bool isItSeeded> inline void XXH3_64bitsTest(const void* key, int32_t len, uint32_t seed, void* out) {
	if constexpr (isItSeeded) {
		*( uint64_t* )out = XXH3_64bits_withSeed(( const char* )key, len, seed);
	} else {
		*( uint64_t* )out = XXH3_64bits(( const char* )key, len);
	}
}

constexpr uint64_t fnvOffsetBasis{ 0xcbf29ce484222325ull };
constexpr uint64_t fnvPrime{ 0x00000100000001B3ull };

inline uint64_t fnv1a(const char* string, uint64_t length) {
	uint64_t hashValue{ fnvOffsetBasis };
	for (uint64_t x = 0; x < length; ++x) {
		hashValue ^= static_cast<uint64_t>(string[x]);
		hashValue *= fnvPrime;
	}
	return hashValue;
}

template<bool isItSeeded> inline void fnv1aTest(const void* key, int32_t len, uint32_t seed, void* out) {
	*( uint64_t* )out = fnv1a(( const char* )key, len);
}

template<bool isItSeeded> struct HashInfo {
	pfHash<isItSeeded> hash{};
	uint32_t verification{};
	std::string desc{};
	int32_t hashbits{};
};

template<bool isItSeeded> std::array<HashInfo<isItSeeded>, 3> g_hashes{
	//HashInfo<isItSeeded>{ "cyan", fnv1aTest<isItSeeded>, 0x3A46B660, "Fnv1a", "A general purpose hasher", 64 },
	HashInfo<isItSeeded>{ XXH3_64bitsTest<isItSeeded>, 0x5A116D6B, "A win hash algo by xxHash", 64 },
	HashInfo<isItSeeded>{ chrishendoTestNew<isItSeeded>, 0x814F3144, "A general purpose hybrid hasher", 64 },
	HashInfo<isItSeeded>{ chrishendoTestNewCt<isItSeeded>, 0x814F3144, "A general purpose hybrid hasher", 64 },
};

inline uint32_t MurmurOAAT(const void* key, int32_t len, uint32_t seed) {
	const uint8_t* data = ( const uint8_t* )key;

	uint32_t h = seed;

	for (int32_t i = 0; i < len; i++) {
		h ^= data[i];
		h *= 0x5bd1e995;
		h ^= h >> 15;
	}

	return h;
}