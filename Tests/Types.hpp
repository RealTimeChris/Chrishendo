// The code in this header was sampled from the SMHasher test suite: https://github.com/aappleby/smhasher
#pragma once

#include <jsonifier/Index.hpp>
#include "Platform.hpp"
//#include "Bitvec.hpp"
#include "Random.hpp"
#if defined (setbit)
#undef setbit
#endif
#include <assert.h>
#include <memory.h>
#include <vector>
#include <map>
#include <set>

//-----------------------------------------------------------------------------
// We want to verify that every test produces the same result on every platform
// To do this, we hash the results of every test to produce an overall
// verification value for the whole test suite. If two runs produce the same
// verification value, then every test in both run produced the same results

void printbits(const void* blob, int32_t len) {
	const uint8_t* data = ( const uint8_t* )blob;

	printf("[");
	for (int32_t i = 0; i < len; i++) {
		unsigned char byte = data[i];

		int32_t hi = (byte >> 4);
		int32_t lo = (byte & 0xF);

		if (hi)
			printf("%01x", hi);
		else
			printf(".");

		if (lo)
			printf("%01x", lo);
		else
			printf(".");

		if (i != len - 1)
			printf(" ");
	}
	printf("]");
}

extern uint32_t g_verify;

inline void clearbit(void* block, uint32_t len, uint32_t bit) {
	uint8_t* b = ( uint8_t* )block;

	uint32_t byte = bit >> 3;
	bit			  = bit & 0x7;

	if (byte < len)
		b[byte] &= ~(1 << bit);
}

inline uint32_t getbit_wrap(const void* block, uint32_t len, uint32_t bit) {
	uint8_t* b = ( uint8_t* )block;

	uint32_t byte = bit >> 3;
	bit			  = bit & 0x7;

	byte %= len;

	return static_cast<uint32_t>(b[byte] >> bit) & 1u;
}

inline void setbit(void* block, uint32_t len, uint32_t bit) {
	uint8_t* b = ( uint8_t* )block;

	uint32_t byte = bit >> 3;
	bit			  = bit & 0x7;

	if (byte < len)
		b[byte] |= (1 << bit);
}

inline void setbit(void* block, uint32_t len, uint32_t bit, uint32_t val) {
	val ? setbit(block, len, bit) : clearbit(block, len, bit);
}

inline uint32_t window1(void* blob, uint32_t len, uint32_t start, uint32_t count) {
	uint32_t nbits = len * 8;
	start %= nbits;

	uint32_t t = 0;

	for (uint32_t i = 0; i < count; i++) {
		setbit(&t, sizeof(t), i, getbit_wrap(blob, len, start + i));
	}

	return t;
}

inline uint32_t window8(void* blob, int32_t len, int32_t start, int32_t count) {
	int32_t nbits = len * 8;
	start %= nbits;

	uint32_t t = 0;
	uint8_t* k = ( uint8_t* )blob;

	if (count == 0)
		return 0;

	int32_t c = start & (8 - 1);
	int32_t d = start / 8;

	for (int32_t i = 0; i < 4; i++) {
		int32_t ia = (i + d + 1) % len;
		int32_t ib = (i + d + 0) % len;

		uint32_t a = k[ia];
		uint32_t b = k[ib];

		uint32_t m = (a << (8 - c)) | (b >> c);

		t |= (m << (8 * i));
	}

	t &= ((1 << count) - 1);

	return t;
}

inline uint32_t window32(void* blob, int32_t len, int32_t start, int32_t count) {
	int32_t nbits = len * 8;
	start %= nbits;

	assert((len & 3) == 0);

	int32_t ndwords = len / 4;

	uint32_t* k = ( uint32_t* )blob;

	if (count == 0)
		return 0;

	int32_t c = start & (32 - 1);
	int32_t d = start / 32;

	if (c == 0)
		return (k[d] & ((1 << count) - 1));

	int32_t ia = (d + 1) % ndwords;
	int32_t ib = (d + 0) % ndwords;

	uint32_t a = k[ia];
	uint32_t b = k[ib];

	uint32_t t = (a << (32 - c)) | (b >> c);

	t &= ((1 << count) - 1);

	return t;
}

