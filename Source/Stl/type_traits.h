#pragma once

#include "traits_internal.h"
#include "utility_internal.h"
#include "__is_mem_func_obj.h"
#include <type_traits>

namespace Yupei
{

	template<typename Type>
	struct remove_extent
	{
		using type = Type;
	};

	template<typename Type>
	struct remove_extent<Type[]>
	{
		using type = Type;
	};

	template<typename Type, std::size_t Index>
	struct remove_extent<Type[Index]>
	{
		using type = Type;
	};

	template<typename Type>
	using remove_extent_t = typename remove_extent<Type>::type;

	template<typename Type>
	struct remove_all_extents
	{
		using type = Type;
	};

	template<typename Type>
	struct remove_all_extents<Type[]>
	{
		using type = typename remove_all_extents<Type>::type;
	};

	template<typename Type, std::size_t Index>
	struct remove_all_extents<Type[Index]>
	{
		using type = typename remove_all_extents<Type>::type;
	};

	template<typename Type>
	using remove_all_extents_t = typename remove_all_extents<Type>::type;

	/*20.10.4.1, primary type categories:
	template <class T> struct is_void;
	template <class T> struct is_null_pointer;
	template <class T> struct is_integral;
	template <class T> struct is_floating_point;
	template <class T> struct is_array;
	template <class T> struct is_pointer;
	template <class T> struct is_lvalue_reference;
	template <class T> struct is_rvalue_reference;
	template <class T> struct is_member_object_pointer;
	template <class T> struct is_member_function_pointer;
	template <class T> struct is_enum;
	template <class T> struct is_union;
	template <class T> struct is_class;
	template <class T> struct is_function;*/
	/*20.10.4.1 Primary type categories [meta.unary.cat]
	1 The primary type categories correspond to the descriptions given in section 3.9 of the C++ standard.
	2 For any given type T, the result of applying one of these templates to T and to cv-qualified T shall yield the
	same result.
	3 [ Note: For any given type T, exactly one of the primary type categories has a value member that evaluates
	to true. —end note ]*/

	template<typename Type>
	using is_void = is_same<remove_cv_t<Type>, void>;

	template<typename Type>
	using is_null_pointer = is_same<remove_cv_t<Type>, std::nullptr_t>;

	template<typename Type>
	struct is_array : false_type {};

	template<typename Type>
	struct is_array<Type[]> : true_type {};

	template<typename Type, std::size_t N>
	struct is_array<Type[N]> : true_type {};


	template<typename Type>
	struct is_lvalue_reference : false_type {};

	template<typename Type>
	struct is_lvalue_reference<Type&> : true_type {};

	template<typename Type>
	struct is_rvalue_reference : false_type {};

	template<typename Type>
	struct is_rvalue_reference<Type&&> : true_type {};

	/*Includes pointers to functions but not pointers to non-static members.*/
	template<typename Type>
	struct is_pointer : false_type {};

	template<typename Type>
	struct is_pointer<Type*> : bool_constant<!is_member_object_pointer<Type*>::value> {};

	using std::is_enum;

	using std::is_union;

	using std::is_class;

	// 20.10.4.2, composite type categories:
	// template <class T> struct is_reference;
	// template <class T> struct is_arithmetic;
	// template <class T> struct is_fundamental;
	// template <class T> struct is_object;
	// template <class T> struct is_scalar;
	// template <class T> struct is_compound;
	// template <class T> struct is_member_pointer;

	template<typename Type>
	using is_reference = bool_constant<is_lvalue_reference<Type>::value || is_rvalue_reference<Type>::value>;

	//N3797 3.9.1 Fundamental types
	template<typename Type>
	using is_fundamental = integral_constant<
		bool,
		is_arithmetic<Type>::value ||
		is_void<Type>::value ||
		is_null_pointer<Type>::value
	>;

