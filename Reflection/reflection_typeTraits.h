#pragma once

template<typename T>
struct is_reflection_struct
{

	template<typename U>
	static void check(decltype(&U::FieldLength));

	template<typename U>
	static int check(...);

	enum { value = std::is_void<decltype(check<T>(0))>::value };

};

template<typename T>
static constexpr bool is_reflection_struct_v = is_reflection_struct<T>::value;


template <typename T, typename N>
	requires is_reflection_struct_v<T>
struct reflection_contains_field
{
	template<typename U>
	static void check(decltype(&U::template FieldByName<N>::Name));

	template<typename U>
	static int check(...);

	enum { value = std::is_void<decltype(check<std::decay_t<T>>(0))>::value };
};
template<typename T, typename N>
static constexpr bool reflection_contains_field_v = reflection_contains_field<T, N>::value;



template<typename T, int Index>
	requires is_reflection_struct_v<T>
struct reflection_element
{
	using type = typename T::template Field<Index>::Type;
};
template<typename T, int Index>
using reflection_element_t = typename reflection_element<T, Index>::type;


template <typename T, typename N>
	requires is_reflection_struct_v<T>&& reflection_contains_field_v<T, N>
struct reflection_field
{
	using type = typename T::template FieldByName<N>::Type;
};
template<typename T, typename N>
using reflection_field_t = typename reflection_field<T, N>::type;



template<typename T, int Index>
	requires is_reflection_struct_v<T>
struct struct_name
{
	static constexpr const char* name = T::template Field<Index>::Name;
};
template<typename T, int Index>
static constexpr const char* struct_name_v = struct_name<T, Index>::name;




