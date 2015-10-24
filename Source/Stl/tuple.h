#pragma once

#ifdef _MSC_VER

#error "Sorry,tuple.h doesn't support MSVC."

#else


#include "type_traits.h"
#include "utility.h"

namespace Yupei
{
	//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4387.html


	struct tuple_alloc_t {};
	constexpr tuple_alloc_t tuple_alloc{};

	template<typename T1,
		typename T2>
	struct pair;

	template<typename... Args>
	class tuple;

	template<std::size_t Index,
		typename T,
		bool B = is_empty<T>::value && is_final<T>::value>
	class tuple_leaf;

	template<std::size_t Index,
		typename T>
	class tuple_leaf<Index,T,false>//cannot EBO
	{
	public:

		constexpr tuple_leaf() noexcept(
			is_nothrow_default_constructible<T>::value)
			:value_()
		{
		}

		//why we need support allocator ?

		template<typename U,
			typename = enable_if_t<!is_same<decay_t<U>,tuple_leaf>::value && 
				is_constructible<T,U>::value>>
		explicit constexpr tuple_leaf(U&& u) noexcept(
				is_nothrow_constructible<T, U>::value)
			:value_(Yupei::forward<U>(u))
		{			
		}

		tuple_leaf (const tuple_leaf&) = default;
		tuple_leaf(tuple_leaf&&) = default;

		template<typename U>
		tuple_leaf& operator=(U&& u) noexcept(
			is_nothrow_assignable<T&,U>::value)
		{
			value_ = Yupei::forward<U>(u);
		}

		int swap(tuple_leaf& rhs) noexcept
		{
			using Yupei::swap;
			swap(value_, rhs.value_);
			return 0;
		}

		T& get() noexcept
		{
			return value_;
		}

		const T& get() const noexcept
		{
			return value_;
		}

	private:
		T value_;
	};

	template<std::size_t Index,
		typename T>
	class tuple_leaf<Index,T,true> : private T //EBO enabled.
	{
	public:
		using BaseType = T;

		tuple_leaf() noexcept(
			is_nothrow_default_constructible<T>::value)
		{
		}

		template<typename U,
			typename = enable_if_t<!is_same<decay_t<U>, tuple_leaf>::value &&
			is_constructible<T, U>::value >>
			explicit tuple_leaf(U&& u) noexcept(
				is_nothrow_constructible<T, U>::value)
			:BaseType(Yupei::forward<U>(u))
		{
		}

		tuple_leaf(const tuple_leaf&) = default;
		tuple_leaf(tuple_leaf&&) = default;

		template<typename U>
		tuple_leaf& operator=(U&& u) noexcept(
			is_nothrow_assignable<T&, U>::value)
		{
			BaseType::operator = (Yupei::forward<U>(u));
		}

		int swap(tuple_leaf& rhs) noexcept
		{
			using Yupei::swap;
			swap(get(), rhs.get());
			return 0;
		}

		T& get() noexcept
		{
			return static_cast<T&>(*this);
		}

		const T& get() const noexcept
		{
			return static_cast<const T&>(*this);
		}
	};

	template<typename... Args>
	void swallow(Args&&...) noexcept {}

	struct ignore_t
	{
		template<typename U>
		const ignore_t& operator =(U&& u) const noexcept
		{
			return *this;
		}
	};

	const ignore_t ignore{};

	template<typename... Args>
	struct tuple_types {};

	template<std::size_t Index,typename TupleT>
	class tuple_element;

	template<std::size_t Index,
		typename TupleT>
	struct tuple_element<Index, const TupleT>
	{
		using type = typename tuple_element<Index, TupleT>::type;
	};

	template<std::size_t Index,
		typename TupleT>
	struct tuple_element<Index, volatile TupleT>
	{
		using type = typename tuple_element<Index, TupleT>::type;
	};

	template<std::size_t Index,
		typename TupleT>
	struct tuple_element<Index, const volatile TupleT>
	{
		using type = typename tuple_element<Index, TupleT>::type;
	};

	template<std::size_t Index>
	struct tuple_element<Index, tuple_types<>>
	{
		static_assert(Index == 0, "tuple_element index out of range.");
		static_assert(Index != 0, "tuple_element index out of range.");
	};

	template<typename T,
		typename... Args>
	struct tuple_element<0, tuple_types<T, Args...>>
	{
		using type = T;
	};