	//N3797 3.9 
	// 8 An object type is a (possibly cv-qualified) type that is not a function type, not a reference type, and not a
	// void type.
	template<typename Type>
	using is_object = bool_constant<
		!is_function<Type>::value&&
		!is_reference<Type>::value&&
		!is_void<Type>::value
	>;

	// T is a pointer to non-static
	// data member or non-static
	// member function
	template<typename Type>
	using is_member_pointer = bool_constant<
		is_member_function_pointer<Type>::value ||
		is_member_object_pointer<Type>::value
	>;

	//N3797 3.9 
	//9 Arithmetic types (3.9.1), enumeration types, pointer types, pointer to member types (3.9.2), 
	//std::nullptr_t, and cv-qualified versions of these types (3.9.3) are collectively called scalar types.
	template<typename Type>
	using is_scalar = bool_constant<
		is_arithmetic<Type>::value ||
		is_enum<Type>::value ||
		is_pointer<Type>::value ||
		is_member_pointer<Type>::value ||
		is_null_pointer<Type>::value
	>;

	template<typename Type>
	using is_compound = bool_constant<
		!is_fundamental<Type>::value
	>;

	//20.10.4.3, type properties :
	// template <class T> struct is_const;
	// template <class T> struct is_volatile;
	// template <class T> struct is_trivial;
	// template <class T> struct is_trivially_copyable;
	// template <class T> struct is_standard_layout;
	// template <class T> struct is_pod;
	// template <class T> struct is_literal_type;
	// template <class T> struct is_empty;
	// template <class T> struct is_polymorphic;
	// template <class T> struct is_abstract;
	// template <class T> struct is_signed;
	// template <class T> struct is_unsigned;
	// template <class T, class... Args> struct is_constructible;
	// template <class T> struct is_default_constructible;
	// template <class T> struct is_copy_constructible;
	// template <class T> struct is_move_constructible;
	// template <class T, class U> struct is_assignable;
	// template <class T> struct is_copy_assignable;
	// template <class T> struct is_move_assignable;
	// template <class T> struct is_destructible;
	// template <class T, class... Args> struct is_trivially_constructible;
	// template <class T> struct is_trivially_default_constructible;
	// template <class T> struct is_trivially_copy_constructible;
	// template <class T> struct is_trivially_move_constructible;
	// template <class T, class U> struct is_trivially_assignable;
	// template <class T> struct is_trivially_copy_assignable;
	// template <class T> struct is_trivially_move_assignable;
	// template <class T> struct is_trivially_destructible;
	// template <class T, class... Args> struct is_nothrow_constructible;
	// template <class T> struct is_nothrow_default_constructible;
	// template <class T> struct is_nothrow_copy_constructible;
	// template <class T> struct is_nothrow_move_constructible;
	// template <class T, class U> struct is_nothrow_assignable;
	// template <class T> struct is_nothrow_copy_assignable;
	// template <class T> struct is_nothrow_move_assignable;
	// template <class T> struct is_nothrow_destructible;
	// template <class T> struct has_virtual_destructor;

	template<typename Type>
	struct is_const : false_type {};

	template<typename Type>
	struct is_const<const Type> : true_type {};

	template<typename Type>
	struct is_volatile : false_type {};

	template<typename Type>
	struct is_volatile<volatile Type> : true_type {};

	// A trivially copyable class is a class that:
	// — has no non-trivial copy constructors (12.8),
	// — has no non-trivial move constructors (12.8),
	// — has no non-trivial copy assignment operators (13.5.3, 12.8),
	// — has no non-trivial move assignment operators (13.5.3, 12.8), and
	// — has a trivial destructor (12.4).
	//Scalar types, trivially copyable class types(Clause 9), arrays of such types, and non - volatile constqualified
	// versions of these types (3.9.3) are collectively called trivially copyable types.

	using std::is_trivially_copyable;


	// A trivial class is a class that has a default constructor (12.1), has no non-trivial default constructors, and is trivially copyable.
	// [ Note: In particular, a trivially copyable or trivial class does not have virtual functions or virtual base classes.—end note ]

