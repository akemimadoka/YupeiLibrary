#pragma once

#include "Extensions.hpp"
#include <exception>		//for std::uncaught_exceptions
#include <type_traits>		//for std::decay_t
#include <utility>			//for std::move, std::forward

namespace Yupei
{
	namespace Internal
	{
		template <typename FuncT>
		class AtScopeExit
		{
			FuncT func_;

		public:
			AtScopeExit(const FuncT& action) noexcept
				: func_(action)
			{}

			AtScopeExit(FuncT&& action) noexcept
				: func_(std::move(action))
			{}

			DISABLECOPY(AtScopeExit)
			DEFAULTMOVE(AtScopeExit)

			~AtScopeExit()
			{
				func_();
			}
		};

		enum class CreateScopeExit {};

		template<typename FuncT>
		AtScopeExit<std::decay_t<FuncT>> operator+ (CreateScopeExit, FuncT&& func) noexcept
		{
			return { std::forward<FuncT>(func) };
		}

		class UncaughtExceptionCounter
		{
			int exceptionCount_;

		public:
			UncaughtExceptionCounter() noexcept
				: exceptionCount_ { std::uncaught_exceptions() }
			{}

			bool newUncaughtException() noexcept
			{
				return std::uncaught_exceptions() > exceptionCount_;
			}
		};

		template<typename FuncT, bool executeOnException>
		class ScopeGuardForNewException
		{
			FuncT function_;
			UncaughtExceptionCounter ec_;

		public:
			explicit ScopeGuardForNewException(const FuncT& fn) noexcept
				: function_ { fn }
			{}

			explicit ScopeGuardForNewException(FuncT&& fn) noexcept
				: function_ { std::move(fn) }
			{}

			DISABLECOPY(ScopeGuardForNewException)
			DEFAULTMOVE(ScopeGuardForNewException)

			~ScopeGuardForNewException()
			{
				if(executeOnException == ec_.newUncaughtException())
					function_();
			}
		};

		enum class CreateScopeFail {};

		enum class CreateScopeSuccess {};

		template<typename FuncT>
		ScopeGuardForNewException<std::decay_t<FuncT>, true> operator+ (CreateScopeFail, FuncT&& func) noexcept
		{
			return ScopeGuardForNewException<std::decay_t<FuncT>, true>{ std::forward<FuncT>(func) };
		}

		template<typename FuncT>
		ScopeGuardForNewException<std::decay_t<FuncT>, false> operator+ (CreateScopeSuccess, FuncT&& func) noexcept
		{
			return ScopeGuardForNewException<std::decay_t<FuncT>, false>{ std::forward<FuncT>(func) };
		}
	}
}

#define SCOPE_EXIT \
	auto ANONYMOUS_VARIABLE(SCOPE_EXIT_STATE) = ::Yupei::Internal::CreateScopeExit{} + [&]()

#define SCOPE_FAIL \
	auto ANONYMOUS_VARIABLE(SCOPE_FAIL_STATE) = ::Yupei::Internal::CreateScopeFail{} + [&]()

#define SCOPE_SUCCESS \
	auto ANONYMOUS_VARIABLE(SCOPE_SUCCESS_STATE) = ::Yupei::Internal::CreateScopeSuccess{} + [&]()

