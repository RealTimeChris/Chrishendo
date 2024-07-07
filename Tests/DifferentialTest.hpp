// The code in this header was sampled from the SMHasher test suite: https://github.com/aappleby/smhasher
//-----------------------------------------------------------------------------
// Differential collision & distribution tests - generate a bunch of random_num keys,
// see what happens to the hash value when we flip a few bits of the key.
#pragma once

#include "Types.hpp"
#include "Stats.hpp"// for chooseUpToK
#include "KeysetTest.hpp"// for SparseKeygenRecurse
#include "Random.hpp"

#include <vector>
#include <algorithm>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Sort through the differentials, ignoring collisions that only occured once
// (these could be false positives). If we find collisions of 3 or more, the
// differential test fails.

template<class keytype> bool ProcessDifferentials(jsonifier::vector<keytype>& diffs, int32_t reps, bool dumpCollisions) {
	std::sort(diffs.begin(), diffs.end());

	int32_t count  = 1;
	int32_t ignore = 0;

	bool result = true;

	if (diffs.size()) {
		keytype kp = diffs[0];

		for (int32_t i = 1; i < ( int32_t )diffs.size(); i++) {
			if (diffs[i] == kp) {
				count++;
				continue;
			} else {
				if (count > 1) {
					result = false;

					double pct = 100 * (double(count) / double(reps));

					if (dumpCollisions) {
						printbits(( unsigned char* )&kp, sizeof(kp));
						printf(" - %4.2f%%\n", pct);
					}
				} else {
					ignore++;
				}

				kp	  = diffs[i];
				count = 1;
			}
		}

		if (count > 1) {
			double pct = 100 * (double(count) / double(reps));

			if (dumpCollisions) {
				printbits(( unsigned char* )&kp, sizeof(kp));
				printf(" - %4.2f%%\n", pct);
			}
		} else {
			ignore++;
		}
	}

	printf("%d total collisions, of which %d single collisions were ignored", ( int32_t )diffs.size(), ignore);

	if (result == false) {
		printf(" !!!!! ");
	}

	printf("\n");
	printf("\n");

	return result;
}

//-----------------------------------------------------------------------------
// Check all possible keybits-choose-N differentials for collisions, report
// ones that occur significantly more often than expected.

// Random collisions can happen with probability 1 in 2^32 - if we do more than
// 2^32 tests, we'll probably see some spurious random_num collisions, so don't report
// them.

template<bool isItSeeded, typename keytype, typename hashtype>
void DiffTestRecurse(pfHash<isItSeeded> hash, keytype& k1, keytype& k2, hashtype& h1, hashtype& h2, int32_t start, int32_t bitsleft, jsonifier::vector<keytype>& diffs) {
	const int32_t bits = sizeof(keytype) * 8;

	for (int32_t i = start; i < bits; i++) {
		flipbit(&k2, sizeof(k2), i);
		bitsleft--;

		hash(&k2, sizeof(k2), 0, &h2);

		if (h1 == h2) {
			diffs.push_back(k1 ^ k2);
		}

		if (bitsleft) {
			DiffTestRecurse(hash, k1, k2, h1, h2, i + 1, bitsleft, diffs);
		}

		flipbit(&k2, sizeof(k2), i);
		bitsleft++;
	}
}

//----------