	using std::is_trivial;

	// 7 A standard-layout class is a class that:
	// — has no non-static data members of type non-standard-layout class (or array of such types) or reference,
	// — has no virtual functions (10.3) and no virtual base classes (10.1),
	// — has the same access control (Clause 11) for all non-static data members,
	// — has no non-standard-layout base classes,
	// — either has no non-static data members in the most derived class and at most one base class with
	// non-static data members, or has no base classes with non-static data members, and
	// — has no base classes of the same type as the first non-static data member.110
	// 8 A standard-layout struct is a standard-layout class defined with the class-key struct or the class-key class.
	// A standard-layout union is a standard-layout class defined with the class-key union.
	// 9 [ Note: Standard-layout classes are useful for communicating with code written in other programming languages.
	// Their layout is specified in 9.2.—end note ]
	//Scalar types, standard-layout class types (Clause 9), arrays of such types and cv-qualified
	// versions of these types (3.9.3) are collectively called standard-layout types.

	using std::is_standard_layout;

	// 10 A POD struct111 is a non-union class that is both a trivial class and a standard-layout class, and has no
	// non-static data members of type non-POD struct, non-POD union (or array of such types). Similarly, a
	// POD union is a union that is both a trivial class and a standard-layout class, and has no non-static data
	// members of type non-POD struct, non-POD union (or array of such types). A POD class is a class that is
	// either a POD struct or a POD union.
	// [ Example:
	// struct N { // neither trivial nor standard-layout
	//	int i;
	//	int j;
	// virtual ~N();
	// };
	// struct T { // trivial but not standard-layout
	//	int i;
	// private:
	//	int j;
	// };
	// struct SL { // standard-layout but not trivial
	//	int i;
	//	int j;
	//	~SL();
	// };
	// struct POD { // both trivial and standard-layout
	//	int i;
	//	int j;
	// };
	// —end example ]

	using std::is_pod;

	// An aggregate is an array or a class (Clause 9) with no user-provided constructors (12.1), no private or
	// protected non-static data members (Clause 11), no base classes (Clause 10), and no virtual functions (10.3)
	// A type is a literal type if it is:
	// — void; or
	// — a scalar type; or
	// — a reference type; or
	// — an array of literal type; or
	// — a class type (Clause 9) that has all of the following properties:
	// — it has a trivial destructor,
	// — it is an aggregate type (8.5.1) or has at least one constexpr constructor or constructor template
	// that is not a copy or move constructor, and
	// — all of its non-static data members and base classes are of non-volatile literal types.

	using std::is_literal_type;

	// T is a class type, but not a
	// union type, with no
	// non-static data members
	// other than bit-fields of
	// length 0, no virtual
	// member functions, no
	// virtual base classes, and
	// no base class B for which
	// is_empty<B>::value is
	// false.

	using std::is_empty;

	// Virtual functions support dynamic binding and object-oriented programming. A class that declares or
	// inherits a virtual function is called a polymorphic class.

	using std::is_polymorphic;

	// An abstract class is a class that can be used only as a base class of some other class; no objects of an abstract
	// class can be created except as subobjects of a class derived from it. A class is abstract if it has at least
	// one pure virtual function.

	using std::is_abstract;


	using std::is_final;


	// If is_arithmetic<T>::value is true, the same result as integral_ constant<bool, T(-1) <T(0)>::value; otherwise,false

	template<typename Type>
	using is_signed = bool_constant <
		is_arithmetic<Type>::value &&
		bool_constant<Type(-1) < Type(0) > ::value>;

	// If is_arithmetic<T>::value is true, the same result as integral_constant<bool, T(0) <T(-1)>::value; otherwise, false

	template<typename Type>
	using is_unsigned = bool_constant <
		is_arithmetic<Type>::value &&
		bool_constant<Type(0) < Type(-1) > ::value>;

	using std::is_constructible;

