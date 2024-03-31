
#include <iostream>
#include <type_traits>
#include <format>
#include <fstream>


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



template<typename T>
	requires is_reflection_struct_v<T>
void DumpToJson(const T& obj,int depth = 0)
{
	auto lambda = [depth]<typename ValueType>(auto name, const ValueType && value, std::ostream& stream)
	{
		const std::string Blank = std::string(depth+1, '\t');
		if constexpr (is_reflection_struct_v<ValueType>)
		{
			stream << std::format("{}\"{}\":\n{}\n", Blank, CTS_VALUE(name), Blank);
			DumpToJson(value, depth + 1);
		}
		else if constexpr (T::template FieldByName<decltype(name)>::Index != T::FieldLength - 1)
			stream << std::format("{}\"{}\": {},\n", Blank, CTS_VALUE(name), value);
		else
			stream << std::format("{}\"{}\": {}\n", Blank, CTS_VALUE(name), value);
	};
	const std::string Blank = std::string(depth, '\t');
	std::cout << Blank << "{\n";
	for_each(std::forward<const T>(obj), lambda,std::cout);
	std::cout << Blank << "}\n";

}

template<typename T>
void print_value(const char* name,const T& value)
{
	std::cout << std::format("Name:{}, Value:{}\n", name, value);
}


int main()
{
	Test3 a{
		.inst = {1,0.5f},
		.flt = 2.5f
	};
	Test4 b;

	auto ca = reflection_clone(a);

	float value = 0.f;

	b = a; 

	//reflection_equal(ca,a);
	std::cout << std::format("ca == a Value: {}\n", ca == a);
	std::cout << std::format("b == a Value: {}\n", b == a);

	//reflection_partial_equal(b,a)
	std::cout << std::format("b partial equals a Value: {}\n", reflection_partial_equal(b,a));

	std::cout << std::format("Name:{}, FieldLength:{}\n", CTS_VALUE(Test4::Name), Test4::FieldLength);

	//Test4::Field<2>::SetValue(b,1.5f);
	reflection_set_value<2>(b, 1.5f);

	//Test4::Field<2>::GetValue(b);
	value = reflection_get_value<2>(b);
	print_value(CTS_VALUE(Test4::Field<2>::Name), value);

	//Test4::FieldByName<CTS_STR("flt")>::SetValue(b, 33.f);
	reflection_set_value<CTS_STR("flt")>(b, 33.f);

	//Test4::FieldByName<CTS_STR("flt")>::GetValue(b);
	value = reflection_get_value<CTS_STR("flt")>(b);

	
	print_value(CTS_VALUE(Test4::FieldByName<CTS_STR("flt")>::Name), value);

	//Test4::FieldByName<CTS_STR("inst")>::Type;
	std::cout << typeid(reflection_field_t<Test4,CTS_STR("inst")>).name() << "\n\n";

	//Dump to json format
	DumpToJson(b);

	reflection_try_set_value(b, "flt", 233.f);

	value = reflection_try_get_value<float>(b, "flt");
	
	std::cout << std::format("Find flt, Value: {}", value);

	return 0;
}