inline uint32_t window(void* blob, int32_t len, int32_t start, int32_t count) {
	if (len & 3) {
		return window8(blob, len, start, count);
	} else {
		return window32(blob, len, start, count);
	}
}

template<typename T> inline uint32_t window(T& blob, int32_t start, int32_t count) {
	if ((sizeof(T) & 3) == 0) {
		return window32(&blob, sizeof(T), start, count);
	} else {
		return window8(&blob, sizeof(T), start, count);
	}
}

template<> inline uint32_t window(uint32_t& blob, int32_t start, int32_t count) {
	return ROTR32(blob, start) & ((1 << count) - 1);
}

template<> inline uint32_t window(uint64_t& blob, int32_t start, int32_t count) {
	return ( uint32_t )ROTR64(blob, start) & ((1 << count) - 1);
}

inline uint32_t getbit(const void* block, uint32_t len, uint32_t bit) {
	uint8_t* b = ( uint8_t* )block;

	uint32_t byte = bit >> 3u;
	bit			  = bit & 0x7u;

	if (byte < len)
		return (b[byte] >> bit) & 1u;

	return 0;
}

inline void flipbit(void* block, uint32_t len, uint32_t bit) {
	uint8_t* b = ( uint8_t* )block;

	uint32_t byte = bit >> 3u;
	bit			  = bit & 0x7u;

	if (byte < len)
		b[byte] ^= (1u << bit);
}

inline void lshift1(void* blob, uint32_t len, uint32_t c) {
	int32_t nbits = static_cast<int32_t>(len * 8);

	for (int32_t i = nbits - 1; i >= 0; i--) {
		setbit(blob, len, static_cast<uint32_t>(i), getbit(blob, len, i - c));
	}
}

void lrot1(void* blob, uint32_t len, uint32_t c) {
	uint32_t nbits = len * 8;

	for (uint32_t i = 0; i < c; i++) {
		uint32_t bit = getbit(blob, len, nbits - 1);

		lshift1(blob, len, 1);

		setbit(blob, len, 0, bit);
	}
}

void lrot8(void* blob, int32_t len, int32_t c) {
	int32_t nbytes = len;

	uint8_t* k = ( uint8_t* )blob;

	if (c == 0)
		return;

	//----------

	int32_t b = c / 8;
	c &= (8 - 1);

	for (int32_t j = 0; j < b; j++) {
		uint8_t t = k[nbytes - 1];

		for (int32_t i = nbytes - 1; i > 0; i--) {
			k[i] = k[i - 1];
		}

		k[0] = t;
	}

	uint8_t t = k[nbytes - 1];

	if (c == 0)
		return;

	for (int32_t i = nbytes - 1; i >= 0; i--) {
		uint8_t a	 = k[i];
		uint8_t bNew = (i == 0) ? t : k[i - 1];

		k[i] = static_cast<uint8_t>((a << c) | (bNew >> (8 - c)));
	}
}

void lrot32(void* blob, int32_t len, int32_t c) {
	assert((len & 3) == 0);

	int32_t nbytes	= len;
	int32_t ndwords = nbytes / 4;

	uint32_t* k = ( uint32_t* )blob;

	if (c == 0)
		return;

	//----------

	int32_t b = c / 32;
	c &= (32 - 1);

	for (int32_t j = 0; j < b; j++) {
		uint32_t t = k[ndwords - 1];

		for (int32_t i = ndwords - 1; i > 0; i--) {
			k[i] = k[i - 1];
		}

		k[0] = t;
	}

	uint32_t t = k[ndwords - 1];

	if (c == 0)
		return;

	for (int32_t i = ndwords - 1; i >= 0; i--) {
		uint32_t a	  = k[i];
		uint32_t bNew = (i == 0) ? t : k[i - 1];

		k[i] = static_cast<uint8_t>((a << c) | (bNew >> (32 - c)));
	}
}

inline void lrot(void* blob, int32_t len, int32_t c) {
	if ((len & 3) == 0) {
		return lrot32(blob, len, c);
	} else {
		return lrot8(blob, len, c);
	}
}

inline void printbytes(const void* blob, int32_t len) {
	uint8_t* d = ( uint8_t* )blob;

	printf("{ ");

	for (int32_t i = 0; i < len; i++) {
		printf("0x%02x, ", d[i]);
	}

	printf(" };");
}

