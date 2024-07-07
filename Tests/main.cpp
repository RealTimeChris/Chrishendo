#include "Tests.hpp"

//----------------------------------------------------------------------------
constexpr std::array<uint64_t, 38> lengthsToIterate{ 1, 2, 4, 8, 16, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216,
	224, 232, 240, 248, 256, 264, 512, 1024, 2048 };

template<typename hashtype, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
void test(const auto hash, const auto* info) {
	static constexpr int32_t hashbits = sizeof(hashtype) * 8;
		
		
		/// PASSED TESTS:
		
	/*
		{
			std::cout << "[[[ Keyset 'Seed' Tests ]]] - " << libraryName.data() << std::endl;

			bool result = true;
			bool drawDiagram = false;
			result &= SeedTest<isItSeeded, hashtype>(hash, 1000000, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			}
			else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			std::cout << "[[[ Keyset 'Text' Tests ]]] - " << libraryName.data() << std::endl;

			bool result		 = true;
			bool drawDiagram = false;

			const char* alnum = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

			result &= TextKeyTest<isItSeeded>(hash, "Foo", alnum, 4, "Bar", drawDiagram);
			result &= TextKeyTest<isItSeeded>(hash, "FooBar", alnum, 4, "", drawDiagram);
			result &= TextKeyTest<isItSeeded>(hash, "", alnum, 4, "FooBar", drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}*/
		
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	std::cout << "--- Testing " << libraryName.data() << " (" << info->desc.data() << ")" << std::endl << std::endl;

	{
		std::cout << "[[[ Speed Tests ]]] - " << libraryName.data() << std::endl;
		TinySpeedTest<false, lengthsToIterate, libraryName, benchmarkColor>(hashfunc<false, hashtype>(info->hash), sizeof(hashtype), info->verification);

		BulkSpeedTest<false, libraryName, benchmarkColor>(info->hash, info->verification);
	}
		
	/*
	{
		std::cout << "[[[ Sanity Tests ]]] - " << libraryName.data() << std::endl;

		VerificationTest<isItSeeded>(hash, hashbits, info->verification, true);
		SanityTest<isItSeeded>(hash, hashbits);
		AppendedZeroesTest<isItSeeded>(hash, hashbits);
	}
		
		
		/*
		/// PASSED TESTS:
		
	{
		std::cout << "[[[ Keyset 'Cyclic' Tests ]]] - " << libraryName.data() << std::endl;

		bool result		 = true;
		bool drawDiagram = false;

		result &= CyclicKeyTest<hashtype>(hash, sizeof(hashtype) + 0, 8, 10000000, drawDiagram);
		result &= CyclicKeyTest<hashtype>(hash, sizeof(hashtype) + 1, 8, 10000000, drawDiagram);
		result &= CyclicKeyTest<hashtype>(hash, sizeof(hashtype) + 2, 8, 10000000, drawDiagram);
		result &= CyclicKeyTest<hashtype>(hash, sizeof(hashtype) + 3, 8, 10000000, drawDiagram);
		result &= CyclicKeyTest<hashtype>(hash, sizeof(hashtype) + 4, 8, 10000000, drawDiagram);

		if (!result) {
			std::cout << "*********FAIL*********" << std::endl;
		} else {
			std::cout << "*********SUCCESS*********" << std::endl;
		}		
	}

	

		{
			std::cout << "[[[ Differential Distribution Tests ]]] - " << libraryName.data() << std::endl;

			bool result = true;

			result &= DiffDistTest2<isItSeeded, uint64_t, hashtype>(hash);
			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

	{
		std::cout << "[[[ Avalanche Tests ]]] - " << libraryName.data() << std::endl;

		bool result = true;

		result &= AvalancheTest<Blob<32>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<40>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<48>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<56>, hashtype>(hash, 300000);

		result &= AvalancheTest<Blob<64>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<72>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<80>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<88>, hashtype>(hash, 300000);

		result &= AvalancheTest<Blob<96>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<104>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<112>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<120>, hashtype>(hash, 300000);

		result &= AvalancheTest<Blob<128>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<136>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<144>, hashtype>(hash, 300000);
		result &= AvalancheTest<Blob<152>, hashtype>(hash, 300000);
		if (!result) {
			std::cout << "*********FAIL*********" << std::endl;
		} else {
			std::cout << "*********SUCCESS*********" << std::endl;
		}		
	}

	{
			std::cout << "[[[ Bit Independence Criteria ]]] - " << libraryName.data() << std::endl;

			bool result = true;

			//result &= BicTest<uint64_t,hashtype>(hash,2000000);
			BicTest3<isItSeeded, Blob<88>, hashtype>(hash, 2000000);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

	/// FAILED TESTS:
/*

		{
			std::cout << "[[[ Keyset 'Window' Tests ]]] - " << libraryName.data() << std::endl;

			bool result = true;
			bool testCollision = true;
			bool testDistribution = false;
			bool drawDiagram = false;

			result &= WindowedKeyTest<isItSeeded, Blob<hashbits * 2>, hashtype >(hash, 20, testCollision, testDistribution, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			}
			else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}

		}

		{
			std::cout << "[[[ Keyset 'Combination Hi-Lo' Tests ]]] - " << libraryName.data() << std::endl;

			bool result = true;
			bool drawDiagram = false;

			uint32_t blocks[] =
			{
			  0x00000000,

			  0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007,

			  0x80000000, 0x40000000, 0xC0000000, 0x20000000, 0xA0000000, 0x60000000, 0xE0000000
			};

			result &= CombinationKeyTest<isItSeeded, hashtype>(hash, 6, blocks, sizeof(blocks) / sizeof(uint32_t), true, true, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			}
			else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}

		}

		{
			std::cout << "[[[ Keyset 'Combination 0x0000001' Tests ]]] - " << libraryName.data() << std::endl;

			bool result = true;
			bool drawDiagram = false;

			uint32_t blocks[] =
			{
			  0x00000000,

			  0x00000001,
			};

			result &= CombinationKeyTest<isItSeeded, hashtype>(hash, 20, blocks, sizeof(blocks) / sizeof(uint32_t), true, true, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			}
			else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			std::cout << "[[[ Keyset 'Sparse' Tests ]]] - " << libraryName.data() << std::endl;

			bool result		 = true;
			bool drawDiagram = false;

			result &= SparseKeyTest<isItSeeded, 32, hashtype>(hash, 6, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 40, hashtype>(hash, 6, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 48, hashtype>(hash, 5, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 56, hashtype>(hash, 5, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 64, hashtype>(hash, 5, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 96, hashtype>(hash, 4, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 256, hashtype>(hash, 3, true, true, true, drawDiagram);
			result &= SparseKeyTest<isItSeeded, 1024, hashtype>(hash, 2, true, true, true, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			std::cout << "[[[ Keyset 'Zeroes' Tests ]]] - " << libraryName.data() << std::endl;

			bool result		 = true;
			bool drawDiagram = false;

			result &= ZeroKeyTest<hashtype>(hash, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			// This one breaks lookup3, surprisingly

			std::cout << "[[[ Keyset 'Combination Lowbits' Tests ]]] - " << libraryName.data() << std::endl;

			bool result		 = true;
			bool drawDiagram = false;

			uint32_t blocks[] = {
				0x00000000,

				0x00000001,
				0x00000002,
				0x00000003,
				0x00000004,
				0x00000005,
				0x00000006,
				0x00000007,
			};

			result &= CombinationKeyTest<isItSeeded, hashtype>(hash, 8, blocks, sizeof(blocks) / sizeof(uint32_t), true, true, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			std::cout << "[[[ Keyset 'Combination Highbits' Tests ]]] - " << libraryName.data() << std::endl;

			bool result		 = true;
			bool drawDiagram = false;

			uint32_t blocks[] = { 0x00000000,

				0x20000000, 0x40000000, 0x60000000, 0x80000000, 0xA0000000, 0xC0000000, 0xE0000000 };

			result &= CombinationKeyTest<isItSeeded, hashtype>(hash, 8, blocks, sizeof(blocks) / sizeof(uint32_t), true, true, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			std::cout << "[[[ Keyset 'Combination 0x8000000' Tests ]]]: " << libraryName.data() << std::endl;

			bool result = true;
			bool drawDiagram = false;

			uint32_t blocks[] = {
				0x00000000,

				0x80000000,
			};

			result &= CombinationKeyTest<isItSeeded, hashtype>(hash, 20, blocks, sizeof(blocks) / sizeof(uint32_t), true, true, drawDiagram);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			}
			else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}
		{
			std::cout << "[[[ Differential Tests ]]] - " << libraryName.data() << std::endl;

			bool result			= true;
			bool dumpCollisions = false;

			result &= DiffTest<Blob<64>, hashtype>(hash, 5, 1000, dumpCollisions);
			result &= DiffTest<Blob<128>, hashtype>(hash, 4, 1000, dumpCollisions);
			result &= DiffTest<Blob<256>, hashtype>(hash, 3, 1000, dumpCollisions);

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}

		{
			std::cout << "[[[ Keyset 'TwoBytes' Tests ]]] - " << libraryName.data() << std::endl;

			bool result		 = true;
			bool drawDiagram = false;

			for (int32_t i = 4; i <= 20; i += 4) {
				result &= TwoBytesTest2<isItSeeded, hashtype>(hash, i, drawDiagram);
			}

			if (!result) {
				std::cout << "*********FAIL*********" << std::endl;
			} else {
				std::cout << "*********SUCCESS*********" << std::endl;
			}
		}
		*/
	
}

