#pragma once

#include <stdexcept> //for std::logic_error
#include <string> //for std::string
#include <cassert>
#include "Utility.hpp"
#include "TypeTraits.hpp"
#include "Tuple.hpp"
#include "ConstructDestruct.hpp"

namespace Yupei 
{
	struct bad_variant_access
		:std::logic_error
	{
		explicit bad_variant_access(const std::string& str)
			:std::logic_error{ str }{}

		explicit bad_variant_access(const char* str)
			:std::logic_error{ str }{}
	};

	namespace Internal
	{
		struct VariantVoid {};

		template<typename T>
		using GetVariantType = std::conditional_t<std::is_void<T>::value, VariantVoid, T>;

		template<typename... Args>
		union VariantData;

		template<>
		union VariantData<>
		{
			template<typename RetT, typename FunctionT>
			RetT Apply(std::size_t n, FunctionT&& func) const
			{
				assert(0);
			}
		};

		template<typename T,typename... Args>
		union VariantData<T, Args...>
		{		
			template<typename... U>
			friend union VariantData;

		private:
			T head_;
			VariantData<Args...> tail_;

			using size_type = std::size_t;

		public:
			VariantData() {}

			~VariantData() {}

			template<typename... ParamsT>
			constexpr VariantData(size_constant<0>, ParamsT&&... params)
				:head_{std::forward<ParamsT>(params)...}
			{}

			template<typename std::size_t N,typename... ParamsT, typename = std::enable_if_t<N != 0>>
			constexpr VariantData(size_constant<N>, ParamsT&&... params)
				:tail_{size_constant<N - 1>{},std::forward<ParamsT>(params)...}
			{}

			template<typename UnionT>
			void Construct(size_type n, UnionT&& u)
			{
				n == 0 ? Yupei::construct(Target(), std::forward<UnionT>(u).head_)
					: tail_.Construct(n - 1, std::forward<UnionT>(u).head_);
			}

			template<typename U,typename... Us>
			bool IsEqual(size_type n, const VariantData<U, Us...>& rhs)
			{
				return n == 0 ? head_ == rhs.head_ : IsEqual(n - 1, rhs.tail_);
			}

			template<typename RetT, typename FunctionT>
			RetT Apply(size_type n, FunctionT&& func)
			{
				return (n == 0 ? std::forward<FunctionT>(func)(head_) :
					tail_.Apply<RetT>(n - 1, std::forward<FunctionT>(func)));
			}

			template<typename RetT, typename FunctionT>
			RetT Apply(size_type n, FunctionT&& func) const
			{
				return (n == 0 ? std::forward<FunctionT>(func)(head_) :
					tail_.Apply<RetT>(n - 1, std::forward<FunctionT>(func)));
			}

		private:
			void* Target() noexcept
			{
				return static_cast<void*>(&head_);
			}
		};

		template<typename... Args>
		using VariantDataType = VariantData<GetVariantType<Args>...>;

		struct empty_variant_tag {};

		struct DestroyFun
		{
			template<typename T>
			void operator()(T& t) const
			{
				t.~T();
			}
		};

		template<std::size_t N,typename... ParamsT>
		struct ConstructFun
		{
		private:
			tuple<ParamsT...> params_;

			template<typename U,std::size_t... Index>
			void Construct(U& u, std::index_sequence<Index...> seq)
			{
                Yupei::construct(std::addressof(u), std::forward<ParamsT>(Yupei::get<Index>(params_))...);
			}

		public:
			//TODO: 在不是自己的索引时选择某个神秘版本（这个版本不会被调用），在是自己索引的时候选择这个 ::new。
			template<typename U, typename = std::enable_if_t<std::is_constructible<U, ParamsT&&...>::value>>
			void operator()(U& u)
			{
				Construct(u, std::make_index_sequence<sizeof...(ParamsT)>{});
			}

			void operator()(...)
			{
				assert(0);
			}

			ConstructFun(ParamsT&&... params)
				:params_{std::forward<ParamsT>(params)...}
			{}
		};

		struct EmptyVariantTag {};
		
		template<typename T,std::size_t I = 0>
		struct Overloads {};

		template<typename T,typename... Args,std::size_t I>
		struct Overloads<tuple<T,Args...>, I> : Overloads<tuple<Args...>, I + 1>
		{
			//using BaseType = Overloads<tuple<Args...>, I + 1>;
			using FuncType = size_constant<I>(*)(T);
			//using BaseType::operator();
			operator FuncType();
			//size_constant<I> operator()(T);
		};

		struct OverloadFailedTag {};

		OverloadFailedTag OverloadInvoke(...);

		template<typename FunctorT, typename T>
		auto OverloadInvoke(FunctorT&&, T&&)
			->decltype(std::declval<FunctorT>()(std::declval<T>()));

		template<typename FunctorT,typename T,typename RetT = 
			decltype(Internal::OverloadInvoke(std::declval<FunctorT>(), std::declval<T>()))>
		struct OverloadResultType : RetT {};

		template<typename FunctorT,typename T>
		struct OverloadResultType<FunctorT, T, OverloadFailedTag> {};

		template<typename U,typename... CandidatesT>
		struct BestMatchIndex : OverloadResultType<Overloads<tuple<CandidatesT...>>, U> {};

