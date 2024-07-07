// The code in this header was sampled from the SMHasher test suite: https://github.com/aappleby/smhasher
#pragma once

#include "Types.hpp"
#include <algorithm>
#include <BnchSwt/BenchmarkSuite.hpp>
#include <inttypes.h>

inline double CalcMean(jsonifier::vector<double>& v) {
	double mean = 0;

	for (int32_t i = 0; i < ( int32_t )v.size(); i++) {
		mean += v[i];
	}

	mean /= double(v.size());

	return mean;
}

inline double CalcMean(jsonifier::vector<double>& v, int32_t a, int32_t b) {
	double mean = 0;

	for (int32_t i = a; i <= b; i++) {
		mean += v[i];
	}

	mean /= (b - a + 1);

	return mean;
}

inline double CalcStdv(jsonifier::vector<double>& v, int32_t a, int32_t b) {
	double mean = CalcMean(v, a, b);

	double stdv = 0;

	for (int32_t i = a; i <= b; i++) {
		double x = v[i] - mean;

		stdv += x * x;
	}

	stdv = sqrt(stdv / (b - a + 1));

	return stdv;
}

inline bool ContainsOutlier(jsonifier::vector<double>& v, size_t len) {
	double mean = 0;

	for (size_t i = 0; i < len; i++) {
		mean += v[i];
	}

	mean /= double(len);

	double stdv = 0;

	for (size_t i = 0; i < len; i++) {
		double x = v[i] - mean;
		stdv += x * x;
	}

	stdv = sqrt(stdv / double(len));

	double cutoff = mean + stdv * 3;

	return v[len - 1] > cutoff;
}

inline void FilterOutliers(jsonifier::vector<double>& v) {
	std::sort(v.begin(), v.end());

	size_t len = 0;

	for (size_t x = 0x40000000; x; x = x >> 1) {
		if ((len | x) >= v.size())
			continue;

		if (!ContainsOutlier(v, len | x)) {
			len |= x;
		}
	}

	v.resize(len);
}

inline void FilterOutliers2(jsonifier::vector<double>& v) {
	std::sort(v.begin(), v.end());

	int32_t a = 0;
	int32_t b = ( int32_t )(v.size() - 1);

	for (int32_t i = 0; i < 10; i++) {

		double mean = CalcMean(v, a, b);
		double stdv = CalcStdv(v, a, b);

		double cutA = mean - stdv * 3;
		double cutB = mean + stdv * 3;

		while ((a < b) && (v[a] < cutA))
			a++;
		while ((b > a) && (v[b] > cutB))
			b--;
	}

	jsonifier::vector<double> v2;

	v2.insert(v2.begin(), v.begin() + a, v.begin() + b + 1);

	v.swap(v2);
}

template<bool isItSeeded, uint64_t trials, bnch_swt::string_literal benchmarkName, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
inline double SpeedTest(pfHash<isItSeeded> hash, uint32_t seed, const int32_t blocksize, const int32_t align) {
	random_num r(seed);

	uint8_t* buf = new uint8_t[blocksize + 512];

	uint64_t t1 = reinterpret_cast<uint64_t>(buf);

	t1 = (t1 + 255) & BIG_CONSTANT(0xFFFFFFFFFFFFFF00);
	t1 += align;

	uint8_t* block = reinterpret_cast<uint8_t*>(t1);

	r.rand_p(block, blocksize);
	uint64_t itrial{};
	double t = bnch_swt::benchmark_suite<"Speed Test">::benchmarkCycles<benchmarkName, libraryName, benchmarkColor, trials>([&] {
		uint32_t temp[16]{};
		hash(block, blocksize, itrial, temp);
		bnch_swt::doNotOptimizeAway(temp);
		++itrial;
	}).resultValue;

	delete[] buf;

	return t;	
}

double roundToDecimalPlaces(double value, int decimalPlaces) {
	double scale = std::pow(10.0, decimalPlaces);
	return std::round(value * scale) / scale;
}