// Mix the given blob of data into the verification code

inline uint32_t popcount(uint32_t v) {
	v		   = v - ((v >> 1) & 0x55555555);// reuse input as temporary
	v		   = (v & 0x33333333) + ((v >> 2) & 0x33333333);// temp
	uint32_t c = ((v + ((v >> 4) & 0xF0F0F0F)) * 0x1010101) >> 24;// count

	return c;
}

void MixVCode(const void* blob, int32_t len);

template<typename T> inline void flipbit(T& blob, uint32_t bit) {
	flipbit(&blob, sizeof(blob), bit);
}

template<typename T> inline uint32_t getbit(T& blob, uint32_t bit) {
	return getbit(&blob, sizeof(blob), bit);
}

template<> inline uint32_t getbit(uint32_t& blob, uint32_t bit) {
	return (blob >> (bit & 31)) & 1;
}
template<> inline uint32_t getbit(uint64_t& blob, uint32_t bit) {
	return (blob >> (bit & 63)) & 1;
}

//-----------------------------------------------------------------------------	
template<bool isItSeeded = false>
using pfHash=void (*)(const void* blob, const int32_t len, const uint32_t seed, void* out);

struct ByteVec : public jsonifier::vector<uint8_t> {
	ByteVec(const void* key, uint32_t len) {
		resize(len);
		memcpy(&front(), key, len);
	}
};

template<typename hashtype, typename keytype> struct CollisionMap : public std::map<hashtype, jsonifier::vector<keytype>> {};

template<typename hashtype> struct Hashset : public std::set<hashtype> {};

//-----------------------------------------------------------------------------

template<bool isItSeeded , class T> class hashfunc {
  public:
	hashfunc() noexcept = default;
	hashfunc(pfHash<isItSeeded> h) : m_hash(h) {
	}

	inline void operator()(const void* key, const int32_t len, const uint32_t seed, uint32_t* out) {
		m_hash(key, len, seed, out);
	}

	inline operator pfHash<isItSeeded>(void) const {
		return m_hash;
	}

	inline T operator()(const void* key, const int32_t len, const uint32_t seed) {
		T result;

		m_hash(key, len, seed, ( void* )&result);

		return result;
	}

	pfHash<isItSeeded> m_hash;
};

//-----------------------------------------------------------------------------
// Key-processing callback objects. Simplifies keyset testing a bit.

struct KeyCallback {
	KeyCallback() : m_count(0) {
	}

	virtual ~KeyCallback() {
	}

	virtual void operator()(const void* key, int32_t len) {
		m_count++;
	}

	virtual void reserve(int32_t keycount) {};

	int32_t m_count;
};

//----------

template<bool isItSeeded, typename hashtype> struct HashCallback : public KeyCallback {
	typedef jsonifier::vector<hashtype> hashvec;

	HashCallback(pfHash<isItSeeded> hash, hashvec& hashes) : m_hashes(hashes), m_pfHash(hash) {
		m_hashes.clear();
	}

	virtual void operator()(const void* key, int32_t len) {
		size_t newsize = m_hashes.size() + 1;

		m_hashes.resize(newsize);

		m_pfHash(key, len, 0, &m_hashes.back());
	}

	virtual void reserve(int32_t keycount) {
		m_hashes.reserve(keycount);
	}

	hashvec& m_hashes;
	pfHash<isItSeeded> m_pfHash;

	//----------

  private:
	HashCallback& operator=(const HashCallback&);
};

//----------

template<bool isItSeeded, typename hashtype> struct CollisionCallback : public KeyCallback {
	typedef Hashset<hashtype> hashset;
	typedef CollisionMap<hashtype, ByteVec> collmap;

	CollisionCallback(pfHash<isItSeeded> hash, hashset& collisions, collmap& cmap) : m_pfHash(hash), m_collisions(collisions), m_collmap(cmap) {
	}

	virtual void operator()(const void* key, int32_t len) {
		hashtype h;

		m_pfHash(key, len, 0, &h);

		if (m_collisions.count(h)) {
			m_collmap[h].emplace_back(ByteVec(key, len));
		}
	}

	//----------

	pfHash<isItSeeded> m_pfHash;
	hashset& m_collisions;
	collmap& m_collmap;

  private:
	CollisionCallback& operator=(const CollisionCallback& c);
};

