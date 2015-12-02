#pragma once

#include "utility_internal.h"
#include "type_traits.h"
#include "__reference_wrapper.h"
#include "__swap.h"
#include "integral_sequence.h"

namespace Yupei
{
	
	struct piecewise_construct_t {};
	constexpr piecewise_construct_t piecewise_construct{};

	template<typename...>
	class tuple;

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

	template <typename T, typename U = T> 
	T exchange(T& obj, U&& new_val)
	{
		T old_val = Yupei::move(obj);
		obj = Yupei::forward<U>(new_val);
		return old_val;
	}

	template <typename T>
	constexpr conditional_t<!is_nothrow_move_constructible<T>::value 
		&& is_copy_constructible<T>::value,
		const T&, 
		T&&> move_if_noexcept(T& x) noexcept
	{
		return Yupei::move(x);
	}

	//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4387.html

	template <typename T1, typename T2>
	struct pair 
	{
		using first_type = T1;
		using second_type = T2;


		first_type first;
		second_type second;


		pair(const pair&) = default;
		pair(pair&&) = default;

		constexpr pair()
			:first(), second()
		{}

		template<
			typename Dummy = void,
			enable_if_t<
				is_same<Dummy,void>::value &&
				is_copy_constructible<first_type>::value && 
				is_copy_constructible<second_type>::value &&
				is_convertible<const first_type&,first_type>::value &&
				is_convertible<const second_type&,second_type>::value,bool> = false>
		constexpr pair(const T1& x, const T2& y)
			:first(x),second(y)
		{
		}

		template<
			typename Dummy = void,
			enable_if_t<
			is_same<Dummy, void>::value &&
			is_copy_constructible<first_type>::value &&
			is_copy_constructible<second_type>::value &&
			!(is_convertible<const first_type&, first_type>::value &&
			is_convertible<const second_type&, second_type>::value),bool > = false>
		explicit constexpr pair(const T1& x, const T2& y)
			:first(x), second(y)
		{
		}

		template<typename U, typename V,
		enable_if_t<
			is_constructible<first_type,U&&>::value &&
			is_constructible<second_type,V&&>::value&&
			is_convertible<U&&, first_type>::value &&
			is_convertible<V&&, second_type>::value,bool> = true>
		constexpr pair(U&& x, V&& y)
			noexcept(is_nothrow_constructible<first_type,U&&>::value&&
				is_nothrow_constructible<second_type, V&&>::value)
			:first(Yupei::forward<U>(x)),
			second(Yupei::forward<V>(y))
		{
		}

		template<typename U, typename V,
			enable_if_t<
				is_constructible<first_type, U&&>::value &&
				is_constructible<second_type, V&&>::value&&
				!(is_convertible<U&&, first_type>::value &&
				is_convertible<V&&, second_type>::value),bool> = false>
		explicit constexpr pair(U&& x, V&& y)
			noexcept(is_nothrow_constructible<first_type, U&&>::value&&
				is_nothrow_constructible<second_type, V&&>::value)
			:first(Yupei::forward<U>(x)),
			second(Yupei::forward<V>(y))
		{
		}

		template<typename U, typename V,
			enable_if_t<is_constructible<first_type,const U&>::value &&
				is_constructible<second_type,const V&>::value  &&
				is_convertible<const U&, first_type>::value &&
				is_convertible<const V&, second_type>::value,bool> = true>
		constexpr pair(const pair<U, V>& p)
			:first(p.first),second(p.second)
		{
		}

		template<typename U, typename V,
			enable_if_t<is_constructible<first_type, const U&>::value &&
			is_constructible<second_type, const V&>::value  &&
			!(is_convertible<const U&, first_type>::value &&
			is_convertible<const V&, second_type>::value), bool> = false>
		explicit constexpr pair(const pair<U, V>& p)
			:first(p.first), second(p.second)
		{
		}

		template<typename U, typename V,
			enable_if_t<is_constructible<first_type, U&&>::value &&
				is_constructible<second_type, V&&>::value  &&
				is_convertible<U&&, first_type>::value &&
				is_convertible<V&&, second_type>::value, bool> = true>
		constexpr pair(pair<U, V>&& p)
			noexcept(is_nothrow_constructible<T1, U&&>::value&&
				is_nothrow_constructible<T2, V&&>::value)
			:first(Yupei::move(p.first)),
			second(Yupei::move(p.second))
		{
		}

		template<typename U, typename V,
			enable_if_t<is_constructible<first_type, U&&>::value &&
				is_constructible<second_type, V&&>::value  &&
				!(is_convertible<U&&, first_type>::value &&
				is_convertible<V&&, second_type>::value), bool> = false>
		explicit constexpr pair(pair<U, V>&& p)
			noexcept(is_nothrow_constructible<T1, U&&>::value&&
				is_nothrow_constructible<T2, V&&>::value)
			:first(Yupei::forward<U>(p.first)),
			second(Yupei::forward<V>(p.second))
		{
		}

		template<typename Tuple1T,
		typename Tuple2T,
			std::size_t... Indexes1,
			std::size_t... Indexes2>
			pair(Tuple1T t1, Tuple2T t2,
				index_sequence<Indexes1...>,
				index_sequence<Indexes2...>);