	template<typename Type>
	using is_default_constructible = is_constructible<Type>;

	// For a referenceable type T, the same result asis_constructible<T,const T&>::value, otherwise false.

	template<typename Type>
	using is_copy_constructible = is_constructible<
		Type, const Type&>;

	// For a referenceable type T,
	// the same result as
	// is_constructible<T,
	// T&&>::value,
	// otherwise false.

	template<typename Type>
	using is_move_constructible = is_constructible<
		Type, Type&&>;

	// The expression declval<T>() = declval<U>() is well-formed when treated
	// as an unevaluated operand (Clause 5). Access checking is performed as if
	// in a context unrelated to T and U. Only the validity of the immediate context of
	// the assignment expression is considered. [ Note: The compilation of the
	// expression can result in side effects such as the instantiation of class
	// template specializations and function template specializations, the
	// generation of implicitly-defined functions, and so on. Such
	// side effects are not in the “immediate context” and can result in the program
	// being ill-formed. —end note ]

	namespace Internal
	{
		template<typename T, typename U, typename = void>
		struct IsAssignableHelper : false_type {};

		template<typename T, typename U>
		struct IsAssignableHelper<T,U,void_t<decltype(Yupei::declval<T>() = Yupei::declval<U>())>> : true_type{};
	}

	template<typename T, typename U>
	struct is_assignable : Internal::IsAssignableHelper<T, U>::type {};

	template<typename Type>
	using is_copy_assignable = is_assignable<Type&, const Type&>;

	template<typename Type>
	using is_move_assignable = is_assignable<Type&, Type&&>;

	using std::is_destructible;

	using std::is_trivially_constructible;

	template<typename Type>
	using is_trivially_default_constructible = is_trivially_constructible<Type>;

	template<typename Type>
	using is_trivially_copy_constructible = is_trivially_constructible<Type, const Type&>;

	template<typename Type>
	using is_trivially_move_constructible = is_trivially_constructible<Type, Type&&>;

	using std::is_trivially_assignable;

	template<typename Type>
	using is_trivially_copy_assignable = is_trivially_assignable<Type&, const Type&>;

	template<typename Type>
	using is_trivially_move_assignable = is_trivially_assignable<Type&, Type&&>;

	using std::is_trivially_destructible;

	using std::is_nothrow_constructible;

	template<typename Type>
	using is_nothrow_default_constructible = is_nothrow_constructible<Type>;

	template<typename Type>
	using is_nothrow_copy_constructible = is_nothrow_constructible<Type, const Type&>;

	template<typename Type>
	using is_nothrow_move_constructible = is_nothrow_constructible<Type, Type&&>;

	template<typename T,typename U>
	struct is_nothrow_assignable : bool_constant<is_assignable<T, U>::value && noexcept(declval<T>() = declval<U>())> {};

	template<typename Type>
	using is_nothrow_copy_assignable = is_nothrow_assignable<Type&, const Type&>;

	template<typename Type>
	using is_nothrow_move_assignable = is_nothrow_assignable<Type&, Type&&>;

	using std::is_nothrow_destructible;

	using std::has_virtual_destructor;

	// 20.10.5, type property queries:
	//template <class T> struct alignment_of;
	//template <class T> struct rank;
	//template <class T, unsigned I = 0> struct extent;

	template<typename Type>
	using alignment_of = integral_constant<std::size_t, alignof(Type)>;

	template<typename Type>
	struct rank : integral_constant<std::size_t, 0> {};

	template<typename Type, size_t N>
	struct rank<Type[N]> : integral_constant<std::size_t, rank<Type>::value + 1> {};

	template<typename Type>
	struct rank<Type[]> : integral_constant<std::size_t, rank<Type>::value + 1> {};

	namespace Internal
	{
		template<typename Type,unsigned I,unsigned Now>
		struct ExtentHelper : integral_constant<std::size_t, 0> {};

