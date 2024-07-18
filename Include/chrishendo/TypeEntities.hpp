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

#include <chrishendo/Config.hpp>
#include <type_traits>
#include <optional>
#include <iostream>
#include <sstream>
#include <variant>
#include <chrono>
#include <vector>
#include <bitset>
#include <array>

namespace chrishendo_internal {

	template<typename value_type, typename... value_types> struct collect_first_type {
		using type = value_type;
	};

	template<typename... value_type> using unwrap_t = std::remove_cvref_t<typename collect_first_type<value_type...>::type>;

	template<template<uint64_t> typename hash_wrapper, std::size_t... indices> constexpr auto generateArrayOfFunctionPtrs(std::index_sequence<indices...>) {
		return std::array<decltype(&hash_wrapper<0>::op), sizeof...(indices)>{ &hash_wrapper<indices>::op... };
	}

	namespace concepts {

		template<typename value_type>
		concept chrishendo_simd_int_512_type = std::is_same_v<chrishendo_simd_int_512, std::remove_cvref_t<value_type>>;

		template<typename value_type>
		concept chrishendo_simd_int_256_type = std::is_same_v<chrishendo_simd_int_256, std::remove_cvref_t<value_type>>;

		template<typename value_type>
		concept chrishendo_simd_int_128_type = std::is_same_v<chrishendo_simd_int_128, std::remove_cvref_t<value_type>>;

		template<typename value_type>
		concept chrishendo_simd_int_type = std::is_same_v<chrishendo_simd_int_t, std::remove_cvref_t<value_type>>;

		template<typename value_type>
		concept bool_t = std::is_same_v<chrishendo_internal::unwrap_t<value_type>, bool> || std::same_as<chrishendo_internal::unwrap_t<value_type>, std::vector<bool>::reference> ||
			std::same_as<chrishendo_internal::unwrap_t<value_type>, std::vector<bool>::const_reference>;

		template<typename value_type>
		concept signed_type = std::signed_integral<chrishendo_internal::unwrap_t<value_type>> && !bool_t<value_type>;

		template<typename value_type>
		concept unsigned_type = std::unsigned_integral<chrishendo_internal::unwrap_t<value_type>> && !bool_t<value_type>;

		template<typename value_type>
		concept uint8_type = std::is_same_v<uint8_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept uint16_type = std::is_same_v<uint16_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept uint32_type = std::is_same_v<uint32_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept uint64_type = std::is_same_v<uint64_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept int8_type = std::is_same_v<int8_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept int16_type = std::is_same_v<int16_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept int32_type = std::is_same_v<int32_t, chrishendo_internal::unwrap_t<value_type>>;

		template<typename value_type>
		concept int64_type = std::is_same_v<int64_t, chrishendo_internal::unwrap_t<value_type>>;

	}

}
