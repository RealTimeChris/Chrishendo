// The code in this header was sampled from the SMHasher test suite: https://github.com/aappleby/smhasher
//-----------------------------------------------------------------------------
// Keyset tests generate various sorts of difficult-to-hash keysets and compare
// the distribution and collision frequency of the hash results against an
// ideal random_num distribution

// The sanity checks are also in this cpp/h
#pragma once

#include "Types.hpp"
#include "Stats.hpp"
#include "Random.hpp"// for rand_p

#include <algorithm>// for std::swap
#include <iostream>
#include <assert.h>

//-----------------------------------------------------------------------------
// Keyset 'Combination' - all possible combinations of input blocks

template<bool isItSeeded, typename hashtype>
void CombinationKeygenRecurse(uint32_t* key, int32_t len, int32_t maxlen, uint32_t* blocks, int32_t blockcount, pfHash<isItSeeded> hash, jsonifier::vector<hashtype>& hashes) {
	if (len == maxlen)
		return;

	for (int32_t i = 0; i < blockcount; i++) {
		key[len] = blocks[i];

		//if(len == maxlen-1)
		{
			hashtype h;
			//std::cout << "CURRENT LENGTH: " << (len + 1) * sizeof(int32_t) << std::endl;
			hash(key, (len + 1) * sizeof(uint32_t), 0, &h);
			hashes.push_back(h);
		}

		//else
		{ CombinationKeygenRecurse<isItSeeded>(key, len + 1, maxlen, blocks, blockcount, hash, hashes); }
	}
}

template<bool isItSeeded, typename hashtype>
bool CombinationKeyTest(hashfunc<isItSeeded, hashtype> hash, int32_t maxlen, uint32_t* blocks, int32_t blockcount, bool testColl, bool testDist, bool drawDiagram) {
	printf("Keyset 'Combination' - up to %d blocks from a set of %d - ", maxlen, blockcount);

	//----------

	jsonifier::vector<hashtype> hashes;

	uint32_t* key = new uint32_t[maxlen];

	CombinationKeygenRecurse<isItSeeded, hashtype>(key, 0, maxlen, blocks, blockcount, hash, hashes);

	delete[] key;

	printf("%d keys\n", ( int32_t )hashes.size());

	//----------

	bool result = true;

	result &= TestHashList<hashtype>(hashes, testColl, testDist, drawDiagram);

	printf("\n");

	return result;
}

//----------------------------------------------------------------------------
// Keyset 'Permutation' - given a set of 32-bit blocks, generate keys
// consisting of all possible permutations of those blocks

template<bool isItSeeded, typename hashtype> void PermutationKeygenRecurse(pfHash<isItSeeded> hash, uint32_t* blocks, int32_t blockcount, int32_t k, jsonifier::vector<hashtype>& hashes) {
	if (k == blockcount - 1) {
		hashtype h;

		hash(blocks, blockcount * sizeof(uint32_t), 0, &h);

		hashes.push_back(h);

		return;
	}

	for (int32_t i = k; i < blockcount; i++) {
		std::swap(blocks[k], blocks[i]);

		PermutationKeygenRecurse(hash, blocks, blockcount, k + 1, hashes);

		std::swap(blocks[k], blocks[i]);
	}
}

