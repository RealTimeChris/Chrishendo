# Chrishendo
 A new hashing algorithm. For hashing small-medium strings, very rapidly.

 Hello everyone. So it seems as though I may have discovered a hashing algorithm that is VERY FAST for hashing strings of roughly 4-2048+ bytes. Look @ [these](https://github.com/RealTimeChris/chrishendo/blob/main/BenchmarkData.md) benchmark results.
 If any of you would be interested in contributing to the body of quality/speed benchmarks that would be very welcome. Anyways - enjoy!

## Overview:

The new hashing algorithm is designed to balance speed and collision resistance by processing data in chunks and utilizing efficient bit manipulation techniques. The algorithm focuses on strings of 3 to 256 bytes, which are common in many data processing and storage systems. This focus allows the algorithm to optimize for typical use cases, ensuring high performance and robustness.

One of the key innovations of this algorithm is the use of an array of function pointers, where each function is tailored to handle a specific length of the input string. By indexing directly into this array based on the string's length, the algorithm avoids the branching operations that typically degrade performance in other hashing methods. This design ensures that the hashing process remains efficient and consistent, regardless of the input length within the targeted range.


