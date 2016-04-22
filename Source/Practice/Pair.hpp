#pragma once

#include <utility>
#include <type_traits>

namespace Yupei
{
    struct piecewise_construct_t {};
    constexpr piecewise_construct_t piecewise_construct{};

    template<typename... Args>
    class tuple;

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

        template<typename Dummy = void, std::enable_if_t<std::is_same<Dummy, void>::value &&
            std::is_copy_constructible<first_type>::value &&
            std::is_copy_constructible<second_type>::value &&
            std::is_convertible<const first_type&, first_type>::value &&
            std::is_convertible<const second_type&, second_type>::value, bool> = false>
        constexpr pair(const T1& x, const T2& y)
            :first(x), second(y)
        {
        }

        template<typename Dummy = void, std::enable_if_t<std::is_same<Dummy, void>::value &&
            std::is_copy_constructible<first_type>::value &&
            std::is_copy_constructible<second_type>::value &&
            !(std::is_convertible<const first_type&, first_type>::value &&
                std::is_convertible<const second_type&, second_type>::value), bool> = false>
        explicit constexpr pair(const T1& x, const T2& y)
            :first(x), second(y)
        {
        }

        template<typename U, typename V, std::enable_if_t<
            std::is_constructible<first_type, U&&>::value &&
            std::is_constructible<second_type, V&&>::value&&
            std::is_convertible<U&&, first_type>::value &&
            std::is_convertible<V&&, second_type>::value, bool> = true>
        constexpr pair(U&& x, V&& y)
            noexcept(std::is_nothrow_constructible<first_type, U&&>::value &&
                std::is_nothrow_constructible<second_type, V&&>::value)
            :first(std::forward<U>(x)),
            second(std::forward<V>(y))
        {
        }

        template<typename U, typename V,
            std::enable_if_t<
            std::is_constructible<first_type, U&&>::value &&
            std::is_constructible<second_type, V&&>::value&&
            !(std::is_convertible<U&&, first_type>::value &&
                std::is_convertible<V&&, second_type>::value), bool> = false>
        explicit constexpr pair(U&& x, V&& y)
            noexcept(std::is_nothrow_constructible<first_type, U&&>::value&&
                std::is_nothrow_constructible<second_type, V&&>::value)
            :first(std::forward<U>(x)),
            second(std::forward<V>(y))
        {
        }

        template<typename U, typename V, std::enable_if_t<
            std::is_constructible<first_type, const U&>::value &&
            std::is_constructible<second_type, const V&>::value &&
            std::is_convertible<const U&, first_type>::value &&
            std::is_convertible<const V&, second_type>::value, bool> = true>
        constexpr pair(const pair<U, V>& p)
            :first(p.first), second(p.second)
        {
        }

        template<typename U, typename V, std::enable_if_t<
            std::is_constructible<first_type, const U&>::value &&
            std::is_constructible<second_type, const V&>::value  &&
            !(std::is_convertible<const U&, first_type>::value &&
                std::is_convertible<const V&, second_type>::value), bool> = false>
        explicit constexpr pair(const pair<U, V>& p)
            :first(p.first), second(p.second)
        {
        }

        template<typename U, typename V, std::enable_if_t<
            std::is_constructible<first_type, U&&>::value &&
            std::is_constructible<second_type, V&&>::value  &&
            std::is_convertible<U&&, first_type>::value &&
            std::is_convertible<V&&, second_type>::value, bool> = true>
        pair(pair<U, V>&& p)
            noexcept(std::is_nothrow_constructible<T1, U&&>::value&&
                std::is_nothrow_constructible<T2, V&&>::value)
            :first(std::move(p.first)), second(std::move(p.second))
        {
        }

        template<typename U, typename V,
            std::enable_if_t<std::is_constructible<first_type, U&&>::value &&
            std::is_constructible<second_type, V&&>::value  &&
            !(std::is_convertible<U&&, first_type>::value &&
                std::is_convertible<V&&, second_type>::value), bool> = false>
        explicit pair(pair<U, V>&& p)
            noexcept(std::is_nothrow_constructible<T1, U&&>::value &&
                std::is_nothrow_constructible<T2, V&&>::value)
            :first(std::forward<U>(p.first)),
            second(std::forward<V>(p.second))
        {
        }

        template<typename Tuple1T, typename Tuple2T, std::size_t... Indexes1, std::size_t... Indexes2>
        pair(Tuple1T t1, Tuple2T t2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>);

        template <typename... Args1, typename... Args2>
        pair(piecewise_construct_t, tuple<Args1...> val1, tuple<Args2...> val2);

        pair& operator =(const pair& p)
            noexcept(std::is_nothrow_copy_assignable<first_type>::value &&
                std::is_nothrow_copy_assignable<second_type>::value)
        {
            first = p.first;
            second = p.second;
        }

        pair& operator= (pair&& p)
            noexcept(std::is_nothrow_move_assignable<first_type>::value &&
                std::is_nothrow_move_assignable<second_type>::value)
        {
            first = std::move(p.first);
            second = std::move(p.second);
            return *this;
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

        template<typename U, typename V>
        pair& operator= (pair<U, V>&& p)
        {
            first = std::move(p.first);
            second = std::move(p.second);
            return *this;
        }

        void swap(pair& p) noexcept(noexcept(swap(first, p.first)) && noexcept(swap(second, p.second)))
        {
            using std::swap;
            swap(first, p.first);
            swap(second, p.second);
        }
    };