		template<typename Type,unsigned I,unsigned Now>
		struct ExtentHelper<Type[], I, Now> :ExtentHelper<Type, I, Now + 1> {};

		template<typename Type, unsigned I>
		struct ExtentHelper<Type[], I, I> :integral_constant<std::size_t, 0> {};

		template<typename Type, unsigned I, unsigned Now, std::size_t N>
		struct ExtentHelper<Type[N], I, Now> :ExtentHelper<Type, I, Now + 1> {};

		template<typename Type,unsigned I,std::size_t N>
		struct ExtentHelper<Type[N], I, I>:integral_constant<std::size_t,N>{};
	}

	template<typename Type,unsigned I = 0>
	struct extent : Internal::ExtentHelper<Type, I, 0>{};

	// 20.10.6, type relations:
	//template <class T, class U> struct is_same;
	//template <class Base, class Derived> struct is_base_of;
	//template <class From, class To> struct is_convertible;

	//is_same is in traits_internal.h


	template <typename F, typename T>
	using is_convertible = std::is_convertible<F, T>;

	// 20.10.7.1, const-volatile modifications:
	/*template <class T> struct remove_const;
	template <class T> struct remove_volatile;
	template <class T> struct remove_cv;
	template <class T> struct add_const;
	template <class T> struct add_volatile;
	template <class T> struct add_cv;*/
	//complete

	// 20.10.7.2, reference modifications:
	//template <class T> struct remove_reference;
	//template <class T> struct add_lvalue_reference;
	//template <class T> struct add_rvalue_reference;
	//complete

	// 20.10.7.3, sign modifications:
	//template <class T> struct make_signed;
	//template <class T> struct make_unsigned;

	namespace Internal
	{
		template<typename Type>
		struct SignedHelper
		{
			static_assert(
				is_integral<Type>::value || is_enum<Type>::value
				&& !is_same<Type, bool>::value,
				"Hahahahahahahahahaha,non-integral type & bool are disallowed ! hahahahaha");
			//if..else
			using signed_type = conditional_t <
				is_one_of<Type, signed char, unsigned char>::value, //if(Type == signed char || Type == unsigned char)
				signed char, // then type == signed char
				conditional_t < //else if
				is_one_of<Type, signed short, unsigned short>::value,//if(Type == signed short || Type == unsigned short)
				signed short,  //then type == signed short
				conditional_t < //else 
				is_one_of<Type, signed int, unsigned int>::value,//if(Type == signed int || Type == unsigned int)
				signed int, //then type == signed int
				conditional_t <
				is_one_of<Type, signed long, unsigned long>::value,
				signed long,
				conditional_t <
				is_one_of<Type, signed long long, unsigned long long>::value,
				signed long long,
				conditional_t < sizeof(Type) == sizeof(signed char),
				signed char,
				conditional_t < sizeof(Type) == sizeof(signed short),
				signed short,
				conditional_t < sizeof(Type) == sizeof(signed int),
				signed int,
				conditional_t<sizeof(Type) == sizeof(signed long),
				signed long,
				signed long long
				>> >> >> >> >;
			using unsigned_type =
				conditional_t <
				is_same<signed_type, signed char>::value,
				unsigned char,
				conditional_t <
				is_same<signed_type, signed short>::value,
				unsigned short,
				conditional_t <
				is_same<signed_type, signed int>::value,
				unsigned int,
				conditional_t <
				is_same<signed_type, signed long>::value,
				unsigned long,
				unsigned long long
				>> >> ;
		};
	}

	template<typename Type>
	struct make_signed
	{
		using type = typename Internal::SignedHelper<Type>::signed_type;
	};

	template<typename Type>
	using make_signed_t = typename make_signed<Type>::type;

	template<typename Type>
	struct make_unsigned
	{
		using type = typename Internal::SignedHelper<Type>::unsigned_type;
	};

	template<typename Type>
	using make_unsigned_t = typename make_unsigned<Type>::type;