		template <typename... Args1, typename... Args2>
		pair(piecewise_construct_t,
			tuple<Args1...> first_args,
			tuple<Args2...> second_args);

		pair& operator= (const pair& p) noexcept(
			is_nothrow_copy_assignable<first_type>::value &&
			is_nothrow_copy_assignable<second_type>::value)
		{
			first = p.first;
			second = p.second;
		}

		pair& operator= (pair&& p) noexcept(
			is_nothrow_move_assignable<first_type>::value &&
			is_nothrow_move_assignable<second_type>::value)
		{
			first = Yupei::move(p.first);
			second = Yupei::move(p.second);
		}
		
		template<typename U, typename V> 
		pair& operator= (const pair<U, V>& p)
		{
			if (this != &p)
			{
				first = p.first;
				second = p.second;
			}
			return *this;
		}

		template<typename U,typename V>
		pair& operator= (pair<U,V>&& p) 
		{
			first = Yupei::move(p.first);
			second = Yupei::move(p.second);
			return *this;
		}


		void swap(pair& p) noexcept(noexcept(swap(first,p.first))&&
			noexcept(swap(second,p.second)))
		{
			swap(first, p.first);
			swap(second, p.second);
		}
	};

	template <typename T1, typename T2>
	constexpr bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return x.first == y.first && x.second == y.second;
	}
	
	//if x.first < y.first,return true
	//else if x.first == y.first and x.second < y.second,return true.
	template <typename T1, typename T2>
	constexpr bool operator<(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		
		return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
	}
	
	template <typename T1, typename T2>
	constexpr bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return !(x == y);
	}

	template <typename T1, typename T2>
	constexpr bool operator>(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return y < x;
	}
	template <typename T1, typename T2>
	constexpr bool operator>=(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return !(x < y);
	}
	template <typename T1, typename T2>
	constexpr bool operator<=(const pair<T1, T2>& x, const pair<T1, T2>& y)
	{
		return !(y < x);
	}

	template<typename T1, typename T2> void swap(pair<T1, T2>& x, pair<T1, T2>& y)
		noexcept(noexcept(x.swap(y)))
	{
		x.swap(y);
	}

	namespace Internal
	{
		template<typename T>
		struct tuple_ret_type_impl
		{
			using type = T;
		};

		template<typename T>
		struct tuple_ret_type_impl<Yupei::reference_wrapper<T>>
		{
			using type = Yupei::add_lvalue_reference_t<T>;
		};

		template<typename T>
		using tuple_ret_type = typename tuple_ret_type_impl<decay_t<T>>::type;

	}

	template<typename T1,typename T2>
	constexpr auto make_pair(T1&& x, T2&& y)
	{
		return pair<Internal::tuple_ret_type<T1>,Internal::tuple_ret_type<T2>>(Yupei::forward<T1>(x),
				Yupei::forward<T2>(y));
	}

	template<typename Type>
	struct tuple_size;
	
	template<std::size_t Id,typename Type>
	struct tuple_element;

	template<std::size_t Id,typename T>
	using tuple_element_t = typename tuple_element<Id, T>::type;

	template<typename T1,typename T2>
	struct tuple_size<pair<T1,T2>> : integral_constant<std::size_t,2>{};

	template <typename T1, typename T2> 
	struct tuple_element<0, pair<T1, T2> >
	{
		using type = T1;
	};

	template <typename T1, typename T2> 
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
			static constexpr decltype(auto) GetValue(PairType&& p) noexcept
			{
				return (Yupei::forward<PairType>(p).first);
			}
		};
		template<>
		struct get_by_index_impl<1>
		{
			template<typename PairType>
			static constexpr decltype(auto) GetValue(PairType&& p) noexcept
			{
				return (Yupei::forward<PairType>(p).second);
			}
		};
	}

	template<size_t I, typename T1, typename T2>
	constexpr const tuple_element_t<I, pair<T1, T2>>&
		get(const pair<T1, T2>& p) noexcept
	{
		return Internal::get_by_index_impl<I>::GetValue(p);
	}

	template<size_t I, typename T1, typename T2>
	constexpr tuple_element_t<I, pair<T1, T2>>&
		get(pair<T1, T2>& p) noexcept
	{
		return Internal::get_by_index_impl<I>::GetValue(p);
	}

	template<size_t I, typename T1, typename T2>
	constexpr tuple_element_t<I, pair<T1, T2>>&&
		get(pair<T1, T2>&& p) noexcept
	{
		return Internal::get_by_index_impl<I>::GetValue(Yupei::forward<pair<T1, T2>>(p));
	}

	template <typename T, typename U>
	constexpr T& get(pair<T, U>& p) noexcept
	{
		return p.first;
	}

	template <typename T, typename U>
	constexpr const T& get(const pair<T, U>& p) noexcept
	{
		return p.first;
	}

	template <typename T, typename U>
	constexpr T&& get(pair<T, U>&& p) noexcept
	{
		return p.first;
	}

	template <typename T, typename U>
	constexpr T& get(pair<U, T>& p) noexcept
	{
		return p.second;
	}

	template <typename T, typename U>
	constexpr const T& get(const pair<U, T>& p) noexcept
	{
		return p.second;
	}

	template <typename T, typename U>
	constexpr T&& get(pair<U, T>&& p) noexcept
	{
		return p.second;
	}

	
}