		template<typename FunctorT>
		struct Ignore2ndParam : FunctorT
		{
			using BaseType = FunctorT;

		private:
			FunctorT& GetFunctor() noexcept { return *this; }
			const FunctorT& GetFunctor() const noexcept { return *this; }

		public:
			Ignore2ndParam(FunctorT functor)
				:BaseType{std::move(functor)}
			{
			}

			template<typename U,std::size_t I>
			decltype(auto) operator()(U&& u, size_constant<I> i)
			{
				return GetFunctor()(std::forward<U>(u));
			}
		};
	}

	template<typename T>
	struct emplaced_type_t {};

	//template<typename T>
	//constexpr emplaced_type_t<T> emplaced_type;

	template<std::size_t I>
	struct emplaced_index_t {};

	//template<std::size_t I>
	//constexpr emplaced_index_t<I> emplaced_index;


	template<typename... FunctorsT>
	struct overload_t;

	template<typename FunctorT>
	struct overload_t<FunctorT> : FunctorT
	{
		//using result_type = RetT;
		using BaseType = FunctorT;
		using BaseType::operator();
		overload_t(FunctorT functor) : FunctorT{std::move(functor)} {}
	};


	template<typename FirstFunctorT, typename... FunctorsT>
	struct overload_t<FirstFunctorT, FunctorsT...> :
		FirstFunctorT, overload_t<FunctorsT...>
	{
		using BaseType = overload_t<FunctorsT...>;
		using FirstFunctorT::operator();
		using BaseType::operator();

		overload_t(FirstFunctorT functor, FunctorsT... others)
			:FirstFunctorT{std::move(functor)},
			BaseType{std::move(others)...}
		{
		}
	};

	template<typename... FunctorsT>
    inline overload_t<FunctorsT...> make_overload(FunctorsT&&... functors)
	{
		return overload_t<FunctorsT...>(std::forward<FunctorsT>(functors)...);
	}

	template<typename... Args>
	struct variant
	{
		using size_type = std::size_t;

		static constexpr size_type InvalidIndex = static_cast<size_type>(-1);

		variant()
			:variant{emplaced_index_t<0>{}}
		{
		}

		template<size_type I,typename... Args>
		constexpr explicit variant(emplaced_index_t<I>, Args&&... args)
			:index_{I},
			data_{size_constant<I>{}, std::forward<Args>(args)...}
		{}

		variant(variant&& rhs) noexcept
			: variant{Internal::EmptyVariantTag{}}
		{
			Assign(std::move(rhs));
		}

		variant(const variant& rhs) 
			: variant{Internal::EmptyVariantTag{}}
		{
			Assign(rhs);
		}

		variant& operator=(variant&& rhs) noexcept
		{
			Clear();
			Assign(std::move(rhs));
			return *this;
		}

		variant& operator=(const variant& rhs)
		{
			Clear();
			Assign(rhs);
			return *this;
		}

		template<typename U, size_type I = Internal::BestMatchIndex<U&&,Args...>::value,
			typename = std::enable_if_t<!std::is_same<variant, std::decay_t<U>>::value>>
		variant& operator=(U&& element)
		{
			//TODO
			emplace(emplaced_index_t<I>{}, std::forward<U>(element));
			return *this;
		}

		~variant()
		{
			Clear();
		}

		static constexpr size_type size() noexcept
		{
			return sizeof...(Args);
		}

		bool valid() const noexcept
		{
			return index_ != InvalidIndex;
		}

		size_type index() const noexcept
		{
			return index_;
		}

		template<size_type I,typename... Args>
		void emplace(emplaced_index_t<I>,Args&&... args)
		{
			Clear();
			Internal::ConstructFun<I, Args&&...> fun{ std::forward<Args>(args)... };
			data_.Apply<void>(I, fun);
			index_ = I;
		}

		template<typename RetT,typename FunctorT>
		RetT apply(FunctorT&& functor)
		{
			return data_.Apply<RetT>(index_, std::forward<FunctorT>(functor));
		}

		template<typename RetT, typename FunctorT>
		RetT apply(FunctorT&& functor) const
		{
			return data_.Apply<RetT>(index_, std::forward<FunctorT>(functor));
		}

	private:
		size_type index_;
		Internal::VariantData<Args...> data_;

		constexpr variant(Internal::EmptyVariantTag)
			:index_{ InvalidIndex }
		{
		}

		void Clear()
		{
			if (valid())
			{
				data_.Apply<void>(index_, Internal::DestroyFun{});
				index_ = InvalidIndex;
			}
		}

		template<typename UnionT>
		void Assign(UnionT&& rhs)
		{
			if (rhs.valid())
			{
				data_.Construct(rhs.index_, std::forward<UnionT>(rhs));
				index_ = rhs.index_;
			}
		}
	};

	template<typename RetT,typename... ElementsT,typename... Functors>
	RetT match(const variant<ElementsT...>& v, Functors&&... functors)
	{
		return v.apply<RetT>(Yupei::make_overload(std::forward<Functors>(functors)...));
	}

	template<typename RetT, typename... ElementsT, typename... Functors>
	RetT match(variant<ElementsT...>& v, Functors&&... functors)
	{
		return v.apply<RetT>(Yupei::make_overload(std::forward<Functors>(functors)...));
	}
}