	// 20.10.7.5, pointer modifications:
	//template <class T> struct remove_pointer;
	//template <class T> struct add_pointer;

	namespace Internal
	{
		template<typename Type>
		struct RemovePointerHelper
		{
			using type = Type;
		};

		template<typename Type>
		struct RemovePointerHelper<Type*>
		{
			using type = Type;
		};
	}

	template<typename Type>
	using remove_pointer = Internal::RemovePointerHelper<remove_cv_t<Type>>;

	template <typename Type>
	using remove_pointer_t = typename remove_pointer<Type>::type;

	template<typename Type>
	struct add_pointer
	{
		using type = remove_reference_t<Type>*;
	};

	template <typename Type>
	using add_pointer_t = typename add_pointer<Type>::type;

	// 20.10.7.6, other transformations:
	//template <std::size_t Len,
	// std::size_t Align = default-alignment> // see 20.10.7.6
	// struct aligned_storage;
	// template <std::size_t Len, class... Types> struct aligned_union;
	// template <class T> struct decay;
	// template <bool, class T = void> struct enable_if;
	// template <bool, class T, class F> struct conditional;
	// template <class... T> struct common_type;
	// template <class T> struct underlying_type;
	// template <class> class result_of; // not defined
	// template <class F, class... ArgTypes> class result_of<F(ArgTypes...)>;
	// template <std::size_t Len,
	// std::size_t Align = default-alignment > // see 20.10.7.6
	// using aligned_storage_t = typename aligned_storage<Len,Align>::type;
	// template <std::size_t Len, class... Types>
	// using aligned_union_t = typename aligned_union<Len,Types...>::type;
	// template <class T>
	// using decay_t = typename decay<T>::type;
	// template <bool b, class T = void>
	// using enable_if_t = typename enable_if<b,T>::type;
	// template <bool b, class T, class F>
	// using conditional_t = typename conditional<b,T,F>::type;
	// template <class... T>
	// using common_type_t = typename common_type<T...>::type;
	// template <class T>
	// using underlying_type_t = typename underlying_type<T>::type;
	// template <class T>
	// using result_of_t = typename result_of<T>::type;
	// template <class...>
	// using void_t = void;


	template <std::size_t Len,std::size_t Align = alignment_of<max_align_t>::value>
	struct aligned_storage
	{
		struct type
		{
			alignas(Align) unsigned char data[Len];
		};
	};

	template<std::size_t Len,std::size_t Align = alignment_of<std::max_align_t>::value>
	using aligned_storage_t = typename aligned_storage<Len, Align>::type;

	template<typename Type>
	struct decay
	{
		using temp_type = remove_reference_t<Type>;
		using type = conditional_t <is_array<temp_type>::value,
			remove_extent_t<temp_type>,
			conditional_t <is_function<temp_type>::value,
				add_pointer_t<temp_type>,
				remove_cv_t<temp_type>
			>> ;
	};

	template<typename Type>
	using decay_t = typename decay<Type>::type;

	template <typename... Args>
	struct common_type;

	template <typename Type>
	struct common_type<Type>
	{
		using type = Type;
	};

	//http://stackoverflow.com/questions/21975812/should-stdcommon-type-use-stddecay
	template<typename Type1,
		typename Type2>
	struct common_type<Type1, Type2>
	{
		using type = decay_t<
			decltype(true ? declval<Type1>() : declval<Type2>())
		>;
	};

	template<typename Type1,
		typename Type2,
		typename... Args>
	struct common_type<Type1,
		Type2,
		Args...>
	{
		using type =typename common_type<typename common_type<Type1, Type2>::type,Args...>::type;
	};

	template<typename... Type>
	using common_type_t = typename common_type<Type...>::type;


	template<typename Type>
	inline constexpr Type static_max(Type t) noexcept
	{
		return t;
	}

	template<typename Type1,
		typename Type2,
		typename... Args>
		inline constexpr Yupei::common_type_t<Type1, Type2, Args...>
		static_max(Type1 t1, Type2 t2, Args... args) noexcept
	{
		return static_max((t1 > t2 ?t1: t2),static_max(args)...);
	}