	template<std::size_t Index,
		typename T,
		typename... Args>
	struct tuple_element<Index, tuple_types<T, Args...>>
	{
		using type = typename tuple_element<Index - 1, tuple_types<Args...>>::type;
	};

	template<typename... Args>
	class tuple;

	template<std::size_t Index,typename... Args>
	struct tuple_element<Index, tuple<Args...>>
	{
		using type = typename tuple_element<Index, tuple_types<Args...>>::type;
	};

	template<std::size_t Index,
		typename TupleT>
		using tuple_element_t = typename tuple_element<Index, TupleT>::type;

	template<std::size_t Index,
		typename... Args>
		constexpr tuple_element_t<Index, tuple<Args...>>& get(tuple<Args...>&) noexcept;

	template<std::size_t Index,
		typename... Args>
		constexpr const tuple_element_t<Index, tuple<Args...>>& get(const tuple<Args...>&) noexcept;

	template<std::size_t Index,
		typename... Args>
		constexpr tuple_element_t<Index, tuple<Args...>>&& get(tuple<Args...>&&) noexcept;

	namespace Internal
	{
		template<typename ResTupleT, typename SourceTupleT, std::size_t Start, std::size_t End>
		struct make_tuple_types_impl;

		template<typename... Args,typename SourceTupleT,std::size_t End>
		struct make_tuple_types_impl<tuple_types<Args...>, SourceTupleT, End, End>
		{
			using type = tuple_types<Args...>;
		};

		template<typename... Args,typename SourceTupleT,std::size_t Start,std::size_t End>
		struct make_tuple_types_impl<tuple_types<Args...>, SourceTupleT, Start, End>
		{
			using RTupleT = remove_reference_t<SourceTupleT>;
			using type = typename make_tuple_types_impl<
				tuple_types<Args..., tuple_element_t<Start, SourceTupleT>>, SourceTupleT, Start + 1, End>::type;
		};

		template<typename T>
		class tuple_size;

		template<typename... Args>
		class tuple_size<tuple<Args...>> : public integral_constant<std::size_t,sizeof...(Args)> {};
		
		template< typename T>
		class tuple_size<const T> : public integral_constant<std::size_t, tuple_size<T>::value> {};

		template< typename T >
		class tuple_size<volatile T> : public integral_constant<std::size_t, tuple_size<T>::value> {};

		template< typename T >
		class tuple_size<const volatile T> : public integral_constant<std::size_t, tuple_size<T>::value> {};

		template<typename SourceTupleT, std::size_t Start = 0, std::size_t End = tuple_size<SourceTupleT>::value>
		struct make_tuple_types
		{
			static_assert(Start <= End, "Start > End");
			using type = typename make_tuple_types_impl<tuple_types<>, SourceTupleT, Start, End>::type;
		};

		template<typename From, typename To>
		struct tuple_convertible : false_type {};

		template<typename T,
			typename... Args,
			typename U,
			typename... UArgs>
		struct tuple_convertible<tuple_types<T, Args...>, tuple_types<U, UArgs...>>
			: bool_constant<is_convertible<T, U>::value && tuple_convertible<tuple_types<Args...>, tuple_types<UArgs...>>::value> {};

		template<>
		struct tuple_convertible<tuple_types<>, tuple_types<>> : true_type {};

		template<typename, typename>
		struct tuple_constructible : false_type {};

		template<typename T,
			typename... Args,
			typename U,
			typename... UArgs>
		struct tuple_constructible<tuple_types<T, Args...>, tuple_types<U, UArgs...>>
			: bool_constant<is_constructible<U, T>::value && tuple_constructible<tuple_types<Args...>, tuple_types<UArgs...>>::value> {};

		template<>
		struct tuple_constructible<tuple_types<>, tuple_types<>> : true_type {};

		template<typename, typename>
		struct tuple_assignable : false_type {};

		template<typename T,
			typename... Args,
			typename U,
			typename... UArgs>
		struct tuple_assignable<tuple_types<T, Args...>, tuple_types<U, UArgs...>>
			: bool_constant<is_assignable<U, T>::value && tuple_assignable<tuple_types<Args...>, tuple_types<UArgs...>>::value> {};

		template<>
		struct tuple_assignable<tuple_types<>, tuple_types<>> : true_type {};
	}


