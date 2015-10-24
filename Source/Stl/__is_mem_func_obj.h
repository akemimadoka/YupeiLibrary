#include <type_traits>


namespace Yupei
{
	using std::is_function;

	template<typename>
	struct is_member_function_pointer_impl : false_type {};

	template<typename T,typename ClassT>
	struct is_member_function_pointer_impl<T ClassT::*> :
		bool_constant<is_function<T>::value> 
	{
		using class_type = ClassT;
	};

	template<typename T>
	struct is_member_function_pointer : is_member_function_pointer_impl<remove_cv_t<T>> {};

	template<typename>
	struct is_member_object_pointer_impl : false_type {};

	template<typename T, typename ClassT>
	struct is_member_object_pointer_impl<T ClassT::*> :
		bool_constant<!is_function<T>::value>
	{
		using class_type = ClassT;
	};

	template<typename T>
	struct is_member_object_pointer : is_member_object_pointer_impl<remove_cv_t<T>> {};
	
}