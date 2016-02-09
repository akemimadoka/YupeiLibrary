#pragma once

#include "DefaultDelete.hpp"
#include "..\Tuple.hpp"
#include "..\TypeTraits.hpp"
#include "..\Comparators.hpp" //for less<> etc.
#include <cstddef>

namespace Yupei
{
    //http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2014/n4089.pdf

	template<typename ObjectT, typename DeleterT = default_delete<ObjectT>>
    class unique_ptr
    {
    public:
        using element_type = ObjectT;
        using deleter_type = DeleterT;
        using pointer = element_type*;

        constexpr unique_ptr() noexcept
            :data_{}
        {}

        explicit unique_ptr(pointer p) noexcept
            :data_{p, deleter_type{}}
        {}

        unique_ptr(pointer p, deleter_type d)
            :data_{p, d}
        {}

        unique_ptr(unique_ptr&& other) noexcept
            :data_{other.data_}
        {
            other.release();
        }

        constexpr unique_ptr(std::nullptr_t) noexcept
            :unique_ptr{}
        {}

        template<typename U, typename E, 
            typename = std::enable_if_t<std::is_convertible<typename unique_ptr<U, E>::pointer, pointer>{} && 
            std::is_constructible<deleter_type, E&&>{}>>
        unique_ptr(unique_ptr<U, E>&& other) noexcept
            :data_{std::move(other.data_)}
        {
            other.release();
        }

        unique_ptr& operator =(unique_ptr&& other) noexcept
        {
            unique_ptr{std::move(other)}.swap(*this);
            return *this;
        }

        template<typename U, typename E, 
            typename = std::enable_if_t<std::is_convertible<typename unique_ptr<U, E>::pointer, pointer>{} &&
            std::is_assignable<deleter_type&, E&&>{}>>
        unique_ptr& operator =(unique_ptr<U, E>&& other) noexcept
        {
            if (&other != this)
            {
                reset(other.release());
                get_deleter() = std::move(other.get_deleter());
            }
            return *this;
        }