//-----------------------------------------------------------------------------

template<int32_t _bits> class Blob {
  public:
	Blob() {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			bytes[i] = 0;
		}
	}

	Blob(int32_t x) {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			bytes[i] = 0;
		}

		*( int32_t* )bytes = x;
	}

	Blob(const Blob& k) {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			bytes[i] = k.bytes[i];
		}
	}

	Blob& operator=(const Blob& k) {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			bytes[i] = k.bytes[i];
		}

		return *this;
	}

	Blob(uint64_t a, uint64_t b) {
		uint64_t t[2] = { a, b };
		set(&t, 16);
	}

	void set(const void* blob, size_t len) {
		const uint8_t* k = ( const uint8_t* )blob;

		len = len > sizeof(bytes) ? sizeof(bytes) : len;

		for (size_t i = 0; i < len; i++) {
			bytes[i] = k[i];
		}

		for (size_t i = len; i < sizeof(bytes); i++) {
			bytes[i] = 0;
		}
	}

	uint8_t& operator[](int32_t i) {
		return bytes[i];
	}

	const uint8_t& operator[](int32_t i) const {
		return bytes[i];
	}

	//----------
	// boolean operations

	bool operator<(const Blob& k) const {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			if (bytes[i] < k.bytes[i])
				return true;
			if (bytes[i] > k.bytes[i])
				return false;
		}

		return false;
	}

	bool operator==(const Blob& k) const {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			if (bytes[i] != k.bytes[i])
				return false;
		}

		return true;
	}

	bool operator!=(const Blob& k) const {
		return !(*this == k);
	}

	//----------
	// bitwise operations

	Blob operator^(const Blob& k) const {
		Blob t;

		for (size_t i = 0; i < sizeof(bytes); i++) {
			t.bytes[i] = bytes[i] ^ k.bytes[i];
		}

		return t;
	}

	Blob& operator^=(const Blob& k) {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			bytes[i] ^= k.bytes[i];
		}

		return *this;
	}

	int32_t operator&(int32_t x) {
		return (*( int32_t* )bytes) & x;
	}

	Blob& operator&=(const Blob& k) {
		for (size_t i = 0; i < sizeof(bytes); i++) {
			bytes[i] &= k.bytes[i];
		}
	}

	Blob operator<<(int32_t c) {
		Blob t = *this;

		lshift(&t.bytes[0], sizeof(bytes), c);

		return t;
	}

	Blob operator>>(int32_t c) {
		Blob t = *this;

		rshift(&t.bytes[0], sizeof(bytes), c);

		return t;
	}

	Blob& operator<<=(int32_t c) {
		lshift(&bytes[0], sizeof(bytes), c);

		return *this;
	}

	Blob& operator>>=(int32_t c) {
		rshift(&bytes[0], sizeof(bytes), c);

		return *this;
	}

	//----------

  private:
	uint8_t bytes[(_bits + 7) / 8];
};

typedef Blob<128> uint128_t;
typedef Blob<256> uint256_t;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#if defined(_MSC_VER)
	#pragma optimize("", off)
#endif

#if defined(_MSC_VER)
	#pragma optimize("", on)
#endif

inline uint32_t g_verify = 1;

//-----------------------------------------------------------------------------

inline bool isprime(uint32_t x) {
	uint32_t p[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157,
		163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251 };

	for (size_t i = 0; i < sizeof(p) / sizeof(uint32_t); i++) {
		if ((x % p[i]) == 0) {
			return false;
		}
	}

	for (int32_t i = 257; i < 65536; i += 2) {
		if ((x % i) == 0) {
			return false;
		}
	}

	return true;
}

