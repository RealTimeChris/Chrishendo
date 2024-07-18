#include "../Tests.hpp"

//----------------------------------------------------------------------------
constexpr std::array<uint64_t, 38> lengthsToIterate{ 1, 2, 4, 8, 16, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216,
	224, 232, 240, 248, 256, 264, 512, 1024, 2048 };

template<bool isItSeeded, typename hashtype, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor> void test(const auto hash, const auto* info) {
	static constexpr int32_t hashbits = sizeof(hashtype) * 8;
	std::cout << "-------------------------------------------------------------------------------" << std::endl;
	std::cout << "--- Testing " << libraryName.data() << " (" << info->desc.data() << ")" << std::endl << std::endl;

	{
		std::cout << "[[[ Speed Tests ]]] - " << libraryName.data() << std::endl;
		TinySpeedTest<false, lengthsToIterate, libraryName, benchmarkColor>(hashfunc<false, hashtype>(info->hash), sizeof(hashtype), info->verification);

		BulkSpeedTest<false, libraryName, benchmarkColor>(info->hash, info->verification);
	}
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
template<bool isItSeeded, bnch_swt::string_literal libraryName, bnch_swt::string_literal benchmarkColor>
void testHash(const auto pInfo) {
	g_hashUnderTest<isItSeeded> = &pInfo;

	if (pInfo.hashbits == 32) {
		test<isItSeeded, uint32_t, libraryName, benchmarkColor>(VerifyHash<false>, &pInfo);
	}
	else if (pInfo.hashbits == 64) {
		test<isItSeeded, uint64_t, libraryName, benchmarkColor>(pInfo.hash, &pInfo);
	}
	else if (pInfo.hashbits == 128) {
		test<isItSeeded, uint128_t, libraryName, benchmarkColor>(pInfo.hash, &pInfo);
	}
	else if (pInfo.hashbits == 256) {
		test<isItSeeded, uint256_t, libraryName, benchmarkColor>(pInfo.hash, &pInfo);
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
	testHash<false, "XXH3_64bitsTest", "cyan">(HashInfo<false>{ XXH3_64bitsTest<false>, 0x5A116D6B, "A win hash algo by xxHash", 64 });
	testHash<false, "Chrishendo", "turquoise">(HashInfo<false>{ chrishendoTestNew<false>, 0x814F3144, "A general purpose hybrid hasher", 64 });
	//testHash<"chrishendoNewCt", "turquoise">(HashInfo<true>{ chrishendoTestNewCt<true>, 0x814F3144, "A general purpose hybrid hasher", 64 });
	bnch_swt::benchmark_suite<"Speed Test">::writeJsonData("./BenchmarkData.json");
	bnch_swt::benchmark_suite<"Speed Test">::printResults();
	executePythonScript(static_cast<std::string>(basePath) + "/Tests/GenerateGraphs.py", getCurrentWorkingDirectory() + "/BenchmarkData.json",
		static_cast<std::string>(basePath) + "/Graphs");
	bnch_swt::benchmark_suite<"Speed Test">::writeMarkdownToFile(static_cast<std::string>(basePath) + "/BenchmarkData.md",
		"https://github.com/RealTimeChris/Chrishendo/blob/main/Graphs/");
	return 0;
}