	template<typename IndexT,typename... Args>
	class tuple_impl;

	template<std::size_t... Index,
		typename... Args>
	class tuple_impl<index_sequence<Index...>,Args...>
		:public tuple_leaf<Index,Args>...
	{
	public:
		constexpr tuple_impl() noexcept(
			Internal::StaticAnd<is_nothrow_default_constructible<Args>::value...>::value )
		{}

		template<
			typename... ParamsT,
			typename = enable_if_t<sizeof...(ParamsT) == sizeof...(Args)>>
		constexpr tuple_impl(ParamsT&&... params) noexcept(
					Internal::StaticAnd<is_nothrow_constructible<Args, ParamsT>::value...>::value)
			:tuple_impl(make_index_sequence<sizeof...(ParamsT)>(),tuple_types<ParamsT...>(),Yupei::forward<ParamsT>(params)...)
			{
				
			}

		template<std::size_t... Index1,typename... TArgs,
				typename... ParamsT,
					typename = enable_if_t<sizeof...(ParamsT) == sizeof...(Args)>>
			constexpr tuple_impl(index_sequence<Index1...>, tuple_types<TArgs...>,
				ParamsT&&... params) noexcept(
					Internal::StaticAnd<is_nothrow_constructible<TArgs, ParamsT>::value...>::value)
			:tuple_leaf<Index1,Args>(Yupei::forward<TArgs>(params))...
		{
		}

		template<typename TupleT,
			typename = enable_if_t<Internal::tuple_constructible<TupleT,tuple<Args...>>::value>>
			constexpr tuple_impl(TupleT&& rhs) noexcept(Internal::StaticAnd<is_nothrow_constructible<Args,
				tuple_element_t<Index,TupleT>>::value...>::value
				)
			:tuple_leaf<Index,Args>(Yupei::forward<tuple_element_t<Index,TupleT>>(get<Index>(rhs)))...
		{
		}

		template<typename TupleT,
			typename = enable_if_t<
			Internal::tuple_assignable<TupleT,tuple<Args...>>::value>>
			tuple_impl& operator = (TupleT&& t) noexcept(
				Internal::StaticAnd<is_nothrow_assignable<Args&,tuple_element_t<Index,TupleT>>::value...>::value)
		{
			swallow(tuple_leaf<Index, Args>::operator = (Yupei::forward<tuple_element_t<Index, TupleT>>(get<Index>(t)))...);
		}

		tuple_impl(const tuple_impl&) = default;
		tuple_impl(tuple_impl&&) = default;

		tuple_impl& operator=(const tuple_impl& t)
			noexcept(Internal::StaticAnd<is_nothrow_copy_assignable<Args>::value...>::value)
		{
			swallow(tuple_leaf<Index, Args>::operator = (static_cast<const tuple_leaf<Index, Args>&>(t).get())...);
			return *this;
		}

		tuple_impl& operator=(tuple_impl&& t)
			noexcept(Internal::StaticAnd<is_nothrow_move_assignable<Args>::value...>::value)
		{
			swallow(tuple_leaf<Index, Args>::operator = (Yupei::forward<Args>(static_cast<tuple_leaf<Index,Args>&>(t).get()))...);
			return *this;
		}

		void swap(tuple_impl& rhs)
		{
			swallow(tuple_leaf<Index, Args>::swap(static_cast<tuple_leaf<Index, Args>&>(rhs))...);
		}
	};

	template<typename... Args>
	class tuple
	{
		using BaseType = tuple_impl<make_index_sequence<sizeof...(Args)>, Args...>;
		BaseType base_;
	public:
		static constexpr auto tuple_size = sizeof...(Args);

	public:
		template<std::size_t Index,
			typename... UArgs>
		friend constexpr tuple_element_t<Index, tuple<UArgs...>>& get(tuple<UArgs...>&) noexcept;

		template<std::size_t Index,
			typename... UArgs>
			friend constexpr const tuple_element_t<Index, tuple<UArgs...>>& get(const tuple<UArgs...>&) noexcept;

		template<std::size_t Index,
			typename... UArgs>
			friend constexpr tuple_element_t<Index, tuple<UArgs...>>&& get(tuple<UArgs...>&&) noexcept;

