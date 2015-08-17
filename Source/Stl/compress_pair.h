#pragma once

#include "type_traits.h"
#include "utility_internal.h"

#pragma warning(push)

#pragma warning(disable:4172) //returning address of local variable or temporary

namespace Yupei
{
	template<
		typename T1,
		typename T2,
		bool = Yupei::is_empty<T1>::value &&
		!Yupei::is_final<T1>::value
	>
	struct compress_pair;

	struct compress_value_initialize_first_arg_t {};

	constexpr compress_value_initialize_first_arg_t compress_value_initialize_first_arg{};

	struct compress_value_initialize_both_args_t {};

	constexpr compress_value_initialize_both_args_t compress_value_initialize_both_args{};

	template<
		typename T1,
		typename T2
	>
	struct compress_pair<T1, T2, true> final:
		private T1
	{
		T2 second_obj;

		template<typename... Args>
		constexpr explicit compress_pair(compress_value_initialize_first_arg_t,
			Args&&... args)
			:T1{},
			second_obj{ Yupei::forward<Args>(args)... }
		{

		}

		template<
			typename T,
			typename... UArgs
		>
			compress_pair(
				compress_value_initialize_both_args_t hint, 
				T&& t1,
				UArgs&&... uargs)
			:T1(Yupei::forward<T>(t1)),
			second_obj(Yupei::forward<UArgs>(uargs)...)
		{

		}

		constexpr compress_pair()
			:T1(),
			second_obj()
		{

		}

		T1& first() noexcept
		{
			return static_cast<T1>(*this);
		}

		const T1& first() const noexcept
		{
			return static_cast<T1>(*this);
		}

		T2& second() noexcept
		{
			return second_obj;
		}

		const T2& second() const noexcept
		{
			return second_obj;
		}

		void swap(compress_pair& rhs)
		{
			swap(first(), rhs.first());
			swap(second(), rhs.second());
		}
	};

	template<
		typename T1,
		typename T2
	>
	struct compress_pair<T1, T2, false> 
	{
		T2 second_obj;
		T1 first_obj;
		template<typename... Args>
		constexpr compress_pair(compress_value_initialize_first_arg_t,
			Args&&... args)
			:first_obj(),
			second_obj(Yupei::forward<Args>(args)...)
		{

		}

		template<
			typename T,
			typename... UArgs
		>
			compress_pair(
				compress_value_initialize_both_args_t hint,
				T&& t1,
				UArgs&&... uargs)
			:first_obj(Yupei::forward<T>(t1)),
			second_obj(Yupei::forward<UArgs>(uargs)...)
		{

		}

		constexpr compress_pair()
			:first_obj(),
			second_obj()
		{

		}

		T1& first() noexcept
		{
			return first_obj;
		}

		const T1& first() const noexcept
		{
			return first_obj;
		}

		T2& second() noexcept
		{
			return second_obj;
		}

		const T2& second() const noexcept
		{
			return second_obj;
		}

		void swap(compress_pair& rhs)
		{
			swap(first(), rhs.first());
			swap(second(), rhs.second());
		}
	};
}

#pragma warning (pop)