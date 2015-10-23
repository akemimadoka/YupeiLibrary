#pragma once

#include "type_traits.h"
#include "__addressof.h"

namespace Yupei
{
	//open-std.org/JTC1/SC22/WG21/docs/papers/2015/p0090r0.htm
	//Removing result_type, etc.

	template <typename T>
	class reference_wrapper
	{
	public:
		// types
		reference_wrapper(T& arg) noexcept
			: ptr(Yupei::addressof(arg))
		{
		}

		reference_wrapper(T&&) = delete; // do not bind to temporary objects
		reference_wrapper(const reference_wrapper& x) noexcept = default;

		// assignment
		reference_wrapper& operator=(const reference_wrapper& x) noexcept = default;

		// access
		operator T& () const noexcept
		{
			return *ptr;
		}

		T& get() const noexcept
		{
			return *ptr;
		}

		// invocation
		template <class... Args>
		auto operator() (Args&&...args) const
			->decltype(Yupei::invoke(get(), Yupei::forward<Args>(args)...))
		{
			return Yupei::invoke(get(), Yupei::forward<Args>(args)...);
		}
	private:
		T* ptr;
	};

	template<typename T>
	reference_wrapper<T> ref(T& val)
	{
		return{ val };
	}

	template<typename T>
	void ref(const T&&) = delete;

	template<typename T>
	reference_wrapper<T> ref(reference_wrapper<T> val)
	{
		return Yupei::ref(val.get());
	}

	template<typename T>
	reference_wrapper<const T> cref(const T& val)
	{
		return{ val };
	}

	template<typename T>
	reference_wrapper<const T> cref(reference_wrapper<const T> val)
	{
		return Yupei::cref(val.get());
	}

	template<typename T>
	void cref(const T&&) = delete;
}