uint32_t g_inputVCode  = 1;
uint32_t g_outputVCode = 1;
uint32_t g_resultVCode = 1;
template<bool isItSeeded> const HashInfo<isItSeeded>* g_hashUnderTest = NULL;

template<bool isItSeeded> void VerifyHash(const void* key, int32_t len, uint32_t seed, void* out) {
	g_inputVCode = MurmurOAAT(key, len, g_inputVCode);
	g_inputVCode = MurmurOAAT(&seed, sizeof(uint32_t), g_inputVCode);

	g_hashUnderTest<isItSeeded>->hash(key, len, seed, out);

	g_outputVCode = MurmurOAAT(out, g_hashUnderTest<isItSeeded>->hashbits / 8, g_outputVCode);
}

//-----------------------------------------------------------------------------
template<bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
void testHash(const auto pInfo) {
	g_hashUnderTest<true>	   = &pInfo;

	if (pInfo.hashbits == 32) {
		test<uint32_t, libraryName, benchmarkColor>(VerifyHash<false>, &pInfo);
	}
	else if (pInfo.hashbits == 64) {
		test<uint64_t, libraryName, benchmarkColor>(pInfo.hash, &pInfo);
	}
	else if (pInfo.hashbits == 128) {
		test<uint128_t, libraryName, benchmarkColor>(pInfo.hash, &pInfo);
	}
	else if (pInfo.hashbits == 256) {
		test<uint256_t, libraryName, benchmarkColor>(pInfo.hash, &pInfo);
	}
	else {
		std::cout << "Invalid hash bit width " << pInfo.hashbits << " for hash " << libraryName.data() << std::endl;
	}
}