template<bool isItSeeded, typename hashtype>
bool PermutationKeyTest(hashfunc<isItSeeded, hashtype> hash, uint32_t* blocks, int32_t blockcount, bool testColl, bool testDist, bool drawDiagram) {
	printf("Keyset 'Permutation' - %d blocks - ", blockcount);

	//----------

	jsonifier::vector<hashtype> hashes;

	PermutationKeygenRecurse<hashtype>(hash, blocks, blockcount, 0, hashes);

	printf("%d keys\n", ( int32_t )hashes.size());

	//----------

	bool result = true;

	result &= TestHashList<hashtype>(hashes, testColl, testDist, drawDiagram);

	printf("\n");

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'Sparse' - generate all possible N-bit keys with up to K bits set

template<bool isItSeeded, typename keytype, typename hashtype>
void SparseKeygenRecurse(pfHash<isItSeeded> hash, int32_t start, int32_t bitsleft, bool inclusive, keytype& k, jsonifier::vector<hashtype>& hashes) {
	const int32_t nbytes = sizeof(keytype);
	const int32_t nbits	 = nbytes * 8;

	hashtype h;

	for (int32_t i = start; i < nbits; i++) {
		flipbit(&k, nbytes, i);

		if (inclusive || (bitsleft == 1)) {
			hash(&k, sizeof(keytype), 0, &h);
			hashes.push_back(h);
		}

		if (bitsleft > 1) {
			SparseKeygenRecurse(hash, i + 1, bitsleft - 1, inclusive, k, hashes);
		}

		flipbit(&k, nbytes, i);
	}
}

//----------

template<bool isItSeeded, int32_t keybits, typename hashtype>
bool SparseKeyTest(hashfunc<isItSeeded, hashtype> hash, const int32_t setbits, bool inclusive, bool testColl, bool testDist, bool drawDiagram) {
	printf("Keyset 'Sparse' - %d-bit keys with %s %d bits set - ", keybits, inclusive ? "up to" : "exactly", setbits);

	typedef Blob<keybits> keytype;

	jsonifier::vector<hashtype> hashes;

	keytype k;
	memset(&k, 0, sizeof(k));

	if (inclusive) {
		hashtype h;

		hash(&k, sizeof(keytype), 0, &h);

		hashes.push_back(h);
	}

	SparseKeygenRecurse(hash, 0, setbits, inclusive, k, hashes);

	printf("%d keys\n", ( int32_t )hashes.size());

	bool result = true;

	result &= TestHashList<hashtype>(hashes, testColl, testDist, drawDiagram);

	printf("\n");

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'Windows' - for all possible N-bit windows of a K-bit key, generate
// all possible keys with bits set in that window

template<bool isItSeeded,typename keytype, typename hashtype> bool WindowedKeyTest(hashfunc<isItSeeded, hashtype> hash, const int32_t windowbits, bool testCollision, bool testDistribution, bool drawDiagram) {
	const int32_t keybits  = sizeof(keytype) * 8;
	const int32_t keycount = 1 << windowbits;

	jsonifier::vector<hashtype> hashes;
	hashes.resize(keycount);

	bool result = true;

	int32_t testcount = keybits;

	printf("Keyset 'Windowed' - %3d-bit key, %3d-bit window - %d tests, %d keys per test\n", keybits, windowbits, testcount, keycount);

	for (int32_t j = 0; j <= testcount; j++) {
		int32_t minbit = j;

		keytype key;

		for (int32_t i = 0; i < keycount; i++) {
			key = i;
			//key = key << minbit;

			lrot(&key, sizeof(keytype), minbit);

			hash(&key, sizeof(keytype), 0, &hashes[i]);
		}

		printf("Window at %3d - ", j);

		result &= TestHashList(hashes, testCollision, testDistribution, drawDiagram);

		//printf("\n");
	}

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'Cyclic' - generate keys that consist solely of N repetitions of M
// bytes.

// (This keyset type is designed to make MurmurHash2 fail)

template<bool isItSeeded, typename hashtype> bool CyclicKeyTest(pfHash<isItSeeded> hash, int32_t cycleLen, int32_t cycleReps, const int32_t keycount, bool drawDiagram) {
	printf("Keyset 'Cyclic' - %d cycles of %d bytes - %d keys\n", cycleReps, cycleLen, keycount);

	random_num r(483723);

	jsonifier::vector<hashtype> hashes;
	hashes.resize(keycount);

	int32_t keyLen = cycleLen * cycleReps;

	uint8_t* cycle = new uint8_t[cycleLen + 16];
	uint8_t* key   = new uint8_t[keyLen];

	//----------

	for (int32_t i = 0; i < keycount; i++) {
		r.rand_p(cycle, cycleLen);

		*( uint32_t* )cycle = f3mix(i ^ 0x746a94f1);

		for (int32_t j = 0; j < keyLen; j++) {
			key[j] = cycle[j % cycleLen];
		}

		hash(key, keyLen, 0, &hashes[i]);
	}

	//----------

	bool result = true;

	result &= TestHashList(hashes, true, true, drawDiagram);
	printf("\n");

	delete[] cycle;
	delete[] key;

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'TwoBytes' - generate all keys up to length N with two non-zero bytes

void TwoBytesKeygen(int32_t maxlen, KeyCallback& c);

template<bool isItSeeded, typename hashtype> bool TwoBytesTest2(pfHash<isItSeeded> hash, int32_t maxlen, bool drawDiagram) {
	jsonifier::vector<hashtype> hashes;

	HashCallback<isItSeeded, hashtype> c(hash, hashes);

	TwoBytesKeygen(maxlen, c);

	bool result = true;

	result &= TestHashList(hashes, true, true, drawDiagram);
	printf("\n");

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'Text' - generate all keys of the form "prefix"+"core"+"suffix",
// where "core" consists of all possible combinations of the given character
// set of length N.

template<bool isItSeeded, typename hashtype>
bool TextKeyTest(hashfunc<isItSeeded, hashtype> hash, const char* prefix, const char* coreset, const int32_t corelen, const char* suffix, bool drawDiagram) {
	const int32_t prefixlen = ( int32_t )strlen(prefix);
	const int32_t suffixlen = ( int32_t )strlen(suffix);
	const int32_t corecount = ( int32_t )strlen(coreset);

	const int32_t keybytes = prefixlen + corelen + suffixlen;
	const int32_t keycount = ( int32_t )pow(double(corecount), double(corelen));

	printf("Keyset 'Text' - keys of form \"%s[", prefix);
	for (int32_t i = 0; i < corelen; i++)
		printf("X");
	printf("]%s\" - %d keys\n", suffix, keycount);

	uint8_t* key = new uint8_t[keybytes + 1];

	key[keybytes] = 0;

	memcpy(key, prefix, prefixlen);
	memcpy(key + prefixlen + corelen, suffix, suffixlen);

	//----------

	jsonifier::vector<hashtype> hashes;
	hashes.resize(keycount);

	for (int32_t i = 0; i < keycount; i++) {
		int32_t t = i;

		for (int32_t j = 0; j < corelen; j++) {
			key[prefixlen + j] = coreset[t % corecount];
			t /= corecount;
		}

		hash(key, keybytes, 0, &hashes[i]);
	}

	//----------

	bool result = true;

	result &= TestHashList(hashes, true, true, drawDiagram);

	printf("\n");

	delete[] key;

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'Zeroes' - keys consisting of all zeroes, differing only in length

// We reuse one block of empty bytes, otherwise the RAM cost is enormous.

template<bool isItSeeded, typename hashtype> bool ZeroKeyTest(pfHash<isItSeeded> hash, bool drawDiagram) {
	int32_t keycount = 64 * 1024;

	printf("Keyset 'Zeroes' - %d keys\n", keycount);

	unsigned char* nullblock = new unsigned char[keycount];
	memset(nullblock, 0, keycount);

	//----------

	jsonifier::vector<hashtype> hashes;

	hashes.resize(keycount);

	for (int32_t i = 0; i < keycount; i++) {
		hash(nullblock, i, 0, &hashes[i]);
	}

	bool result = true;

	result &= TestHashList(hashes, true, true, drawDiagram);

	printf("\n");

	delete[] nullblock;

	return result;
}

//-----------------------------------------------------------------------------
// Keyset 'Seed' - hash "the quick brown fox..." using different seeds

template<bool isItSeeded, typename hashtype> bool SeedTest(pfHash<isItSeeded> hash, int32_t keycount, bool drawDiagram) {
	printf("Keyset 'Seed' - %d keys\n", keycount);

	const char* text = "The quick brown fox jumps over the lazy dog";
	const int32_t len	 = ( int32_t )strlen(text);

	//----------

	jsonifier::vector<hashtype> hashes;

	hashes.resize(keycount);

	for (int32_t i = 0; i < keycount; i++) {
		hash(text, len, i, &hashes[i]);
	}

	bool result = true;

	result &= TestHashList(hashes, true, true, drawDiagram);

	printf("\n");

	return result;
}

//-----------------------------------------------------------------------------
template<bool isItSeeded> inline bool VerificationTest(pfHash<isItSeeded> hash, const int32_t hashbits, uint32_t expected, bool verbose) {
	const int32_t hashbytes = hashbits / 8;

	uint8_t* key	= new uint8_t[256];
	uint8_t* hashes = new uint8_t[hashbytes * 256];
	uint8_t* final	= new uint8_t[hashbytes];

	memset(key, 0, 256);
	memset(hashes, 0, hashbytes * 256);
	memset(final, 0, hashbytes);

	// Hash keys of the form {0}, {0,1}, {0,1,2}... up to N=255,using 256-N as
	// the seed

	for (int32_t i = 0; i < 256; i++) {
		key[i] = ( uint8_t )i;

		hash(key, i, 256 - i, &hashes[i * hashbytes]);
	}

	// Then hash the result array

	hash(hashes, hashbytes * 256, 0, final);

	// The first four bytes of that hash, interpreted as a little-endian integer, is our
	// verification value

	uint32_t verification = (final[0] << 0) | (final[1] << 8) | (final[2] << 16) | (final[3] << 24);

	delete[] key;
	delete[] hashes;
	delete[] final;

	//----------

	if (expected != verification) {
		if (verbose)
			printf("Verification value 0x%08X : Failed! (Expected 0x%08x)\n", verification, expected);
		return false;
	} else {
		if (verbose)
			printf("Verification value 0x%08X : Passed!\n", verification);
		return true;
	}
}

//----------------------------------------------------------------------------
// Basic sanity checks -

// A hash function should not be reading outside the bounds of the key.

// Flipping a bit of a key should, with overwhelmingly high probability,
// result in a different hash.

// Hashing the same key twice should always produce the same result.

// The memory alignment of the key should not affect the hash result.
template<bool isItSeeded> inline bool SanityTest(pfHash<isItSeeded> hash, const int32_t hashbits) {
	printf("Running sanity check 1");

	random_num r(883741);

	bool result = true;

	const int32_t hashbytes = hashbits / 8;
	const int32_t reps		= 10;
	const int32_t keymax	= 256;
	const int32_t pad		= 16;
	const int32_t buflen	= keymax + pad * 3;

	uint8_t* buffer1 = new uint8_t[buflen];
	uint8_t* buffer2 = new uint8_t[buflen];

	uint8_t* hash1 = new uint8_t[hashbytes];
	uint8_t* hash2 = new uint8_t[hashbytes];

	//----------

	for (int32_t irep = 0; irep < reps; irep++) {
		if (irep % (reps / 10) == 0)
			printf(".");

		for (int32_t len = 4; len <= keymax; len++) {
			for (int32_t offset = pad; offset < pad * 2; offset++) {
				uint8_t* key1 = &buffer1[pad];
				uint8_t* key2 = &buffer2[pad + offset];

				r.rand_p(buffer1, buflen);
				r.rand_p(buffer2, buflen);

				memcpy(key2, key1, len);

				hash(key1, len, 0, hash1);

				for (int32_t bit = 0; bit < (len * 8); bit++) {
					// Flip a bit, hash the key -> we should get a different result.

					flipbit(key2, len, bit);
					hash(key2, len, 0, hash2);

					if (memcmp(hash1, hash2, hashbytes) == 0) {
						//std::cout << "FAILED TO GET A DIFFERENT RESULT: " << len << std::endl;
						result = false;
					}

					// Flip it back, hash again -> we should get the original result.

					flipbit(key2, len, bit);
					hash(key2, len, 0, hash2);

					if (memcmp(hash1, hash2, hashbytes) != 0) {
						std::cout << "FAILED TO GET THE ORIGINAL RESULT: " << len << std::endl;
						result = false;
					}
				}
			}
		}
	}

	if (result == false) {
		printf("*********FAIL*********\n");
	} else {
		printf("PASS\n");
	}

	delete[] buffer1;
	delete[] buffer2;

	delete[] hash1;
	delete[] hash2;

	return result;
}

//----------------------------------------------------------------------------
// Appending zero bytes to a key should always cause it to produce a different
// hash value
template<bool isItSeeded> inline void AppendedZeroesTest(pfHash<isItSeeded> hash, const int32_t hashbits) {
	printf("Running sanity check 2");

	random_num r(173994);

	const int32_t hashbytes = hashbits / 8;

	for (int32_t rep = 0; rep < 100; rep++) {
		if (rep % 10 == 0)
			printf(".");

		unsigned char key[256];

		memset(key, 0, sizeof(key));

		r.rand_p(key, 32);

		uint32_t h1[16];
		uint32_t h2[16];

		memset(h1, 0, hashbytes);
		memset(h2, 0, hashbytes);

		for (int32_t i = 0; i < 32; i++) {
			hash(key, 32 + i, 0, h1);

			if (memcmp(h1, h2, hashbytes) == 0) {
				throw("\n*********FAIL*********\n");
				return;
			}

			memcpy(h2, h1, hashbytes);
		}
	}

	printf("PASS\n");
}

//-----------------------------------------------------------------------------
// Generate all keys of up to N bytes containing two non-zero bytes

inline void TwoBytesKeygen(int32_t maxlen, KeyCallback& c) {
	//----------
	// Compute # of keys

	int32_t keycount = 0;

	for (int32_t i = 2; i <= maxlen; i++)
		keycount += ( int32_t )chooseK(i, 2);

	keycount *= 255 * 255;

	for (int32_t i = 2; i <= maxlen; i++)
		keycount += i * 255;

	printf("Keyset 'TwoBytes' - up-to-%d-byte keys, %d total keys\n", maxlen, keycount);

	c.reserve(keycount);

	//----------
	// Add all keys with one non-zero byte

	uint8_t key[256];

	memset(key, 0, 256);

	for (int32_t keylen = 2; keylen <= maxlen; keylen++)
		for (int32_t byteA = 0; byteA < keylen; byteA++) {
			for (int32_t valA = 1; valA <= 255; valA++) {
				key[byteA] = ( uint8_t )valA;

				c(key, keylen);
			}

			key[byteA] = 0;
		}

	//----------
	// Add all keys with two non-zero bytes

	for (int32_t keylen = 2; keylen <= maxlen; keylen++)
		for (int32_t byteA = 0; byteA < keylen - 1; byteA++)
			for (int32_t byteB = byteA + 1; byteB < keylen; byteB++) {
				for (int32_t valA = 1; valA <= 255; valA++) {
					key[byteA] = ( uint8_t )valA;

					for (int32_t valB = 1; valB <= 255; valB++) {
						key[byteB] = ( uint8_t )valB;
						c(key, keylen);
					}

					key[byteB] = 0;
				}

				key[byteA] = 0;
			}
}

//-----------------------------------------------------------------------------

template<typename hashtype> inline void DumpCollisionMap(CollisionMap<hashtype, ByteVec>& cmap) {
	typedef CollisionMap<hashtype, ByteVec> cmap_t;

	for (typename cmap_t::iterator it = cmap.begin(); it != cmap.end(); ++it) {
		const hashtype& hash = (*it).first;

		printf("Hash - ");
		printbytes(&hash, sizeof(hashtype));
		printf("\n");

		jsonifier::vector<ByteVec>& keys = (*it).second;

		for (int32_t i = 0; i < ( int32_t )keys.size(); i++) {
			ByteVec& key = keys[i];

			printf("Key  - ");
			printbytes(&key[0], ( int32_t )key.size());
			printf("\n");
		}
		printf("\n");
	}
}

// test code
template<bool isItSeeded> inline void ReportCollisions(pfHash<isItSeeded> hash) {
	printf("Hashing keyset\n");

	jsonifier::vector<uint128_t> hashes;

	HashCallback<isItSeeded, uint128_t> c(hash, hashes);

	TwoBytesKeygen(20, c);

	printf("%d hashes\n", ( int32_t )hashes.size());

	printf("Finding collisions\n");

	Hashset<uint128_t> collisions;

	FindCollisions(hashes, collisions, 1000);

	printf("%d collisions\n", ( int32_t )collisions.size());

	printf("Mapping collisions\n");

	CollisionMap<uint128_t, ByteVec> cmap;

	CollisionCallback<isItSeeded, uint128_t> c2(hash, collisions, cmap);

	TwoBytesKeygen(20, c2);

	printf("Dumping collisions\n");

	DumpCollisionMap(cmap);
}