	//Provides the member typedef type, which is a POD type of a size and 
	//alignment suitable for use as uninitialized storage for an object of any 
	//of the types listed in Types. The size of the storage is at least Len. 
	//std::aligned_union also determines the strictest (largest) alignment requirement among 
	//all Types and makes it available as the constant alignment_value. 
	//
	//满足Types...中最严格的对齐要求，大小至少为Len。
	//http://en.cppreference.com/w/cpp/types/aligned_union

	template <std::size_t Len, typename... Types>
	struct aligned_union
	{
		static constexpr std::size_t alignment_value = Yupei::static_max(alignof(Types)...);

		struct type
		{
			alignas(alignment_value) char _s[Yupei::static_max(Len, alignment_value)];
		};
	};

	template <std::size_t Len, typename... Types>
	using aligned_union_t = typename aligned_union<Len, Types...>::type;

	using std::underlying_type;

	template<typename Type>
	using underlying_type_t = typename underlying_type<Type>::type;

	namespace Internal
	{
		struct InvokeMemberFunc
		{
			template<typename F,typename T1,typename... Args>
			static decltype(auto) _Call(F f, T1&& t1, Args&&... args)
				//->decltype((Yupei::forward<T1>(t1).*f)(Yupei::forward<Args>(args)...))
			{
				return (Yupei::forward<T1>(t1).*f)(Yupei::forward<Args>(args)...);
			}
		};

		struct InvokeMemberFunc2
		{
			template<typename F,typename T1,typename... Args>
			static decltype(auto) _Call(F f, T1&& t1, Args&&... args)
				//->decltype(((*Yupei::forward<T1>(t1)).*f)(Yupei::forward<Args>(args)...))
			{
				return ((*Yupei::forward<T1>(t1)).*f)(Yupei::forward<Args>(args)...);
			}
		};

		struct GetMember
		{
			template<typename F,typename T1>
			static decltype(auto) _Call(F f, T1&& t1)
				//->decltype((Yupei::forward<T1>(t1)).*f)
			{
				return (Yupei::forward<T1>(t1)).*f;
			}
		};

		struct GetMember2
		{
			template<typename F,typename T1>
			static decltype(auto) _Call(F f, T1&& t1)
				//->decltype((*Yupei::forward<T1>(t1)).*f)
			{
				return (*Yupei::forward<T1>(t1)).*f;
			}
		};

		struct NormalCall
		{
			template<typename F,typename... Args>
			static decltype(auto) _Call(F f, Args&&... args)
				//->decltype(Yupei::forward<F>(f)(Yupei::forward<Args>(args)...))
			{
				return f(Yupei::forward<Args>(args)...);
			}
		};

		template<typename DecayType,typename T1,bool IsMemberFunc = is_member_function_pointer<DecayType>::value,
			bool IsMemberObj = is_member_object_pointer<DecayType>::value>
		struct Invoker;

		template<typename DecayType,typename T1>
		struct Invoker<DecayType, T1, true, false> :
			conditional_t<is_same<typename is_member_function_pointer<DecayType>::class_type, decay_t<T1>>::value ||
			is_base_of<typename is_member_function_pointer<DecayType>::class_type, decay_t<T1>>::value,
			InvokeMemberFunc,
			InvokeMemberFunc2>
		{

		};

		template<typename DecayType,typename T1>
		struct Invoker<DecayType, T1, false, true> :
			conditional_t <is_same<typename is_member_object_pointer<DecayType>::class_type,decay_t<T1>> ::value ||
			is_base_of<typename is_member_object_pointer<DecayType>::class_type,decay_t<T1>>::value,
			GetMember,
			GetMember2>
		{

		};
		template<typename DecayType,typename T1>
		struct Invoker<DecayType, T1, false, false> : NormalCall{};

