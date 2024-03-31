#pragma once

#include <type_traits>

#include "reflection_typeTraits.h"

namespace detail
{
	template<typename T, typename Func, size_t Index, typename... OtherArgs>
	void call_each_impl(T&& obj, Func&& func, OtherArgs&&... args)
	{
		using Field = typename std::decay_t<T>::template Field<Index>;
		if constexpr (std::is_same_v<std::tuple<std::tuple<>>, std::tuple<OtherArgs...>>)
			func(Field::Name, std::forward<const typename Field::Type>(Field::GetValue(obj)));

		else
			func(Field::Name, std::forward<const typename Field::Type>(Field::GetValue(obj)), std::forward<OtherArgs>(args)...);

	}

	template<typename T, typename Func, size_t... Index, typename... OtherArgs>
	void for_each_impl(T&& obj, Func&& func, std::index_sequence<Index...> is, OtherArgs&&... args)
	{
		(call_each_impl<decltype(std::forward<T>(obj)), Func, Index>(std::forward<T>(obj), std::forward<Func>(func),
			std::forward<OtherArgs>(args)...), ...);
	}


	template<typename T, typename Func, size_t Index, typename... OtherArgs>
	void call_each_impl(const T&& obj, Func&& func, OtherArgs&&... args)
	{
		using Field = typename std::decay_t<T>::template Field<Index>;
		if constexpr (std::is_same_v<std::tuple<std::tuple<>>, std::tuple<OtherArgs...>>)

			func(Field::Name, std::forward<const typename Field::Type>(Field::GetValue(obj)));

		else

			func(Field::Name, std::forward<const typename Field::Type>(Field::GetValue(obj)), std::forward<OtherArgs>(args)...);

	}

	template<typename T, typename Func, size_t... Index, typename... OtherArgs>
	void for_each_impl(const T&& obj, Func&& func, std::index_sequence<Index...> is, OtherArgs&&... args)
	{
		(call_each_impl<decltype(std::forward<const T>(obj)), Func, Index>(std::forward<const T>(obj), std::forward<Func>(func),
			std::forward<OtherArgs>(args)...), ...);
	}
}


// for_each
template<typename T, typename Func, typename... OtherArgs>
	requires is_reflection_struct_v<T>
void for_each(T&& obj, Func&& func, OtherArgs&&... args)
{
	detail::for_each_impl(std::forward<T>(obj), std::forward<Func>(func), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::forward<OtherArgs>(args)...);
}

template<typename T, typename Func>
	requires is_reflection_struct_v<T>
void for_each(T&& obj, Func&& func)
{
	detail::for_each_impl(std::forward<T>(obj), std::forward<Func>(func), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::tuple<>());
}


// for_each for const
template<typename T, typename Func, typename... OtherArgs>
	requires is_reflection_struct_v<T>
void for_each(const T&& obj, Func&& func, OtherArgs&&... args)
{
	detail::for_each_impl(std::forward<const T>(obj), std::forward<Func>(func), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::forward<OtherArgs>(args)...);
}

template<typename T, typename Func>
	requires is_reflection_struct_v<T>
void for_each(const T&& obj, Func&& func)
{
	detail::for_each_impl(std::forward<const T>(obj), std::forward<Func>(func), std::make_index_sequence<std::decay_t<T>::FieldLength>(), std::tuple<>());
}


template<typename U, typename T>
	requires is_reflection_struct_v<U> && is_reflection_struct_v<T>
