#pragma once

//#include "basic.h"
#include "utility_internal.h"
#include "type_traits.h"
#include "__reference_wrapper.h"
#include "__swap.h"

namespace Yupei
{
	
//  #include <initializer_list>
//	namespace std {
//		// 20.2.1, operators:
//		namespace rel_ops {
//			template<class T> bool operator!=(const T&, const T&);
//			template<class T> bool operator> (const T&, const T&);
//			template<class T> bool operator<=(const T&, const T&);
//			template<class T> bool operator>=(const T&, const T&);
//		}
//		// 20.2.2, swap:
//		template<class T> void swap(T& a, T& b) noexcept(see below);
//		template <class T, size_t N> void swap(T(&a)[N], T(&b)[N]) noexcept(noexcept(swap(*a, *b)));
	struct piecewise_construct_t {};
	constexpr piecewise_construct_t piecewise_construct{};

	template<class...>
	struct tuple;

	namespace rel_ops
	{
		template<typename Type> 
		bool operator != (const Type& lhs, const Type& rhs)
		{
			return !(lhs == rhs);
		}

		template<typename Type>
		bool operator > (const Type& lhs, const Type& rhs)
		{
			return rhs < lhs;
		}

		template<typename Type>
		bool operator <= (const Type& lhs, const Type& rhs)
		{
			return !(rhs < lhs);
		}

		template<typename Type>
		bool operator >= (const Type& lhs, const Type& rhs)
		{
			return !(lhs < rhs);
		}
	}
	// 20.2.3, exchange:
	//template <class T, class U = T> T exchange(T& obj, U&& new_val);

	template <class T, class U = T> 
	T exchange(T& obj, U&& new_val)
	{
		T old_val = Yupei::move(obj);
		obj = Yupei::forward<U>(new_val);
		return old_val;
	}

	/*template <class T>
constexpr conditional_t<
!is_nothrow_move_constructible<T>::value && is_copy_constructible<T>::value,
const T&, T&&> move_if_noexcept(T& x) noexcept;*/

	template <class T>
	constexpr conditional_t<!is_nothrow_move_constructible<T>::value 
		&& is_copy_constructible<T>::value,
		const T&, 
		T&&> move_if_noexcept(T& x) noexcept
	{
		return Yupei::move(x);
	}

	// 20.5, Compile-time integer sequences
	/*template<class T, T...> struct integer_sequence;
	template<size_t... I>
	using index_sequence = integer_sequence<size_t, I...>;
	template<class T, T N>
	using make_integer_sequence = integer_sequence<T, see below >;
	template<size_t N>
	using make_index_sequence = make_integer_sequence<size_t, N>;
	template<class... T>
	using index_sequence_for = make_index_sequence<sizeof...(T)>;*/

	// 20.5, Compile-time integer sequences
	template<class T, T... I>
	struct integer_sequence
	{
		using value_type = T;
		static constexpr std::size_t size() noexcept
		{
			return sizeof...(I);
		}
	};

	namespace Internal
	{
		template<std::size_t NowCount,
			std::size_t Final,
			typename NowType>
		struct integer_seq_impl;

		template<std::size_t Final, typename T, T... I>
		struct integer_seq_impl<Final, Final, integer_sequence<T, I...>>
		{
			using type = integer_sequence<T, I...>;
		};

		template< std::size_t Now, std::size_t Final, typename T, T... I>
		struct integer_seq_impl<Now, Final, integer_sequence<T, I...>>
		{
			using type = typename integer_seq_impl<Now + 1, Final, integer_sequence<T, I..., Now>>::type;
		};

	}

	template<size_t... I>
	using index_sequence = integer_sequence<size_t, I...>;

	template<class T, T N>
	using make_integer_sequence = typename Internal::integer_seq_impl<0, N, integer_sequence<T>>::type;

	template<size_t N>
	using make_index_sequence = make_integer_sequence<size_t, N>;

	template<class... T>
	using index_sequence_for = make_index_sequence<sizeof...(T)>;

	/*template <class T1, class T2>
	struct pair {
		typedef T1 first_type;
		typedef T2 second_type;
		T1 first;
		T2 second;
		pair(const pair&) = default;
		pair(pair&&) = default;
		constexpr pair();
		EXPLICIT constexpr pair(const T1& x, const T2& y);
		template<class U, class V> EXPLICIT constexpr pair(U&& x, V&& y);
		template<class U, class V> EXPLICIT constexpr pair(const pair<U, V>& p);
		template<class U, class V> EXPLICIT constexpr pair(pair<U, V>&& p);
		template <class... Args1, class... Args2>
		pair(piecewise_construct_t,
			tuple<Args1...> first_args, tuple<Args2...> second_args);
		pair& operator=(const pair& p);
		template<class U, class V> pair& operator=(const pair<U, V>& p);
		pair& operator=(pair&& p) noexcept(see below);
		template<class U, class V> pair& operator=(pair<U, V>&& p);
		void swap(pair& p) noexcept(see below);
	};*/