		template<typename = enable_if_t<Internal::StaticAnd<is_default_constructible<Args>::value...>::value>>
		constexpr tuple() noexcept(
			is_nothrow_default_constructible<BaseType>::value)
		{}

		template< 
			typename Dummy = void,
			enable_if_t<tuple_size >= 1 &&
			is_same<Dummy,void>::value
			&& Internal::StaticAnd<is_copy_constructible<Args>::value...>::value &&
			Internal::StaticAnd<is_convertible<const Args&,Args>::value...>::value,bool> = true>
		constexpr tuple(const Args&... args) noexcept(
				is_nothrow_constructible<BaseType, const Args&...>::value)
			:base_(args...)
		{
		}

		template<
			typename Dummy = void,
			enable_if_t<tuple_size >= 1 &&
			is_same<Dummy, void>::value
			&& Internal::StaticAnd<is_copy_constructible<Args>::value...>::value &&
			!Internal::StaticAnd<is_convertible<const Args&, Args>::value...>::value,bool> = false>
		explicit constexpr tuple(const Args&... args)
			noexcept(
				is_nothrow_constructible<BaseType, const Args&...>::value)
			:base_(args...)
		{
		}

		template<typename... UArgs,
			enable_if_t<sizeof...(Args) >= 1 && sizeof...(UArgs) == tuple_size
				&& Internal::StaticAnd<is_constructible<Args,UArgs&&>::value...>::value 
					&& Internal::StaticAnd<is_convertible<UArgs&&, Args>::value...>::value,bool> = false>
			constexpr tuple(UArgs&&... args)noexcept(
				is_nothrow_constructible<BaseType, UArgs&&...>::value)
			:base_(Yupei::forward<UArgs>(args)...)
		{
		}

		template<typename... UArgs,
			enable_if_t<tuple_size >= 1 && sizeof...(UArgs) == tuple_size && 
				Internal::StaticAnd<is_constructible<Args, UArgs&&>::value...>::value &&
					!Internal::StaticAnd<is_convertible<UArgs&&, Args>::value...>::value,bool> = true>
		explicit constexpr tuple(UArgs&&... args)
			noexcept(
				is_nothrow_constructible<BaseType,UArgs&&...>::value)
			:base_(Yupei::forward<UArgs>(args)...)
		{
		}

		tuple(const tuple&) = default;
		tuple(tuple&&) = default;

		template<typename... UArgs,
			enable_if_t<sizeof...(UArgs) == tuple_size &&
				Internal::StaticAnd<is_constructible<Args,const UArgs&>::value...>::value &&			 
					Internal::StaticAnd<is_convertible<const UArgs&,Args>::value...>::value,bool> = false>
			constexpr tuple(const tuple<UArgs...>& rhs)
			noexcept(is_nothrow_constructible<BaseType,const tuple<UArgs...>&>::value)
			:base_(rhs)
		{
		}

		template<typename... UArgs,
			enable_if_t<Internal::StaticAnd<is_constructible<Args, const UArgs&>::value...>::value&&
			sizeof...(UArgs) == tuple_size &&
			!Internal::StaticAnd<is_convertible<const UArgs&, Args>::value...>::value,bool> = true>
		explicit constexpr tuple(const tuple<UArgs...>& rhs)
			noexcept(is_nothrow_constructible<BaseType, const tuple<UArgs...>&>::value)
			:base_(rhs)
		{
		}

		template<typename... UArgs,
			enable_if_t<sizeof...(UArgs) == tuple_size &&
			Internal::StaticAnd<is_constructible<Args, UArgs&&>::value...>::value &&			
			Internal::StaticAnd<is_convertible<UArgs&&, Args>::value...>::value,bool> = false>
		constexpr tuple(tuple<UArgs...>&& rhs)
			noexcept(is_nothrow_constructible<BaseType, tuple<UArgs...>&&>::value)
			:base_(Yupei::move(rhs))
		{
		}

		template<typename... UArgs,
			enable_if_t<Internal::StaticAnd<is_constructible<Args, UArgs&&>::value...>::value&&
			sizeof...(UArgs) == tuple_size &&
			!Internal::StaticAnd<is_convertible<UArgs&&, Args>::value...>::value,bool> = false>
		explicit constexpr tuple(tuple<UArgs...>&& rhs)
			noexcept(is_nothrow_constructible<BaseType, tuple<UArgs...>&&>::value)
			:base_(Yupei::move(rhs))
		{
		}