        unique_ptr& operator =(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        ~unique_ptr()
        {
            get_deleter()(get());
            reset();
        }

        auto operator *() const -> std::add_lvalue_reference_t<element_type>
        {
            return *get();
        }

        pointer operator ->() const noexcept
        {
            return get();
        }

        deleter_type& get_deleter() noexcept
        {
            return Yupei::get<1>(data_);
        }

        const deleter_type& get_deleter() const noexcept
        {
            return Yupei::get<1>(data_);
        }

        explicit operator bool() const noexcept
        {
            return get() != nullptr;
        }

        pointer release() noexcept
        {
            return std::exchange(Yupei::get<0>(data_), nullptr);
        }
                    
        void reset(pointer ptr = {}) noexcept
        {
            Yupei::get<0>(data_) = ptr;
        }

        void swap(unique_ptr& other) noexcept
        {
            Yupei::swap(data_, other.data_);
        }

        pointer get() const noexcept
        {
            return Yupei::get<0>(data_);
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator =(const unique_ptr&) = delete;

    private:
        tuple<pointer, deleter_type> data_;
    };

    template<typename ObjectT, typename DeleterT>
    class unique_ptr<ObjectT[], DeleterT>
    {
    public:
        using element_type = ObjectT;
        using pointer = ObjectT*;
        using deleter_type = DeleterT;

        constexpr unique_ptr() noexcept
            :data_{}
        {}

        template<typename U>
        static constexpr bool CouldBeConstructed = std::is_same<U, pointer>{} ||
            (std::is_pointer<U>{} && std::is_convertible<std::remove_pointer_t<U>(*)[], element_type(*)[]>{});

        template<typename U, 
            typename = std::enable_if_t<CouldBeConstructed<U>>>
        explicit unique_ptr(U p) noexcept
            :data_{p, deleter_type{}}
        {}

        template<typename U, typename E,
            typename = std::enable_if_t<CouldBeConstructed<U> && std::is_constructible<deleter_type, E>{}>>
        unique_ptr(U p, E d) noexcept
            :data_{p, d}
        {}

        unique_ptr(unique_ptr&& other) noexcept
            : data_{std::move(other.data_)}
        {
            other.release();
        }

        template<typename U, typename E, 
            typename = std::enable_if_t<std::is_convertible<std::remove_pointer_t<U>(*)[], element_type(*)[]>{} && 
            std::is_constructible<deleter_type, E>{}>>
            unique_ptr(unique_ptr<U, E>&& other) noexcept
            :data_{std::move(other.data_)}
        {
            other.release();
        }

        constexpr unique_ptr(std::nullptr_t) noexcept
            :unique_ptr{}
        {}

        ~unique_ptr()
        {
            get_deleter()(get());
        }

        unique_ptr& operator =(unique_ptr&& other) noexcept
        {
            unique_ptr(std::move(other)).swap(*this);
            return *this;
        }

        template<typename U, typename E,
            typename = std::enable_if_t<std::is_convertible<std::remove_pointer_t<U>(*)[], element_type(*)[]>{} &&
            std::is_assignable<deleter_type&, E&&>{}>>
        unique_ptr& operator =(unique_ptr<U, E>&& other) noexcept
        {
            if (&other != this)
            {
                reset(other.release());
                get_deleter() = std::move(other.get_deleter());
            }
            return *this;
        }

        unique_ptr& operator =(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        element_type& operator [](std::size_t i) const
        {
            return get()[i];
        }

        pointer get() const noexcept
        {
            return Yupei::get<0>(data_);
        }

        deleter_type& get_deleter() noexcept
        {
            return Yupei::get<1>(data_);
        }

        const deleter_type& get_deleter() const noexcept
        {
            return Yupei::get<1>(data_);
        }

        explicit operator bool() const noexcept
        {
            return get() != nullptr;
        }

        void reset(std::nullptr_t p = nullptr) noexcept
        {
            reset(pointer{});
        }

        template<typename U, typename = std::enable_if_t<CouldBeConstructed<U>>>
        void reset(U p) noexcept
        {
            Yupei::get<0>(data_) = p;
        }

        pointer release() noexcept
        {
            return std::exchange(Yupei::get<0>(data_), nullptr);
        }

        void swap(unique_ptr& u) noexcept
        {
            Yupei::swap(data_, u.data_);
        }

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator =(const unique_ptr&) = delete;
    private:
        tuple<pointer, deleter_type> data_;
    };

	namespace Internal
	{
		template<typename T>
		struct IsIncompleteArrayType : std::false_type {};

		template<typename T>
		struct IsIncompleteArrayType<T[]> : std::true_type {};
	}

	template<typename T,typename... ParamsT,typename = std::enable_if_t<!std::is_array<T>::value>>
	inline unique_ptr<T> make_unique(ParamsT&&... params)
	{
		return unique_ptr<T>(new T(std::forward<ParamsT>(params)...));
	}

	template<typename T,typename = std::enable_if_t<Internal::IsIncompleteArrayType<T>::value>>
	inline unique_ptr<T> make_unique(std::size_t n)
	{
		return unique_ptr<T>(new std::remove_extent_t<T>[n]());
	}

	template <typename T, typename... Args>
    std::enable_if_t<std::extent<T>::value != 0>
		make_unique(Args&&...) = delete;

	template<class T, class D>
	inline void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) noexcept
	{
		x.swap(y);
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return x.get() == y.get();
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return x.get() != y.get();
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return less<common_type_t<typename unique_ptr<T1, D1>::pointer,typename unique_ptr<T2, D2>::pointer>>()(x, y);
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return !(y < x);
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return y < x;
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return !(x < y);
	}

	template <class T, class D>
	inline bool operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept
	{
		return !x;
	}

	template <class T, class D>
	inline bool operator==(std::nullptr_t, const unique_ptr<T, D>& y) noexcept
	{
		return !y;
	}

	template <class T, class D>
	inline bool operator!=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept
	{
		return static_cast<bool>(x);
	}

	template <class T, class D>
	inline bool operator!=(std::nullptr_t, const unique_ptr<T, D>& y) noexcept
	{
		return static_cast<bool>(y);
	}

	template <class T, class D>
	inline bool operator<(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return less<typename unique_ptr<T, D>::pointer>()(x.get(), nullptr);
	}

	template <class T, class D>
	inline bool operator<(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return less<typename unique_ptr<T, D>::pointer>()(nullptr, y.get());
	}

	template <class T, class D>
	inline bool operator<=(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return !(nullptr < x);
	}

	template <class T, class D>
	inline bool operator<=(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return !(y < nullptr);
	}

	template <class T, class D>
	inline bool operator>(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return nullptr < x;
	}

	template <class T, class D>
	inline bool operator>(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return y < nullptr;
	}

	template <class T, class D>
	inline bool operator>=(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return !(x < nullptr);
	}

	template <class T, class D>
	inline bool operator>=(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return !(nullptr < y);
	}
}
