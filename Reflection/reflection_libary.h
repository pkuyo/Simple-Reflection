#pragma once

#include <type_traits>

#include "complie_time_string.h"
#include "reflection_typeTraits.h"

// for_each for lambda
template<typename T, typename Func, size_t Index, typename... OtherArgs>
void call_each_impl(T&& obj, Func&& func, OtherArgs&&... args)
{
	using Field = typename std::decay_t<T>::template Field<Index>;
	if constexpr (std::is_same_v<std::tuple<std::tuple<>>, std::tuple<OtherArgs...>>)
		func(Field::Name, Field::GetValue(obj));
	else
		func(Field::Name, Field::GetValue(obj), std::forward<OtherArgs>(args)...);
}

template<typename T, typename Func, size_t... Index, typename... OtherArgs>
void for_each_impl(T&& obj, Func&& func, std::index_sequence<Index...> is, OtherArgs&&... args)
{
	(call_each_impl<decltype(std::forward<T>(obj)), Func, Index>(std::forward<T>(obj), std::forward<Func>(func),
		std::forward<OtherArgs>(args)...), ...);
}

template<typename T, typename Func, typename... OtherArgs>
	requires is_reflection_struct_v<T>
void for_each(T&& obj, Func&& func, OtherArgs&&... args)
{
	for_each_impl(std::forward<T>(obj), std::forward<Func>(func), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::forward<OtherArgs>(args)...);
}


template<typename T, typename Func>
	requires is_reflection_struct_v<T>
void for_each(T&& obj, Func&& func)
{
	for_each_impl(std::forward<T>(obj), std::forward<Func>(func), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::tuple<>());
}




// for_each for struct
template<typename Each, typename T, size_t Index, typename... OtherArgs>
void call_each_impl(T&& obj, OtherArgs&&... args)
{
	using Field = typename std::decay_t<T>::template Field<Index>;
	if constexpr (std::is_same_v<std::tuple<std::tuple<>>, std::tuple<OtherArgs...>>)
		Each::template invoke<typename Field::Type>(Field::Name, Field::GetValue(obj));
	else
		Each::template invoke<typename Field::Type>(Field::Name, Field::GetValue(obj), std::forward<OtherArgs>(args)...);
}

template<typename Each, typename T, size_t... Index, typename... OtherArgs>
void for_each_impl(T&& obj, std::index_sequence<Index...> is, OtherArgs&&... args)
{
	(call_each_impl<Each, decltype(std::forward<T>(obj)), Index>(std::forward<T>(obj), std::forward<OtherArgs>(args)...), ...);
}

template<typename Each, typename T, typename... OtherArgs>
	requires requires (OtherArgs... args)
{
	Each::template invoke<int>("", 1, args...);
	typename std::decay_t<T>::template Field<0>;
	std::decay_t<T>::FieldLength;
}
void for_each(T&& obj, OtherArgs&&... args)
{
	for_each_impl<Each>(std::forward<T>(obj), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::forward<OtherArgs>(args)...);
}
template<typename Each, typename T>
	requires is_reflection_struct_v<T>
void for_each(T&& obj)
{
	for_each_impl<Each>(std::forward<T>(obj), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::tuple<>());
}



template<typename U, typename T>
	requires is_reflection_struct_v<U> && is_reflection_struct_v<T>
void reflection_copy(U& dest,T&source)
{
	using DecayU = std::decay_t<U>;
	using DecayT = std::decay_t<T>;
	for_each(std::forward<T>(source), [&dest]<typename Cts, typename valueType>(Cts name, valueType && value)
		{
			if constexpr (reflection_contains_field_v<DecayU,Cts> &&
				std::is_same_v<std::decay_t<typename DecayU::template FieldByName<Cts>::Type>, std::decay_t<valueType>>)
			{
				if constexpr (is_reflection_struct_v<valueType>)
				{
					auto clonePart = reflection_clone(value);
					DecayU::template FieldByName<Cts>::SetValue(dest, clonePart);
				}
				else
				{
					DecayU::template FieldByName<Cts>::SetValue(dest, value);
				}
			}
		});
}

template<typename T>
	requires is_reflection_struct_v<T>
T reflection_clone(T& source)
{
	using DecayT = std::decay_t<T>;
	DecayT clone;
	for_each(std::forward<T>(source), [&clone]<typename Cts, typename valueType>(Cts name, valueType && value)
	{
		if constexpr (is_reflection_struct_v<valueType>)
		{
			auto clonePart = reflection_clone(value);
			DecayT::template FieldByName<Cts>::SetValue(clone, clonePart);
		}
		else
		{
			DecayT::template FieldByName<Cts>::SetValue(clone, value);
		}
	});
	return clone;
}