		void swap(tuple& rhs)
		{
			base_.swap(rhs.base_);
		}
	};

	template<>
	class tuple<>
	{
		constexpr tuple() noexcept = default;

		void swap(tuple&) noexcept {}
	};

	template<typename... Args>
	void swap(tuple<Args...>& lhs, tuple<Args...>& rhs)
	{
		lhs.swap(rhs);
	}

	namespace Internal
	{	
		constexpr bool TupleAnd() noexcept
		{
			return true;
		}

		template<typename T = bool,typename... Args>
		constexpr bool TupleAnd(T b, Args... args) noexcept
		{
			return b && TupleAnd(args...);
		}

		constexpr bool TupleCompare() noexcept
		{
			return false;
		}

		template<typename T = bool, typename... Args>
		constexpr bool TupleCompare(T b, Args... args) noexcept
		{
			return b || TupleCompare(args...);
		}

		template<typename Index1T,
			typename Tuple1T,
			typename Index2T,
			typename Tuple2T>
		struct TupleIsEqualImp;

		template<std::size_t... Index1,
			typename... TArgs,
			std::size_t... Index2,
			typename... UArgs>
		struct TupleIsEqualImp<index_sequence<Index1...>, tuple<TArgs...>, index_sequence<Index2...>, tuple<UArgs...>>
		{
			static constexpr bool IsEqual(const tuple<TArgs...>& t1, const tuple<UArgs...>& t2) noexcept
			{
				return TupleAnd((get<Index1>(t1) == get<Index2>(t2))...);
			}
		};

		template<typename Tuple1T,typename Tuple2T>
		struct TupleIsEqual : TupleIsEqualImp<make_index_sequence<tuple_size<Tuple1T>::value>, Tuple1T,
			make_index_sequence<tuple_size<Tuple2T>::value>, Tuple2T> {};


		template<typename Index1T,
			typename Tuple1T,
			typename Index2T,
			typename Tuple2T>
		struct TupleIsLessImp;

		template<std::size_t... Index1,
			typename... TArgs,
			std::size_t... Index2,
			typename... UArgs>
		struct TupleIsLessImp<index_sequence<Index1...>, tuple<TArgs...>, index_sequence<Index2...>, tuple<UArgs...>>
		{
			static constexpr bool IsLess(const tuple<TArgs...>& t1, const tuple<UArgs...>& t2) noexcept
			{
				return TupleCompare((get<Index1>(t1) < get<Index2>(t2))...);
			}
		};

		template<typename Tuple1T, typename Tuple2T>
		struct TupleIsLess : TupleIsLessImp<make_index_sequence<tuple_size<Tuple1T>::value>, Tuple1T,
			make_index_sequence<tuple_size<Tuple2T>::value>, Tuple2T> {};
	}

	template<typename... Args,
		typename... UArgs>
	constexpr bool operator == (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return Internal::TupleIsEqual<tuple<Args...>, tuple<UArgs...>>::IsEqual(lhs, rhs);
	}

	template<typename... Args,
		typename... UArgs>
		constexpr bool operator < (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return Internal::TupleIsLess<tuple<Args...>, tuple<UArgs...>>::IsLess(lhs, rhs);
	}
	
