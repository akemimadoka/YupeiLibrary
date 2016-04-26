#pragma once

#include "Coordinate.hpp"
#include "TypeTraits.hpp"
#include <utility>

namespace Yupei
{
	namespace Internal
	{
		template<typename T>
		constexpr T& GetFirstElement(T& a) noexcept
		{
			return a;
		}

		template<typename T, std::size_t N>
		constexpr decltype(auto) GetFirstElement(T(&a)[N]) noexcept
		{
			return GetFirstElement(a[0]);
		}
	}

	template<typename T, std::ptrdiff_t Rank = 1>
	class array_view
	{
	public:
		static constexpr std::ptrdiff_t rank = Rank;
		using index_type = index<Rank>;
		using bounds_type = bounds<Rank>;
		CONTAINER_DEFINE(T)
		using iterator = pointer;
		using const_iterator = const_pointer;

		CXX14_CONSTEXPR array_view() noexcept
			:ptr_ {}
		{}

		template<typename U, std::ptrdiff_t R, std::ptrdiff_t R2 = Rank, std::enable_if_t<R2 == R && std::conjunction<std::is_convertible<std::add_pointer_t<U>, pointer>,
			std::is_same<std::remove_cv_t<U>, std::remove_cv_t<value_type>>>::value, int> = 0>
		CXX14_CONSTEXPR array_view(const array_view<U, R>& rhs) noexcept
			:ptr_{rhs.ptr_},
			bounds_{rhs.bounds_.size()}
		{}

		template<typename ArrayT, 
			typename = std::enable_if_t<std::conjunction<std::is_convertible<std::add_pointer_t<std::remove_all_extents_t<ArrayT>>, pointer>, 
			std::is_same<std::remove_cv_t<std::remove_all_extents_t<ArrayT>>, 
			std::remove_cv_t<value_type>>>::value && std::rank<ArrayT>::value == rank>/*, int> = 0*/>
		CXX14_CONSTEXPR array_view(ArrayT& arr) noexcept
			: array_view(arr, std::make_index_sequence<rank>{})
		{}

		CXX14_CONSTEXPR array_view(pointer ptr, bounds_type bounds) noexcept
			:ptr_{ptr},
			bounds_{bounds}
		{}

		constexpr bounds_type bounds() const noexcept
		{
			return bounds_;
		}

		constexpr size_type size() const noexcept
		{
			return bounds().size();
		}

		CXX14_CONSTEXPR index_type stride() const noexcept
		{
			index_type stride;
			std::ptrdiff_t cur = 1;
			for (auto i = rank - 1; i >= 0; --i)
			{
				stride[i] = cur;
				cur *= bounds_[i];
			}
			return stride;
		}

		constexpr pointer data() const noexcept
		{
			return ptr_;
		}

		iterator begin() noexcept
		{
			return data();
		}

		const_iterator begin() const noexcept
		{
			return data();
		}

		iterator end() noexcept
		{
			return data() + bounds_.size();
		}

		const_iterator end() const noexcept
		{
			return data() + bounds_.size();
		}

		CXX14_CONSTEXPR reference operator[](const index_type& idx) const
		{
			assert(bounds().contains(idx));
			std::ptrdiff_t cur = 1;
			std::ptrdiff_t index = 0;
			for (auto i = rank - 1; i >= 0; --i)
			{
				index += idx[i] * cur;
				cur *= bounds_[i];
			}
			return data()[index];
		}

		template<std::ptrdiff_t R = rank, std::enable_if_t<(R > 1), int> = 0>
		CXX14_CONSTEXPR array_view<T, rank - 1> operator[](std::ptrdiff_t slice) const noexcept
		{
			using BoundsType = typename array_view<T, rank - 1>::bounds_type;
			BoundsType newBounds;
			std::ptrdiff_t offset = 0;
			std::ptrdiff_t cur = 1;
			const auto tmpBounds = bounds();
			for (std::ptrdiff_t i = rank - 1; i >= 1; --i)
			{
				const auto v = tmpBounds[i];
				cur *= v;
				newBounds[i - 1] = v;
			}
			return { data() + cur * slice, newBounds };			
		}

	private:
		template<typename ArrayT, std::size_t... I>
		CXX14_CONSTEXPR array_view(ArrayT& arr, std::index_sequence<I...>) noexcept
			:ptr_{std::addressof(Internal::GetFirstElement(arr))},
			bounds_{static_cast<std::ptrdiff_t>(std::extent_v<ArrayT, I>)...}
		{}

		pointer ptr_;
		bounds_type bounds_;
	};

	using std::begin;
	using std::end;
	using std::cbegin;
	using std::cend;
	using std::rbegin;
	using std::rend;
	using std::size;
	using std::data;
}