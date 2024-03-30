#pragma once

#define GET_NTH_ARG(                                                                        \
    _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, _16,         \
    _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32,         \
    _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,         \
    _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, n, ...) n

#define GET_ARG_COUNT(...) GET_NTH_ARG(__VA_ARGS__,                     \
        64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, \
        48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
        32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, \
        16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1)


#define CONCAT(a,b) a##b
#define DELAY_CONCAT(a,b) CONCAT(a,b)

#define PAIR(x) PARE x
#define PARE(...) __VA_ARGS__,


#define FOR_EACH_1(func,func2,structName, i, pair)       DELAY_CONCAT(func,func2)(i,structName, PAIR(pair));
#define FOR_EACH_2(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_1(func,func2,structName, i + 1, __VA_ARGS__)
#define FOR_EACH_3(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_2(func,func2,structName, i + 1, __VA_ARGS__)
#define FOR_EACH_4(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_3(func,func2,structName, i + 1, __VA_ARGS__)
#define FOR_EACH_5(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_4(func,func2,structName, i + 1, __VA_ARGS__)
#define FOR_EACH_6(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_5(func,func2,structName, i + 1, __VA_ARGS__)
#define FOR_EACH_7(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_6(func,func2,structName, i + 1, __VA_ARGS__)
#define FOR_EACH_8(func,func2,structName, i, pair, ...)  DELAY_CONCAT(func,func2)(i,structName, PAIR(pair)); FOR_EACH_7(func,func2,structName, i + 1, __VA_ARGS__)

#define STRUCT(structName,...) struct structName\
{\
	static constexpr size_t FieldLength = GET_ARG_COUNT(__VA_ARGS__);\
	static constexpr cts_wrapper<DELAY_CONCAT(#structName,_cts)> Name{};\
	template<int Index>\
	struct Field;\
	template<typename name>\
	struct FieldByName;\
	DELAY_CONCAT(FOR_EACH_, GET_ARG_COUNT(__VA_ARGS__)) (DEFINE,_FIELD_DELAY,structName,0,__VA_ARGS__) \
};

#define DEFINE_FIELD_DELAY(index,structName,...)  DEFINE_FIELD(index,structName,__VA_ARGS__)
#define DEFINE_FIELD(index,structName,type,name) \
	template<>\
	struct Field<index> \
	{\
		using Type = type;\
		template<typename OutType = type> static OutType GetValue(const structName & instance) { return instance.##name; }\
		template<typename InType = type> static void SetValue(structName & instance,InType newValue) { instance.##name = newValue; }\
		static const constexpr cts_wrapper<DELAY_CONCAT(#name,_cts)> Name {};\
		static const constexpr size_t Index = index;\
	};\
	template<>\
	struct FieldByName<cts_wrapper<DELAY_CONCAT(#name,_cts)>> \
	{\
		using Type = type;\
		template<typename OutType = type> static OutType GetValue(const structName & instance) { return instance.##name; }\
		template<typename InType = type> static void SetValue(structName & instance,InType newValue) { instance.##name = newValue; }\
		static const constexpr cts_wrapper<DELAY_CONCAT(#name,_cts)> Name {};\
		static const constexpr size_t Index = index;\
	};\
	type name