    template <typename T1, typename T2>
    constexpr bool operator ==(const pair<T1, T2>& x, const pair<T1, T2>& y) noexcept
    {
        return x.first == y.first && x.second == y.second;
    }

    template <typename T1, typename T2>
    constexpr bool operator <(const pair<T1, T2>& x, const pair<T1, T2>& y) noexcept
    {
        return x.first < y.first || (!(y.first < x.first) && x.second < y.second);
    }

    template <typename T1, typename T2>
    constexpr bool operator !=(const pair<T1, T2>& x, const pair<T1, T2>& y) noexcept
    {
        return !(x == y);
    }

    template <typename T1, typename T2>
    constexpr bool operator >(const pair<T1, T2>& x, const pair<T1, T2>& y) noexcept
    {
        return y < x;
    }
    template <typename T1, typename T2>
    constexpr bool operator >=(const pair<T1, T2>& x, const pair<T1, T2>& y) noexcept
    {
        return !(x < y);
    }

    template <typename T1, typename T2>
    constexpr bool operator <=(const pair<T1, T2>& x, const pair<T1, T2>& y) noexcept
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
        struct TupleRetTypeImpl
        {
            using type = T;
        };

        template<typename T>
        struct TupleRetTypeImpl<std::reference_wrapper<T>>
        {
            using type = std::add_lvalue_reference_t<T>;
        };

        template<typename T>
        using TupleRetType = typename TupleRetTypeImpl<std::decay_t<T>>::type;
    }

    template<typename T1, typename T2>
    constexpr auto make_pair(T1&& x, T2&& y)
    {
        return pair<Internal::TupleRetType<T1>, Internal::TupleRetType<T2>>(std::forward<T1>(x), std::forward<T2>(y));
    }

    template<typename Type>
    struct tuple_size;

    template<std::size_t Id, typename Type>
    struct tuple_element;

	template<std::size_t Index, typename TupleT>
	struct tuple_element<Index, const TupleT>
	{
		using type = typename tuple_element<Index, TupleT>::type;
	};

	template<std::size_t Index, typename TupleT>
	struct tuple_element<Index, volatile TupleT>
	{
		using type = typename tuple_element<Index, TupleT>::type;
	};

	template<std::size_t Index, typename TupleT>
	struct tuple_element<Index, const volatile TupleT>
	{
		using type = typename tuple_element<Index, TupleT>::type;
	};

    template<std::size_t Id, typename T>
    using tuple_element_t = typename tuple_element<Id, T>::type;

    template<typename T1, typename T2>
    struct tuple_size<pair<T1, T2>> : size_constant<2> {};

    template <typename T1, typename T2>
    struct tuple_element<0, pair<T1, T2>>
    {
        using type = T1;
    };

    template <typename T1, typename T2>
    struct tuple_element<1, pair<T1, T2>>
    {
        using type = T2;
    };

    namespace Internal
    {
		template<typename T1, typename T2>
		constexpr const tuple_element_t<0, pair<T1, T2>>&
			GetImpl(const pair<T1, T2>& p, size_constant<0>) noexcept
		{
			return p.first;
		}

		template<typename T1, typename T2>
		constexpr const tuple_element_t<1, pair<T1, T2>>&
			GetImpl(const pair<T1, T2>& p, size_constant<1>) noexcept
		{
			return p.second;
		}

		template<typename T1, typename T2>
		constexpr tuple_element_t<0, pair<T1, T2>>&
			GetImpl(pair<T1, T2>& p, size_constant<0>) noexcept
		{
			return p.first;
		}

		template<typename T1, typename T2>
		constexpr tuple_element_t<1, pair<T1, T2>>&
			GetImpl(pair<T1, T2>& p, size_constant<1>) noexcept
		{
			return p.second;
		}

		template<typename T1, typename T2>
		constexpr tuple_element_t<0, pair<T1, T2>>&&
			GetImpl(pair<T1, T2>&& p, size_constant<0>) noexcept
		{
			return std::move(p.first);
		}

		template<typename T1, typename T2>
		constexpr tuple_element_t<1, pair<T1, T2>>&&
			GetImpl(pair<T1, T2>&& p, size_constant<1>) noexcept
		{
			return std::move(p.second);
		}
    }

    template<size_t I, typename T1, typename T2>
    constexpr const tuple_element_t<I, pair<T1, T2>>&
        get(const pair<T1, T2>& p) noexcept
    {
		return Internal::GetImpl(p, size_constant<I>{});
    }

    template<size_t I, typename T1, typename T2>
    constexpr tuple_element_t<I, pair<T1, T2>>&
        get(pair<T1, T2>& p) noexcept
    {
		return Internal::GetImpl(p, size_constant<I>{});
    }

    template<size_t I, typename T1, typename T2>
    constexpr tuple_element_t<I, pair<T1, T2>>&&
        get(pair<T1, T2>&& p) noexcept
    {
		return Internal::GetImpl(p, size_constant<I>{});
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