		template<typename F,
			typename...Args>
		struct Invoker1;

		template<typename F>
		struct Invoker1<F> :NormalCall	{};

		template<typename F,typename T1,typename... Args>
		struct Invoker1<F, T1, Args...> : Invoker<decay_t<F>, T1>{};
	}


	/*(1.1) —(t1.*f)(t2, ..., tN) when f is a pointer to a member function of a class T and t1 is an object of
		type T or a reference to an object of type T or a reference to an object of a type derived from T;
	(1.2) —((*t1).*f)(t2, ..., tN) when f is a pointer to a member function of a class T and t1 is not one of
		the types described in the previous item;
	(1.3) — t1.*f when N == 1 and f is a pointer to member data of a class T and t1 is an object of type T or a
		reference to an object of type T or a reference to an object of a type derived from T;
	(1.4) —(*t1).*f when N == 1 and f is a pointer to member data of a class T and t1 is not one of the types
		described in the previous item;
	(1.5) — f(t1, t2, ..., tN) in all other cases.*/

	// 20.9.3, invoke:	
	//template <class F, class... Args> result_of_t<F && (Args&&...)> invoke(F&& f, Args&&... args);

	template <typename F, typename... Args>
	inline decltype(auto) invoke(F&& f, Args&&... args)
	{
		return Internal::Invoker1<F, Args...>::_Call(Yupei::forward<F>(f), Yupei::forward<Args>(args)...);
	}


	//template <class> class result_of; // not defined
   // template <class F, class... ArgTypes> class result_of<F(ArgTypes...)>;

   //This is an Expression-SFINAE implement,which is not supported by VS2015 ...
   template<typename Type,typename = void>
   struct result_of
   {
   	// no member type
   };

   template<typename F,typename... Args>
   struct result_of<F(Args...),void_t<decltype(Yupei::invoke(declval<F>(), declval<Args>()...))>>
   {
   		using type = decltype(invoke(declval<F>(), declval<Args>()...));
   };

	template <typename Type>
	using result_of_t = typename result_of<Type>::type;

	namespace Internal
	{
		template<bool...B>
		struct StaticAnd;

		template<bool B, bool... Args>
		struct StaticAnd<B, Args...> : bool_constant<B && StaticAnd<Args...>::value>
		{
		};

		template<>
		struct StaticAnd<> : true_type
		{
		};

	}

	struct nonesuch
	{
		nonesuch() = delete;
		~nonesuch() = delete;
		nonesuch(const nonesuch&) = delete;
		void operator=(const nonesuch&) = delete;
	};

	template<typename DefaultT,
		typename, // always void
		template<typename...>class Op,
		typename... Args>
	struct detector
	{
		using value_t = false_type;
		using type = DefaultT;
	};

	template<typename DefaultT,
		template<typename...>class Op,
		typename... Args>
	struct detector<DefaultT, void_t<Op<Args...>>, Op, Args...>
	{
		using value_t = true_type;
		using type = Op<Args...>;
	};

	template<template<typename...>class Op,typename... Args>
	using is_detected = typename detector<void, void, Op, Args...>::value_t;

	template<template<typename...>class Op, typename... Args>
	using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

	template<typename DefaultT,template<typename...>class Op,typename... Args>
	using detected_or = detector<DefaultT, void, Op, Args...>;

	template<typename DefaultT,template<typename...>class Op,typename... Args>
	using deteced_or_t = typename detected_or<DefaultT, Op, Args...>::type;

	template<typename Expected, template<typename...>class Op, typename... Args>
	using is_deteced_exact = is_same<detected_t<Op, Args...>,Expected>;

#if __cpp_variable_templates
	template<template<typename...>class Op, typename... Args>
	constexpr bool is_detected_v = is_detected<Op, Args...>::value;

	template<typename Expected, template<typename...>class Op, typename... Args>
	constexpr bool is_deteced_exact_v = is_deteced_exact<Expected, Op, Args...>::value;
#endif

}