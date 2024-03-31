#pragma once

#include <algorithm>

template <size_t N>
struct compile_time_string {
	constexpr compile_time_string(
		const char(&str)[N])
	{
		std::copy_n(str, N, value);
	}
	char value[N]{};
};

template <compile_time_string cts>
struct cts_wrapper {
	static constexpr compile_time_string value{ cts };
};

template <compile_time_string cts>
consteval auto operator""_cts()
{
	return cts;
}

#define CTS_STR(x) cts_wrapper<x##_cts>
#define CTS_VALUE(x)  (std::decay_t<decltype(x)>::value.value)