	template <class T1, class T2>
	struct pair {
		typedef T1 first_type;
		typedef T2 second_type;
		T1 first;
		T2 second;
		pair(const pair&) = default;
		pair(pair&&) = default;
		constexpr pair()
			:first{}, second{}
		{

		}
		template<typename = Yupei::enable_if_t<
			is_copy_constructible<first_type>::value 
			&& is_copy_constructible<second_type>::value>>
		constexpr pair(const T1& x, const T2& y)
			:first(x),second(y)
		{

		}
		template<class U, class V,
		typename = Yupei::enable_if_t<
			is_constructible<U&&,first_type>::value &&
			is_constructible<V&&,second_type>::value>> 
		constexpr pair(U&& x, V&& y)
			noexcept(is_nothrow_constructible<T1,U&&>::value&&
				is_nothrow_constructible<T2, V&&>::value)
			:first(Yupei::forward<U>(x)),
			second(Yupei::forward<V>(y))
		{

		}
		template<class U, class V,
		typename = enable_if_t<
		is_convertible<const U&,first_type>::value &&
		is_convertible<const V&, second_type>::value>>
		constexpr pair(const pair<U, V>& p)
			:first(p.first),second(p.second)
		{

		}
		template<class U, class V,
			typename = enable_if_t<
			is_convertible<U, first_type>::value &&
			is_convertible<V, second_type>::value >>
		constexpr pair(pair<U, V>&& p)
			noexcept(is_nothrow_constructible<T1, U&&>::value&&
				is_nothrow_constructible<T2, V&&>::value)
			:first(Yupei::forward<U>(p.first)),
			second(Yupei::forward<V>(p.second))
		{

		}

		template<class TupleType1,
		class TupleType2,
			std::size_t... Indexes1,
			std::size_t... Indexes2>
			pair(TupleType1& t1, TupleType2& t2,
				index_sequence<Indexes1...>,
				index_sequence<Indexes2...>);

		template <class... Args1, class... Args2>
		pair(piecewise_construct_t,
			tuple<Args1...> first_args, tuple<Args2...> second_args);
		
		template<class U, class V> 
		pair& operator=(const pair<U, V>& p)
		{
			if (this != &p)
			{
				first = p.first;
				second = p.second;
			}
			return *this;
		}
		template<typename U,typename V>
		pair& operator=(pair<U,V>&& p) 
		{
			first = Yupei::forward<U>(p.first);
			second = Yupei::forward<V>(p.second);
			return *this;
		}
		pair& operator=(pair&& p)
		{
			first = Yupei::forward<T1>(p.first);
			second = Yupei::forward<T2>(p.second);
			return *this;
		}
		
		void swap(pair& p) noexcept(noexcept(swap(first, p.first)) &&
			noexcept(swap(second, p.second)))
		{
			swap(first, p.first);
			swap(second, p.second);
		}
	};