template<bool isItSeeded, typename keytype, typename hashtype> bool DiffTest(pfHash<isItSeeded> hash, int32_t diffbits, int32_t reps, bool dumpCollisions) {
	const int32_t keybits  = sizeof(keytype) * 8;
	const int32_t hashbits = sizeof(hashtype) * 8;

	double diffcount = chooseUpToK(keybits, diffbits);
	double testcount = (diffcount * double(reps));
	double expected	 = testcount / pow(2.0, double(hashbits));

	random_num r(100);

	jsonifier::vector<keytype> diffs;

	keytype k1, k2;
	hashtype h1, h2;

	printf("Testing %0.f up-to-%d-bit differentials in %d-bit keys -> %d bit hashes.\n", diffcount, diffbits, keybits, hashbits);
	printf("%d reps, %0.f total tests, expecting %2.2f random_num collisions", reps, testcount, expected);

	for (int32_t i = 0; i < reps; i++) {
		if (i % (reps / 10) == 0)
			printf(".");

		r.rand_p(&k1, sizeof(keytype));
		k2 = k1;

		hash(&k1, sizeof(k1), 0, ( uint32_t* )&h1);

		DiffTestRecurse<keytype, hashtype>(hash, k1, k2, h1, h2, 0, diffbits, diffs);
	}
	printf("\n");

	bool result = true;

	result &= ProcessDifferentials(diffs, reps, dumpCollisions);

	return result;
}

//-----------------------------------------------------------------------------
// Differential distribution test - for each N-bit input differential, generate
// a large set of differential key pairs, hash them, and test the output
// differentials using our distribution test code.

// This is a very hard test to pass - even if the hash values are well-distributed,
// the differences between hash values may not be. It's also not entirely relevant
// for testing hash functions, but it's still interesting.

// This test is a _lot_ of work, as it's essentially a full keyset test for
// each of a potentially huge number of input differentials. To speed things
// along, we do only a few distribution tests per keyset instead of the full
// grid.

// #TODO - put diagram drawing back on

template<bool isItSeeded, typename keytype, typename hashtype> void DiffDistTest(pfHash<isItSeeded> hash, const int32_t diffbits, int32_t trials, double& worst, double& avg) {
	jsonifier::vector<keytype> keys(trials);
	jsonifier::vector<hashtype> A(trials), B(trials);

	for (int32_t i = 0; i < trials; i++) {
		rand_p(&keys[i], sizeof(keytype));

		hash(&keys[i], sizeof(keytype), 0, ( uint32_t* )&A[i]);
	}

	//----------

	jsonifier::vector<keytype> diffs;

	keytype temp(0);

	SparseKeygenRecurse<keytype>(0, diffbits, true, temp, diffs);

	//----------

	worst = 0;
	avg	  = 0;

	hashtype h2;

	for (size_t j = 0; j < diffs.size(); j++) {
		keytype& d = diffs[j];

		for (int32_t i = 0; i < trials; i++) {
			keytype k2 = keys[i] ^ d;

			hash(&k2, sizeof(k2), 0, &h2);

			B[i] = A[i] ^ h2;
		}

		double dworst, davg;

		TestDistributionFast(B, dworst, davg);

		avg += davg;
		worst = (dworst > worst) ? dworst : worst;
	}

	avg /= double(diffs.size());
}

//-----------------------------------------------------------------------------
// Simpler differential-distribution test - for all 1-bit differentials,
// generate random_num key pairs and run full distribution/collision tests on the
// hash differentials

template<bool isItSeeded, typename keytype, typename hashtype> bool DiffDistTest2(pfHash<isItSeeded> hash) {
	random_num r(857374);

	int32_t keybits		   = sizeof(keytype) * 8;
	const int32_t keycount = 256 * 256 * 32;
	keytype k;

	jsonifier::vector<hashtype> hashes(keycount);
	hashtype h1, h2;

	bool result = true;

	for (int32_t keybit = 0; keybit < keybits; keybit++) {
		printf("Testing bit %d\n", keybit);

		for (int32_t i = 0; i < keycount; i++) {
			r.rand_p(&k, sizeof(keytype));

			hash(&k, sizeof(keytype), 0, &h1);
			flipbit(&k, sizeof(keytype), keybit);
			hash(&k, sizeof(keytype), 0, &h2);

			hashes[i] = h1 ^ h2;
		}

		result &= TestHashList<hashtype>(hashes, true, true, false);
		printf("\n");
	}

	return result;
}

//----------------------------------------------------------------------------