template<uint64_t currentIndex = 0, uint64_t maxIndex, bool isItSeeded, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
inline void BulkSpeedTestHelper(pfHash<isItSeeded> hash, uint32_t seed) {
	if constexpr (currentIndex < maxIndex) {
		static constexpr int32_t trials	   = 2999;
		static constexpr int32_t blocksize = 256 * 1024;
		double cycles =
			SpeedTest<isItSeeded, trials, bnch_swt::joinLiterals<"Bulk Speed Test - Alignment: ", bnch_swt::toStringLiteral<currentIndex>()>(), libraryName, benchmarkColor>(hash,
				seed, blocksize, currentIndex);

		double bestbpc		= double(blocksize) / cycles;
		double cpuFrequency		  = bnch_swt::getCpuFrequency();
		double bestbps		= (bestbpc * (cpuFrequency * 1e06) / 1048576.0);
		std::cout << "Alignment " << currentIndex << " - " << roundToDecimalPlaces(bestbpc, 2) << " bytes/cycle - " << roundToDecimalPlaces(bestbps, 2) << "MiB/sec @ "
				  << roundToDecimalPlaces(cpuFrequency / 1000, 2) << "ghz." << std::endl;
		return BulkSpeedTestHelper<currentIndex + 1, maxIndex, isItSeeded, libraryName, benchmarkColor>(hash, seed);
	} else {
		return;
	}
}

template<bool isItSeeded, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
inline void BulkSpeedTest(pfHash<isItSeeded> hash, uint32_t seed) {
	static constexpr int32_t trials	   = 2999;
	static constexpr int32_t blocksize = 256 * 1024;

	std::cout << "Bulk speed test - " << blocksize << "-byte keys\n " << std::endl;

	return BulkSpeedTestHelper<0, 8, isItSeeded, libraryName, benchmarkColor>(hash, seed);
}

template<uint64_t currentIndex, auto lengthsToIterate, bool isItSeeded, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
inline void TinySpeedTestHelper(pfHash<isItSeeded> hash, int32_t hashSize, uint32_t seed) {
	if constexpr (currentIndex < lengthsToIterate.size()) {
		static constexpr int32_t trials = 9999;


		double cycles = SpeedTest<isItSeeded, trials,
			bnch_swt::joinLiterals<bnch_swt::joinLiterals<"Small Key Speed Test: ", bnch_swt::toStringLiteral<lengthsToIterate[currentIndex]>()>(), " bytes">(), libraryName,
			benchmarkColor>(hash, seed, lengthsToIterate[currentIndex], 0);
		double bestbpc		= double(lengthsToIterate[currentIndex]) / cycles;
		double cpuFrequency = bnch_swt::getCpuFrequency();
		double bestbps		= (bestbpc * (cpuFrequency * 1e06) / 1048576.0);
		std::cout << "Small key speed test - " << lengthsToIterate[currentIndex] << " byte keys - " << roundToDecimalPlaces(bestbpc, 2) << " bytes/cycle - "
				  << roundToDecimalPlaces(bestbps, 2) << "MiB/sec @ " << roundToDecimalPlaces(cpuFrequency / 1000, 2) << "ghz - " << cycles << " cycles/hash" << std::endl;
		return TinySpeedTestHelper<currentIndex + 1, lengthsToIterate, isItSeeded, libraryName, benchmarkColor>(hash, hashSize, seed);
	}
}

//-----------------------------------------------------------------------------
template<bool isItSeeded, auto lengthsToIterate, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
inline void TinySpeedTest(pfHash<isItSeeded> hash, int32_t hashSize, uint32_t seed) {
	
	TinySpeedTestHelper<0, lengthsToIterate, isItSeeded, libraryName, benchmarkColor>(hash, hashSize, seed);
}

//-----------------------------------------------------------------------------