U& reflection_copy(U& dest, const T& source)
{
	using DecayU = std::decay_t<U>;
	using DecayT = std::decay_t<T>;
	for_each(std::forward<const T>(source), [&dest]<typename Cts, typename valueType>(Cts name, const valueType && value)
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
	return dest;
}

template<typename U,typename T>
	requires is_reflection_struct_v<U>&& is_reflection_struct_v<T>
bool reflection_partial_equal(const U& a, const T& b)
{
	using DecayU = std::decay_t<U>;
	using DecayT = std::decay_t<T>;
	bool equal = true;
	for_each(std::forward<const T>(b), [&equal, &a]<typename Cts, typename valueType>(Cts name, const valueType && value)
	{
		if (!equal) return;
		if constexpr (reflection_contains_field_v<DecayU, Cts> &&
			std::is_same_v<std::decay_t<typename DecayU::template FieldByName<Cts>::Type>, std::decay_t<valueType>>)
		{
			if constexpr (is_reflection_struct_v<valueType>)
				equal = reflection_equal(value, DecayU::template FieldByName<Cts>::GetValue(a));
			else
				equal = value == DecayU::template FieldByName<Cts>::GetValue(a);
		}
	});
	return equal;
}


template<typename U, typename T>
	requires is_reflection_struct_v<U>&& is_reflection_struct_v<T>
bool reflection_equal(const U& a, const T& b)
{
	if constexpr (U::FieldLength == T::FieldLength)
	{
		using DecayU = std::decay_t<U>;
		using DecayT = std::decay_t<T>;
		bool equal = true;
		for_each(std::forward<const T>(b), [&equal, &a]<typename Cts, typename valueType>(Cts name, const valueType && value)
		{
			if (!equal) return;
			if constexpr (reflection_contains_field_v<DecayU, Cts> &&
				std::is_same_v<std::decay_t<typename DecayU::template FieldByName<Cts>::Type>, std::decay_t<valueType>>)
			{
				if constexpr (is_reflection_struct_v<valueType>)
					equal = reflection_equal(value, DecayU::template FieldByName<Cts>::GetValue(a));
				else
					equal = value == DecayU::template FieldByName<Cts>::GetValue(a);
			}
			else
			{
				equal = false;
			}
		});
		return equal;
	}
	return false;
}

template<typename U, typename T>
	requires is_reflection_struct_v<U>&& is_reflection_struct_v<T>
bool operator==(const U& a,const T& b)
{
	return  reflection_equal(a, b);
}


template<typename T>
	requires is_reflection_struct_v<T>
T reflection_clone(const T& source)
{
	using DecayT = std::decay_t<T>;
	DecayT clone;
	for_each(std::forward<const T>(source), [&clone]<typename Cts, typename valueType>(Cts name,const valueType && value)
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

template<typename Name, typename T,typename RT = typename T::template FieldByName<Name>::Type>
	requires is_reflection_struct_v<T>
RT reflection_get_value(const T& obj)
{
	return T::template FieldByName<Name>::GetValue(obj);
}

template<int Index, typename T, typename RT = typename T::template Field<Index>::Type>
	requires is_reflection_struct_v<T>
RT reflection_get_value(const T& obj)
{
	return T::template Field<Index>::GetValue(obj);
}

template<int Index, typename T, typename RT = typename T::template Field<Index>::Type>
	requires is_reflection_struct_v<T>
void reflection_set_value(T& obj, RT&& value)
{
	T::template Field<Index>::SetValue(obj,value);
}

template<typename Name, typename T, typename RT = typename T::template FieldByName<Name>::Type>
	requires is_reflection_struct_v<T>
void reflection_set_value(T& obj, RT&& value)
{
	T::template FieldByName<Name>::SetValue(obj, value);
}

template<typename T,typename NeedType>
	requires is_reflection_struct_v<T>
bool reflection_contains(const T& obj, const char* fieldName)
{
	bool result = false;
	for_each(std::forward<const T>(obj), [&result,&fieldName]<typename Name>(Name name, auto)
		{
		if constexpr (!std::is_same_v<NeedType, std::decay_t<reflection_field_t<std::decay_t<T>, Name>>>) return;
			if (result) return;
			if (!strcmp(CTS_VALUE(name), fieldName))
				result = true;
		});
	return result;
}

template<typename NeedType, typename T>
	requires is_reflection_struct_v<T>
bool reflection_try_set_value(T& obj, const char* fieldName,const NeedType&& value)
{
	bool result = false;
	for_each(std::forward<T>(obj), [&]<typename Name>(Name name,auto)
	{
		if constexpr (std::is_same_v<NeedType, std::decay_t<reflection_field_t<std::decay_t<T>, Name>>>)
		{
			if (result) return;
			if (!strcmp(CTS_VALUE(name), fieldName))
			{
				reflection_set_value<Name, std::decay_t<T>>(obj, value);
				result = true;
			}
		}
	});
	return result;
}


template<typename NeedType, typename T>
	requires is_reflection_struct_v<T>
NeedType reflection_try_get_value(const T& obj, const char* fieldName)
{
	NeedType value;
	bool result = false;
	for_each(std::forward<const T>(obj), [&]<typename Name>(Name name, auto)
	{
		if constexpr (std::is_same_v<NeedType, std::decay_t<reflection_field_t<std::decay_t<T>, Name>>>)
		{
			if (result) return;
			if (strcmp(CTS_VALUE(name), fieldName) == 0)
			{
				value = reflection_get_value<Name, std::decay_t<T>>(obj);
				result = true;
			}
		}
	});
	return value;
}