	template<typename... Args,
		typename... UArgs>
		constexpr bool operator != (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<typename... Args,
		typename... UArgs>
		constexpr bool operator > (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return rhs < lhs;
	}

	template<typename... Args,
		typename... UArgs>
		constexpr bool operator >= (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return !(lhs < rhs);
	}

	template<typename... Args,
		typename... UArgs>
		constexpr bool operator <= (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return !(rhs < lhs);
	}

	template< std::size_t I, typename... Args>
	constexpr tuple_element_t<I, tuple<Args...> >& get(tuple<Args...>&t) noexcept
	{
		return static_cast<tuple_leaf<I, tuple_element_t<I, tuple<Args...>>>&>(t.base_).get();
	}

	template< std::size_t I, typename... Args>
	constexpr const tuple_element_t<I, tuple<Args...> >& get(const tuple<Args...>& t) noexcept
	{
		return static_cast<const tuple_leaf<I, tuple_element_t<I, tuple<Args...>>>&>(t.base_).get();
	}

	template< std::size_t I, typename... Args>
	constexpr tuple_element_t<I, tuple<Args...> >&& get(tuple<Args...>&& t) noexcept
	{
		return static_cast<tuple_leaf<I, tuple_element_t<I, tuple<Args...>>>&&>(t.base_).get();
	}

	template<typename... Args>
	inline constexpr tuple<Args&...> tie(Args&... t) noexcept
	{
		return tuple<Args&...>(t...);
	}

	namespace Internal
	{
		template<typename TypeToFind,typename Type,std::size_t NowIndex>
		struct get_tuple_by_type_checker;

		template<typename TypeToFind,typename Type,std::size_t NowIndex>
		struct get_tuple_by_type_impl;

		template<typename TypeToFind,typename Type>
		struct get_tuple_by_type;

		template<typename TypeToFind,typename Type,typename... Args,std::size_t NowIndex>
		struct get_tuple_by_type_impl < TypeToFind, tuple<Type, Args...>,NowIndex>
		{
			static constexpr std::size_t value = conditional_t <
				is_same<TypeToFind, Type>::value,
				get_tuple_by_type_checker<TypeToFind,tuple<Args...>,NowIndex >,//find it,check. 
				get_tuple_by_type_impl < TypeToFind, tuple<Args...>, NowIndex + 1 >
				>::value;
		};

		template<typename TypeToFind,std::size_t NowIndex>
		struct get_tuple_by_type_impl < TypeToFind, tuple<>, NowIndex>
		{
			static constexpr std::size_t value = -1;
		};

		template<typename TypeToFind,typename... Args,std::size_t NowIndex>
		struct get_tuple_by_type_checker<TypeToFind,tuple<Args...>,NowIndex>
		{
			static_assert(get_tuple_by_type_impl<TypeToFind, tuple<Args...>, 0>::value == -1, "tuple get<>() type duplicate");
			static constexpr std::size_t value = NowIndex;
		};

		template<typename TypeToFind,typename... Args>
		struct get_tuple_by_type<TypeToFind, tuple<Args...>>
		{
			static constexpr std::size_t value = get_tuple_by_type_impl<
				TypeToFind,
				tuple<Args...>,
				0>::value;
			static_assert(value != -1, "type not found.");
		};
	}

	template<typename Type, typename... Args>
	inline constexpr decltype(auto) get(tuple<Args...>&& t) noexcept
	{
		return Yupei::get<Internal::get_tuple_by_type<Type, tuple<Args...>>::value >(Yupei::forward<tuple<Args...>&&>(t));
	}

	template<typename Type, typename... Args >
	inline constexpr decltype(auto) get(tuple<Args...>& t) noexcept
	{
		return  Yupei::get<Internal::get_tuple_by_type<Type, tuple<Args...>>::value >(t);
	}

	template<typename Type, typename... Args >
	inline constexpr decltype(auto) get(const tuple<Args...>& t) noexcept
	{
		return  Yupei::get<Internal::get_tuple_by_type<Type, tuple<Args...>>::value >(t);
	}

	template<typename... Types>
	inline constexpr decltype(auto) make_tuple(Types&&... t)
	{
		return tuple<Internal::tuple_ret_type<Types>...>(Yupei::forward<Types>(t)...);
	}

	template<class... Args>
	inline constexpr tuple<Args&&...> forward_as_tuple(Args&&... t) noexcept
	{
		return tuple<Args&&...>(Yupei::forward<Args>(t)...);
	}

	template<typename T1,typename T2>
	template<typename TupleType1,
		typename TupleType2,
		std::size_t... Indexes1,
		std::size_t... Indexes2>
		pair<T1, T2>::pair(TupleType1 t1, TupleType2 t2,
			index_sequence<Indexes1...>,
			index_sequence<Indexes2...>)
		:first(get<Indexes1>(t1)...),
		second(get<Indexes2>(t2)...)
	{
	}

	template<class T1,
	class T2>
		template<class... Args1,
	class... Args2> inline
		pair<T1, T2>::pair(piecewise_construct_t,
			tuple<Args1...> Val1,
			tuple<Args2...> Val2)
		:pair(Val1, Val2, make_index_sequence<sizeof...(Args1)>(), make_index_sequence<sizeof...(Args2)>())
	{	
	}
}

#endif //_MSC_VER
   