inline void GenerateMixingConstants(void) {
	struct random_num r(8350147);

	int32_t count = 0;

	int32_t trials	  = 0;
	int32_t bitfail	  = 0;
	int32_t popfail	  = 0;
	int32_t matchfail = 0;
	int32_t primefail = 0;

	//for(uint32_t x = 1; x; x++)
	while (count < 100) {
		//if(x % 100000000 == 0) printf(".");

		trials++;
		uint32_t b = r.rand_u32();
		//uint32_t b = x;

		//----------
		// must have between 14 and 18 set bits

		if (popcount(b) < 16) {
			b = 0;
			popfail++;
		}
		if (popcount(b) > 16) {
			b = 0;
			popfail++;
		}

		if (b == 0)
			continue;

		//----------
		// must have 3-5 bits set per 8-bit window

		for (int32_t i = 0; i < 32; i++) {
			uint32_t c = ROTL32(b, i) & 0xFF;

			if (popcount(c) < 3) {
				b = 0;
				bitfail++;
				break;
			}
			if (popcount(c) > 5) {
				b = 0;
				bitfail++;
				break;
			}
		}

		if (b == 0)
			continue;

		//----------
		// all 8-bit windows must be different

		uint8_t match[256];

		memset(match, 0, 256);

		for (int32_t i = 0; i < 32; i++) {
			uint32_t c = ROTL32(b, i) & 0xFF;

			if (match[c]) {
				b = 0;
				matchfail++;
				break;
			}

			match[c] = 1;
		}

		if (b == 0)
			continue;

		//----------
		// must be prime

		if (!isprime(b)) {
			b = 0;
			primefail++;
		}

		if (b == 0)
			continue;

		//----------

		if (b) {
			printf("0x%08x : 0x%08x\n", b, ~b);
			count++;
		}
	}

	printf("%d %d %d %d %d %d\n", trials, popfail, bitfail, matchfail, primefail, count);
}

//-----------------------------------------------------------------------------
#include <filesystem>
#include <fstream>

class file_loader {
  public:
	file_loader(const std::string& filePathNew) {
		filePath = filePathNew;
		std::string directory{ filePathNew.substr(0, filePathNew.find_last_of("/") + 1) };
		if (!std::filesystem::exists(directory.operator std::basic_string_view<char, std::char_traits<char>>())) {
			std::filesystem::create_directories(directory.operator std::basic_string_view<char, std::char_traits<char>>());
		}

		if (!std::filesystem::exists(filePath.operator std::basic_string_view<char, std::char_traits<char>>())) {
			std::ofstream createFile(filePath.data());
			createFile.close();
		}

		std::ifstream theStream(filePath.data(), std::ios::binary | std::ios::in);
		std::stringstream inputStream{};
		inputStream << theStream.rdbuf();
		fileContents = inputStream.str();
		theStream.close();
	}

	void saveFile(const std::string& fileToSave) {
		std::ofstream theStream(filePath.data(), std::ios::binary | std::ios::out | std::ios::trunc);
		theStream.write(fileToSave.data(), static_cast<int64_t>(fileToSave.size()));
		theStream.close();
	}

	operator std::string&() {
		return fileContents;
	}

  protected:
	std::string fileContents{};
	std::string filePath{};
};

struct benchmark_result_final_parse {
	double medianAbsolutePercentageError{};
	jsonifier::string benchmarkColor{};
	jsonifier::string benchmarkName{};
	jsonifier::string libraryName{};
	uint64_t iterationCount{};
	double resultValue{};
};

struct benchmark_suite_results {
	inline benchmark_suite_results() noexcept = default;
	inline benchmark_suite_results(auto& values) {
		for (uint64_t x = 0; x < values.currentCount; ++x) {
			results.emplace_back(values.results[x]);
		}
		benchmarkSuiteName = values.benchmarkSuiteName;
	}
	jsonifier::vector<benchmark_result_final_parse> results{};
	jsonifier::string benchmarkSuiteName{};
};

namespace jsonifier {

	template<> struct core<benchmark_result_final_parse> {
		using value_type				 = benchmark_result_final_parse;
		static constexpr auto parseValue = createValue<&value_type::benchmarkName, &value_type::medianAbsolutePercentageError, &value_type::resultValue, &value_type::benchmarkColor,
			&value_type::iterationCount, &value_type::libraryName>();
	};

	template<> struct core<benchmark_suite_results> {
		using value_type				 = benchmark_suite_results;
		static constexpr auto parseValue = createValue<&value_type::results, &value_type::benchmarkSuiteName>();
	};
}