inline std::string getCurrentWorkingDirectory() {
	try {
		return std::filesystem::current_path().string();
	} catch (const std::filesystem::filesystem_error& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return "";
	}
}

inline void executePythonScript(const std::string& scriptPath, const std::string& argument01, const std::string& argument02) {
#if defined(CHRISHENDO_WIN)
	static constexpr std::string_view pythonName{ "python " };
#else
	static constexpr std::string_view pythonName{ "python3 " };
#endif
	std::string command = static_cast<std::string>(pythonName) + scriptPath + " " + argument01 + " " + argument02;
	int32_t result		= system(command.c_str());
	if (result != 0) {
		std::cout << "Error: Failed to execute Python script. Command exited with code " << result << std::endl;
	}
}
static constexpr std::string_view basePath{ BASE_PATH };
int32_t main() {
	setAffinity((1 << 2));
	testHash<"XXH3_64bitsTest", "cyan">(HashInfo<true>{ XXH3_64bitsTest<true>, 0x5A116D6B, "A win hash algo by xxHash", 64 });
	testHash<"chrishendoNew", "turquoise">(HashInfo<true>{ chrishendoTestNew<true>, 0x814F3144, "A general purpose hybrid hasher", 64 });
	//testHash<"chrishendoNewCt", "turquoise">(HashInfo<true>{ chrishendoTestNewCt<true>, 0x814F3144, "A general purpose hybrid hasher", 64 });
	bnch_swt::benchmark_suite<"Speed Test">::writeJsonData("./BenchmarkData.json");
	bnch_swt::benchmark_suite<"Speed Test">::printResults();
	executePythonScript(static_cast<std::string>(basePath) + "/Tests/GenerateGraphs.py", getCurrentWorkingDirectory() + "/BenchmarkData.json",
		static_cast<std::string>(basePath) + "/Graphs");
	bnch_swt::benchmark_suite<"Speed Test">::writeMarkdownToFile(static_cast<std::string>(basePath) + "/BenchmarkData.md",
		"https://github.com/RealTimeChris/Chrishendo/blob/main/Graphs/");
	return 0;
}
