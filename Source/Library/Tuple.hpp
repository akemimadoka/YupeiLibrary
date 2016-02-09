#pragma once

#include "TypeTraits.hpp"
#include "Utility.hpp"

namespace Yupei
{
	//http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4387.html

	template<typename T1, typename T2>
	struct pair;

	template<typename... Args>
	class tuple;

    namespace Internal
    {
        template<std::size_t Index, typename T, bool B = std::is_empty<T>::value && std::is_final<T>::value>
        class TupleLeaf;

        template<std::size_t Index,
            typename T>
        class TupleLeaf<Index, T, false>//cannot EBO
        {
        public:
            constexpr TupleLeaf() noexcept(std::is_nothrow_default_constructible<T>::value)
                :value_()
            {
            }

            //why we need support allocator ?

            template<typename U,
                typename = std::enable_if_t<!std::is_same<std::decay_t<U>, TupleLeaf>::value && std::is_constructible<T, U>::value >>
                explicit constexpr TupleLeaf(U&& u) noexcept(
                    std::is_nothrow_constructible<T, U>::value)
                :value_(std::forward<U>(u))
            {
            }

            TupleLeaf(const TupleLeaf&) = default;
            TupleLeaf(TupleLeaf&&) = default;

            template<typename U>
            TupleLeaf& operator=(U&& u) noexcept(
                std::is_nothrow_assignable<T&, U>::value)
            {
                value_ = std::forward<U>(u);
            }

