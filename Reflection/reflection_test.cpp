
#include <iostream>
#include <type_traits>
#include <format>
#include <fstream>

#include "complie_time_string.h"
#include "reflection_define.h"
#include "reflection_typeTraits.h"
#include "reflection_libary.h"

//requires C++20, /Zc:preprocessor MSVC

STRUCT(Test2,
	(int)a,
	(float)b
);

STRUCT(Test3,
	(Test2)inst,
	(float)flt
);

STRUCT(Test4,
	(Test2)inst,
	(float)flt1,
	(float)flt
);



//---------------------------------
//struct
template<int depth = 0>
struct reflection_dump_json
{
	template<typename T>
	static void invoke(auto name, T&& value, std::ostream& stream)
	{
		if constexpr (is_reflection_struct_v<T>)
		{
			stream << std::format("{}\"{}\":\n{}{{\n", Blank, CTS_VALUE(name), Blank);
			for_each<reflection_dump_json<depth + 1>>(std::forward<T>(value), stream);
			stream << Blank << "},\n";
		}
		else
			stream << std::format("{}\"{}\": {},\n", Blank, CTS_VALUE(name), value);

	}
private:
	static inline const std::string Blank = std::string(depth, '\t');
};
template<typename T>
requires is_reflection_struct_v<T>
void DumpUseStruct(const T & obj)
{
	std::cout << "{\n";
	for_each<reflection_dump_json<1>>(obj,std::cout);
	std::cout << "}\n";
}

//---------------------------------
//lambda
template<typename T>
	requires is_reflection_struct_v<T>
void DumpUseLambda(const T& obj,int depth = 0)
{
	auto lambda = [depth]<typename ValueType>(auto name, ValueType && value, std::ostream& stream)
	{
		const std::string Blank = std::string(depth+1, '\t');
		if constexpr (is_reflection_struct_v<ValueType>)
		{
			stream << std::format("{}\"{}\":\n{}\n", Blank, CTS_VALUE(name), Blank);
			DumpUseLambda(value, depth + 1);
		}
		else
			stream << std::format("{}\"{}\": {},\n", Blank, CTS_VALUE(name), value);
	};
	const std::string Blank = std::string(depth, '\t');
	std::cout << Blank << "{\n";
	for_each(obj, lambda,std::cout);
	std::cout << Blank << "},\n";

}


int main()
{
	Test3 a{
		.inst = {1,0.5f},
		.flt = 2.5f
	};
	Test4 b{ 0,0,0 ,0};
	reflection_copy(b, a);

	std::cout << std::format("Name:{}, FieldLength:{}\n", CTS_VALUE(Test4::Name), Test4::FieldLength);

	Test4::Field<2>::SetValue(b,1.5f);
	std::cout << std::format("Name:{}, Value:{}\n", CTS_VALUE(Test4::Field<2>::Name), Test4::Field<2>::GetValue(b));

	Test4::FieldByName<CTS_STR("flt")>::SetValue(b, 33.f);
	std::cout << std::format("Name:{}, Value:{}\n", CTS_VALUE(Test4::FieldByName<CTS_STR("flt")>::Name), Test4::FieldByName<CTS_STR("flt")>::GetValue(b));

	std::cout << typeid(typename Test4::FieldByName<CTS_STR("inst")>::Type).name() << "\n\n";

	//Dump to json format
	DumpUseStruct(a);
	DumpUseLambda(b);
	return 0;
}