	template <class T1, class T2>
	constexpr bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return x.first == y.first && x.second == y.second;
	}
	template <class T1, class T2>
	constexpr bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		//if x.first < y.first,return true
		//else if x.first == y.first and x.second < y.second,return true.
		return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
	}
	
	template <class T1, class T2>
	constexpr bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return !(x == y);
	}

	template <class T1, class T2>
	constexpr bool operator>(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return y < x;
	}
	template <class T1, class T2>
	constexpr bool operator>=(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return !(x < y);
	}
	template <class T1, class T2>
	constexpr bool operator<=(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return !(y < x);
	}

	template<class T1, class T2> void swap(pair<T1, T2>& x, pair<T1, T2>& y)
		noexcept(noexcept(x.swap(y)))
	{
		x.swap(y);
	}

	namespace Internal
	{
		template<typename Type>
		struct tuple_ret_type_impl
		{
			using type = Type;
		};

		template<typename Type>
		struct tuple_ret_type_impl<Yupei::reference_wrapper<Type>>
		{
			using type = Yupei::add_lvalue_reference_t<Type>;
		};

		template<typename Type>
		using tuple_ret_type = typename tuple_ret_type_impl<decay_t<Type>>::type;

	}

	template<class T1,class T2>
	constexpr auto make_pair(T1&& x, T2&& y)
	{
		return pair < Internal::tuple_ret_type<T1>,
			Internal::tuple_ret_type < T2 >> (Yupei::forward<T1>(x),
				Yupei::forward<T2>(y));
	}

	//20.3.4, tuple-like access to pair:
	// template <class T> class tuple_size;
	// template <size_t I, class T> class tuple_element;
	// template <class T1, class T2> struct tuple_size<pair<T1, T2> >;
	// template <class T1, class T2> struct tuple_element<0, pair<T1, T2> >;
	// template <class T1, class T2> struct tuple_element<1, pair<T1, T2> >;
	// template<size_t I, class T1, class T2>
	// constexpr tuple_element_t<I, pair<T1, T2>>&
	// get(pair<T1, T2>&) noexcept;
	// template<size_t I, class T1, class T2>
	// constexpr tuple_element_t<I, pair<T1, T2>>&&
	// get(pair<T1, T2>&&) noexcept;
	// template<size_t I, class T1, class T2>
	// constexpr const tuple_element_t<I, pair<T1, T2>>&
	// get(const pair<T1, T2>&) noexcept;
	// template <class T, class U>
	// constexpr T& get(pair<T, U>& p) noexcept;
	// template <class T, class U>
	// constexpr const T& get(const pair<T, U>& p) noexcept;
	// template <class T, class U>
	// constexpr T&& get(pair<T, U>&& p) noexcept;
	// template <class T, class U>
	// constexpr T& get(pair<U, T>& p) noexcept;
	// template <class T, class U>
	// constexpr const T& get(const pair<U, T>& p) noexcept;
	// template <class T, class U>
	// constexpr T&& get(pair<U, T>&& p) noexcept;

	template<typename Type>
	class tuple_size;
	
	template<std::size_t Id,
		typename Type>
	class tuple_element;

	template<std::size_t Id,
		typename T>
		using tuple_element_t = typename tuple_element<Id, T>::type;

	template<class T1,class T2>
	struct tuple_size<pair<T1,T2>> : integral_constant<std::size_t,2>
	{

	};

	template <class T1, class T2> 
	struct tuple_element<0, pair<T1, T2> >
	{
		using type = T1;
	};
	template <class T1, class T2> 
	struct tuple_element<1, pair<T1, T2> >
	{
		using type = T2;
	};

	namespace Internal
	{
		template<std::size_t I>
		struct get_by_index_impl;

		template<>
		struct get_by_index_impl<0>
		{
			template<typename PairType>
			static constexpr decltype(auto) GetValue(PairType&& p)
			{
				return (Yupei::forward<PairType>(p).first);
			}
		};
		template<>
		struct get_by_index_impl<1>
		{
			template<typename PairType>
			static constexpr decltype(auto) GetValue(PairType&& p)
			{
				return (Yupei::forward<PairType>(p).second);
			}
		};
	}

	template<size_t I, class T1, class T2>
	constexpr const tuple_element_t<I, pair<T1, T2>>&
		get(const pair<T1, T2>& p) noexcept
	{
		return Internal::get_by_index_impl<I>::GetValue(p);
	}

	template<size_t I, class T1, class T2>
	constexpr tuple_element_t<I, pair<T1, T2>>&
		get(pair<T1, T2>& p) noexcept
	{
		return Internal::get_by_index_impl<I>::GetValue(p);
	}
	template<size_t I, class T1, class T2>
	constexpr tuple_element_t<I, pair<T1, T2>>&&
		get(pair<T1, T2>&& p) noexcept
	{
		return Internal::get_by_index_impl<I>::GetValue(Yupei::forward<pair<T1, T2>>(p));
	}

	template <class T>
	constexpr T& get(pair<T, T>& p) noexcept
	{
		static_assert(false, "can't have the same type");
		return p.first;
	}

	template <class T>
	constexpr const T& get(const pair<T, T>& p) noexcept
	{
		static_assert(false, "can't have the same type");
		return p.first;
	}

	template <class T>
	constexpr T&& get(pair<T, T>&& p) noexcept
	{
		static_assert(false, "can't have the same type");
		return p.first;
	}

	template <class T, class U>
	constexpr T& get(pair<T, U>& p) noexcept
	{
		return p.first;
	}
	template <class T, class U>
	constexpr const T& get(const pair<T, U>& p) noexcept
	{
		return p.first;
	}
	template <class T, class U>
	constexpr T&& get(pair<T, U>&& p) noexcept
	{
		return p.first;
	}
	template <class T, class U>
	constexpr T& get(pair<U, T>& p) noexcept
	{
		return (int)p.second;
	}
	template <class T, class U>
	constexpr const T& get(const pair<U, T>& p) noexcept
	{
		return p.second;
	}
	template <class T, class U>
	constexpr T&& get(pair<U, T>&& p) noexcept
	{
		return p.second;
	}

	
}