            int swap(TupleLeaf& rhs) noexcept
            {
                using std::swap;
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

        template<std::size_t Index, typename T>
        class TupleLeaf<Index, T, true> : private T //EBO enabled.
        {
        public:
            using BaseType = T;

            TupleLeaf() noexcept(std::is_nothrow_default_constructible<T>::value)
            {}

            template<typename U,
                typename = std::enable_if_t<!std::is_same<std::decay_t<U>, TupleLeaf>::value && std::is_constructible<T, U>::value >>
            explicit TupleLeaf(U&& u) noexcept(std::is_nothrow_constructible<T, U>::value)
                :BaseType(std::forward<U>(u))
            {}

            TupleLeaf(const TupleLeaf&) = default;
            TupleLeaf(TupleLeaf&&) = default;

            template<typename U>
            TupleLeaf& operator=(U&& u) noexcept(std::is_nothrow_assignable<T&, U>::value)
            {
                BaseType::operator = (std::forward<U>(u));
                return *this;
            }

            //TODO: noexcept swappable ?
            int swap(TupleLeaf& rhs) noexcept
            {
                using std::swap;
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

    }
	
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

	template<std::size_t Index, typename TupleT>
	struct tuple_element;

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

	template<std::size_t Index, typename TupleT>
	using tuple_element_t = typename tuple_element<Index, TupleT>::type;

	template<std::size_t Index, typename... Args>
	constexpr tuple_element_t<Index, tuple<Args...>>& get(tuple<Args...>&) noexcept;

	template<std::size_t Index, typename... Args>
	constexpr const tuple_element_t<Index, tuple<Args...>>& get(const tuple<Args...>&) noexcept;

	template<std::size_t Index, typename... Args>
	constexpr tuple_element_t<Index, tuple<Args...>>&& get(tuple<Args...>&&) noexcept;

	namespace Internal
	{
		template<typename ResTupleT, typename SourceTupleT, std::size_t Start, std::size_t End>
		struct MakeTupleTypesImpl;

		template<typename... Args, typename SourceTupleT, std::size_t End>
		struct MakeTupleTypesImpl<tuple_types<Args...>, SourceTupleT, End, End>
		{
			using type = tuple_types<Args...>;
		};

		template<typename... Args, typename SourceTupleT, std::size_t Start, std::size_t End>
		struct MakeTupleTypesImpl<tuple_types<Args...>, SourceTupleT, Start, End>
		{
			using RTupleT = std::remove_reference_t<SourceTupleT>;
			using type = typename MakeTupleTypesImpl<
				tuple_types<Args..., tuple_element_t<Start, SourceTupleT>>, SourceTupleT, Start + 1, End>::type;
		};
		
		template<typename SourceTupleT, std::size_t Start = 0, std::size_t End = tuple_size<SourceTupleT>::value>
		struct MakeTupleTypes
		{
			static_assert(Start <= End, "Start > End");
			using type = typename MakeTupleTypesImpl<tuple_types<>, SourceTupleT, Start, End>::type;
		};

		template<typename From, typename To>
		struct TupleConvertible : std::false_type {};

		template<typename T,
			typename... Args,
			typename U,
			typename... UArgs>
		struct TupleConvertible<tuple_types<T, Args...>, tuple_types<U, UArgs...>>
			: bool_constant<std::is_convertible<T, U>::value && TupleConvertible<tuple_types<Args...>, tuple_types<UArgs...>>::value> {};

		template<>
		struct TupleConvertible<tuple_types<>, tuple_types<>> : std::true_type {};

		template<typename, typename>
		struct TupleConstructible : std::false_type {};

		template<typename T,
			typename... Args,
			typename U,
			typename... UArgs>
		struct TupleConstructible<tuple_types<T, Args...>, tuple_types<U, UArgs...>>
			: bool_constant<std::is_constructible<U, T>::value && TupleConstructible<tuple_types<Args...>, tuple_types<UArgs...>>::value> {};

		template<>
		struct TupleConstructible<tuple_types<>, tuple_types<>> : std::true_type {};

		template<typename, typename>
		struct TupleAssignable : std::false_type {};

		template<typename T,
			typename... Args,
			typename U,
			typename... UArgs>
		struct TupleAssignable<tuple_types<T, Args...>, tuple_types<U, UArgs...>>
			: bool_constant<std::is_assignable<U, T>::value && TupleAssignable<tuple_types<Args...>, tuple_types<UArgs...>>::value> {};

		template<>
		struct TupleAssignable<tuple_types<>, tuple_types<>> : std::true_type {};

        template<typename IndexT, typename... Args>
        class TupleImpl;

        template<std::size_t... Index,
            typename... Args>
        class TupleImpl<std::index_sequence<Index...>, Args...>
            :public TupleLeaf<Index, Args>...
        {
        public:
            constexpr TupleImpl() noexcept(static_and<std::is_nothrow_default_constructible<Args>::value...>::value)
            {}

            template<
                typename... ParamsT,
                typename = std::enable_if_t<sizeof...(ParamsT) == sizeof...(Args)>>
            constexpr TupleImpl(ParamsT&&... params) 
                    noexcept(static_and<std::is_nothrow_constructible<Args, ParamsT>::value...>::value)
                :TupleImpl(std::make_index_sequence<sizeof...(ParamsT)>(), tuple_types<ParamsT...>(), std::forward<ParamsT>(params)...)
            {}

            template<std::size_t... Index1, typename... TArgs, typename... ParamsT,
                typename = std::enable_if_t<sizeof...(ParamsT) == sizeof...(Args)>>
            constexpr TupleImpl(std::index_sequence<Index1...>, tuple_types<TArgs...>, ParamsT&&... params) 
                noexcept(static_and<std::is_nothrow_constructible<TArgs, ParamsT>::value...>::value)
                :TupleLeaf<Index1, Args>(std::forward<TArgs>(params))...
            {
            }

            template<typename TupleT,
                typename = std::enable_if_t<Internal::TupleConstructible<TupleT, tuple<Args...>>::value >>
             constexpr TupleImpl(TupleT&& rhs) 
                noexcept(static_and<std::is_nothrow_constructible<Args, tuple_element_t<Index, TupleT>> ::value...>::value)
                :TupleLeaf<Index, Args>(std::forward<tuple_element_t<Index, TupleT>>(get<Index>(rhs)))...
            {
            }

            template<typename TupleT,
                typename = std::enable_if_t<TupleAssignable<TupleT, tuple<Args...>>::value>>
            TupleImpl& operator = (TupleT&& t) 
                noexcept(static_and<std::is_nothrow_assignable<Args&, tuple_element_t<Index, TupleT>>::value...>::value)
            {
                swallow(TupleLeaf<Index, Args>::operator = (std::forward<tuple_element_t<Index, TupleT>>(get<Index>(t)))...);
            }

            TupleImpl(const TupleImpl&) = default;
            TupleImpl(TupleImpl&&) = default;

            TupleImpl& operator=(const TupleImpl& t)
                noexcept(static_and<std::is_nothrow_copy_assignable<Args>::value...>::value)
            {
                swallow(TupleLeaf<Index, Args>::operator = (static_cast<const TupleLeaf<Index, Args>&>(t).get())...);
                return *this;
            }

            TupleImpl& operator=(TupleImpl&& t)
                noexcept(static_and<std::is_nothrow_move_assignable<Args>::value...>::value)
            {
                swallow(TupleLeaf<Index, Args>::operator = (std::forward<Args>(static_cast<TupleLeaf<Index, Args>&>(t).get()))...);
                return *this;
            }

            void swap(TupleImpl& rhs)
            {
                swallow(TupleLeaf<Index, Args>::swap(static_cast<TupleLeaf<Index, Args>&>(rhs))...);
            }
        };
	}

    template<typename T>
    class tuple_size;

    template<typename... Args>
    class tuple_size<tuple<Args...>> : public size_constant<sizeof...(Args)> {};

    template< typename T>
    class tuple_size<const T> : public size_constant<tuple_size<T>::value> {};

    template< typename T >
    class tuple_size<volatile T> : public size_constant<tuple_size<T>::value> {};

    template< typename T >
    class tuple_size<const volatile T> : public size_constant<tuple_size<T>::value> {};

	template<typename... Args>
	class tuple
	{
		using BaseType = Internal::TupleImpl<std::make_index_sequence<sizeof...(Args)>, Args...>;
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

		template<typename Dummy = void, 
			typename = std::enable_if_t<std::is_same<Dummy,void>::value && 
            static_and<std::is_default_constructible<Args>::value...>::value>>
		constexpr tuple() 
            noexcept(std::is_nothrow_default_constructible<BaseType>::value)
		{}

		template< typename Dummy = void,
			std::enable_if_t<tuple_size >= 1 &&
            std::is_same<Dummy,void>::value
			&& static_and<std::is_copy_constructible<Args>::value...>::value &&
			static_and<std::is_convertible<const Args&,Args>::value...>::value,bool> = true>
		constexpr tuple(const Args&... args) 
            noexcept(std::is_nothrow_constructible<BaseType, const Args&...>::value)
			:base_(args...)
		{}

		template<typename Dummy = void,
			std::enable_if_t<tuple_size >= 1 &&
            std::is_same<Dummy, void>::value
			&& static_and<std::is_copy_constructible<Args>::value...>::value &&
			!static_and<std::is_convertible<const Args&, Args>::value...>::value,bool> = false>
		explicit constexpr tuple(const Args&... args)
			noexcept(std::is_nothrow_constructible<BaseType, const Args&...>::value)
			:base_(args...)
		{}

		template<typename... UArgs,
			std::enable_if_t<sizeof...(Args) >= 1 && sizeof...(UArgs) == tuple_size
				&& static_and<std::is_constructible<Args,UArgs&&>::value...>::value
					&& static_and<std::is_convertible<UArgs&&, Args>::value...>::value,bool> = false>
			constexpr tuple(UArgs&&... args)noexcept(
                std::is_nothrow_constructible<BaseType, UArgs&&...>::value)
			:base_(std::forward<UArgs>(args)...)
		{}

		template<typename... UArgs,
			std::enable_if_t<tuple_size >= 1 && sizeof...(UArgs) == tuple_size && 
				static_and<std::is_constructible<Args, UArgs&&>::value...>::value &&
					!static_and<std::is_convertible<UArgs&&, Args>::value...>::value,bool> = true>
		explicit constexpr tuple(UArgs&&... args)
			noexcept(std::is_nothrow_constructible<BaseType,UArgs&&...>::value)
			:base_(std::forward<UArgs>(args)...)
		{}

		tuple(const tuple&) = default;
		tuple(tuple&&) = default;

		template<typename... UArgs,
			std::enable_if_t<sizeof...(UArgs) == tuple_size &&
				static_and<std::is_constructible<Args,const UArgs&>::value...>::value &&
					static_and<std::is_convertible<const UArgs&,Args>::value...>::value,bool> = false>
	    constexpr tuple(const tuple<UArgs...>& rhs)
			noexcept(std::is_nothrow_constructible<BaseType,const tuple<UArgs...>&>::value)
			:base_(rhs)
		{}

		template<typename... UArgs,
			std::enable_if_t<static_and<std::is_constructible<Args, const UArgs&>::value...>::value &&
			sizeof...(UArgs) == tuple_size &&
			!static_and<std::is_convertible<const UArgs&, Args>::value...>::value,bool> = true>
		explicit constexpr tuple(const tuple<UArgs...>& rhs)
			noexcept(std::is_nothrow_constructible<BaseType, const tuple<UArgs...>&>::value)
			:base_(rhs)
		{}

		template<typename... UArgs,
			std::enable_if_t<sizeof...(UArgs) == tuple_size &&
			static_and<std::is_constructible<Args, UArgs&&>::value...>::value &&
			static_and<std::is_convertible<UArgs&&, Args>::value...>::value, bool> = false>
		constexpr tuple(tuple<UArgs...>&& rhs)
			noexcept(std::is_nothrow_constructible<BaseType, tuple<UArgs...>&&>::value)
			:base_(std::move(rhs))
		{}

		template<typename... UArgs,
			std::enable_if_t<static_and<std::is_constructible<Args, UArgs&&>::value...>::value&&
			sizeof...(UArgs) == tuple_size &&
			!static_and<std::is_convertible<UArgs&&, Args>::value...>::value, bool> = false>
		explicit constexpr tuple(tuple<UArgs...>&& rhs)
			noexcept(std::is_nothrow_constructible<BaseType, tuple<UArgs...>&&>::value)
			:base_(std::move(rhs))
		{}

        //TODO noexcept?
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

		template<typename Index1T, typename Tuple1T, typename Index2T, typename Tuple2T>
		struct TupleIsEqualImp;

		template<std::size_t... Index1, typename... TArgs, std::size_t... Index2, typename... UArgs>
		struct TupleIsEqualImp<std::index_sequence<Index1...>, tuple<TArgs...>, std::index_sequence<Index2...>, tuple<UArgs...>>
		{
			static constexpr bool IsEqual(const tuple<TArgs...>& t1, const tuple<UArgs...>& t2) noexcept
			{
				return TupleAnd((get<Index1>(t1) == get<Index2>(t2))...);
			}
		};

		template<typename Tuple1T,typename Tuple2T>
		struct TupleIsEqual : 
            TupleIsEqualImp<std::make_index_sequence<tuple_size<Tuple1T>::value>, Tuple1T, 
            std::make_index_sequence<tuple_size<Tuple2T>::value>, Tuple2T> {};


		template<typename Index1T, typename Tuple1T, typename Index2T, typename Tuple2T>
		struct TupleIsLessImp;

		template<std::size_t... Index1, typename... TArgs, std::size_t... Index2, typename... UArgs>
		struct TupleIsLessImp<std::index_sequence<Index1...>, tuple<TArgs...>, std::index_sequence<Index2...>, tuple<UArgs...>>
		{
			static constexpr bool IsLess(const tuple<TArgs...>& t1, const tuple<UArgs...>& t2) noexcept
			{
				return TupleCompare((get<Index1>(t1) < get<Index2>(t2))...);
			}
		};

		template<typename Tuple1T, typename Tuple2T>
		struct TupleIsLess : TupleIsLessImp<std::make_index_sequence<tuple_size<Tuple1T>::value>, Tuple1T,
            std::make_index_sequence<tuple_size<Tuple2T>::value>, Tuple2T> {};
	}

	template<typename... Args, typename... UArgs>
	constexpr bool operator == (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return Internal::TupleIsEqual<tuple<Args...>, tuple<UArgs...>>::IsEqual(lhs, rhs);
	}

	template<typename... Args, typename... UArgs>
	constexpr bool operator < (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return Internal::TupleIsLess<tuple<Args...>, tuple<UArgs...>>::IsLess(lhs, rhs);
	}
	
	template<typename... Args, typename... UArgs>
	constexpr bool operator != (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return !(lhs == rhs);
	}

	template<typename... Args, typename... UArgs>
	constexpr bool operator > (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return rhs < lhs;
	}

	template<typename... Args, typename... UArgs>
	constexpr bool operator >= (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return !(lhs < rhs);
	}

	template<typename... Args, typename... UArgs>
	constexpr bool operator <= (const tuple<Args...>& lhs, const tuple<UArgs...>& rhs) noexcept
	{
		return !(rhs < lhs);
	}

	template<std::size_t I, typename... Args>
	constexpr tuple_element_t<I, tuple<Args...> >& get(tuple<Args...>&t) noexcept
	{
		return static_cast<Internal::TupleLeaf<I, tuple_element_t<I, tuple<Args...>>>&>(t.base_).get();
	}

	template<std::size_t I, typename... Args>
	constexpr const tuple_element_t<I, tuple<Args...> >& get(const tuple<Args...>& t) noexcept
	{
		return static_cast<const Internal::TupleLeaf<I, tuple_element_t<I, tuple<Args...>>>&>(t.base_).get();
	}

	template<std::size_t I, typename... Args>
	constexpr tuple_element_t<I, tuple<Args...> >&& get(tuple<Args...>&& t) noexcept
	{
		return static_cast<Internal::TupleLeaf<I, tuple_element_t<I, tuple<Args...>>>&&>(t.base_).get();
	}

	template<typename... Args>
	inline constexpr tuple<Args&...> tie(Args&... t) noexcept
	{
		return tuple<Args&...>(t...);
	}

	namespace Internal
	{
		template<typename TypeToFind, typename Type, std::size_t NowIndex>
		struct GetTupleByTypeChecker;

		template<typename TypeToFind, typename Type, std::size_t NowIndex>
		struct GetTupleByTypeImpl;

		template<typename TypeToFind, typename Type>
		struct GetTupleByType;

		template<typename TypeToFind, typename Type, typename... Args, std::size_t NowIndex>
		struct GetTupleByTypeImpl<TypeToFind, tuple<Type, Args...>, NowIndex>
		{
			static constexpr std::size_t value = std::conditional_t<
                std::is_same<TypeToFind, Type>::value,
				GetTupleByTypeChecker<TypeToFind,tuple<Args...>,NowIndex>,//find it,check. 
				GetTupleByTypeImpl < TypeToFind, tuple<Args...>, NowIndex + 1>>::value;
		};

		template<typename TypeToFind,std::size_t NowIndex>
		struct GetTupleByTypeImpl<TypeToFind, tuple<>, NowIndex>
		{
			static constexpr std::size_t value = -1;
		};

		template<typename TypeToFind, typename... Args, std::size_t NowIndex>
		struct GetTupleByTypeChecker<TypeToFind, tuple<Args...>, NowIndex>
		{
			static_assert(GetTupleByTypeImpl<TypeToFind, tuple<Args...>, 0>::value == -1, "tuple get<>() type duplicate");
			static constexpr std::size_t value = NowIndex;
		};

		template<typename TypeToFind,typename... Args>
		struct GetTupleByType<TypeToFind, tuple<Args...>>
		{
			static constexpr std::size_t value = GetTupleByTypeImpl<TypeToFind,tuple<Args...>,0>::value;
			static_assert(value != -1, "type not found.");
		};
	}

	template<typename Type, typename... Args>
	inline constexpr decltype(auto) get(tuple<Args...>&& t) noexcept
	{
		return Yupei::get<Internal::GetTupleByType<Type, tuple<Args...>>::value>(std::forward<tuple<Args...>&&>(t));
	}

	template<typename Type, typename... Args >
	inline constexpr decltype(auto) get(tuple<Args...>& t) noexcept
	{
		return Yupei::get<Internal::GetTupleByType<Type, tuple<Args...>>::value>(t);
	}

	template<typename Type, typename... Args >
	inline constexpr decltype(auto) get(const tuple<Args...>& t) noexcept
	{
		return Yupei::get<Internal::GetTupleByType<Type, tuple<Args...>>::value>(t);
	}

	template<typename... Types>
	inline constexpr decltype(auto) make_tuple(Types&&... t)
	{
		return tuple<Internal::TupleRetType<Types>...>(std::forward<Types>(t)...);
	}

	template<class... Args>
	inline constexpr tuple<Args&&...> forward_as_tuple(Args&&... t) noexcept
	{
		return tuple<Args&&...>(std::forward<Args>(t)...);
	}

	template<typename T1,typename T2>
	template<typename TupleType1, typename TupleType2, std::size_t... Indexes1, std::size_t... Indexes2>
	inline pair<T1, T2>::pair(TupleType1 t1, TupleType2 t2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
		:first(get<Indexes1>(t1)...), second(get<Indexes2>(t2)...)
	{}

	template<typename T1, typename T2>
	template<typename... Args1, typename... Args2> 
    inline pair<T1, T2>::pair(piecewise_construct_t, tuple<Args1...> val1, tuple<Args2...> val2)
        :pair(val1, val2, std::make_index_sequence<sizeof...(Args1)>{}, std::make_index_sequence<sizeof...(Args2)>{})
	{}
}

   