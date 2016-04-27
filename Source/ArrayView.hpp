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

		template<typename T, std::ptrdiff_t Rank>
		CXX14_CONSTEXPR T& ViewAccess(T* ptr, index<Rank> idx, index<Rank> stride) noexcept
		{
			std::ptrdiff_t offset = {};
			for (std::ptrdiff_t i = 0; i < Rank; ++i)
				offset += idx[i] * stride[i];
			return ptr[offset];
		}
    }

    template<typename, std::ptrdiff_t = 1>
    class strided_array_view;

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

    private:
        template<typename U>
        using ViewConstructible = std::conjunction<std::is_convertible<std::add_pointer_t<U>, pointer>,
            std::is_same<std::remove_cv_t<U>, std::remove_cv_t<value_type>>>;

    public:
        CXX14_CONSTEXPR array_view() noexcept
            :ptr_ {}
        {}

        template<typename U, std::ptrdiff_t R, std::ptrdiff_t R2 = Rank, std::enable_if_t<R2 == 1 && std::conjunction<std::is_convertible<std::add_pointer_t<U>, pointer>,
            std::is_same<std::remove_cv_t<U>, std::remove_cv_t<value_type>>>::value, int> = 0>
        CXX14_CONSTEXPR array_view(const array_view<U, R>& rhs) noexcept
            : ptr_ { rhs.ptr_ },
            bounds_ { rhs.bounds_.size() }
        {}

        template<typename ArrayT,
            typename = std::enable_if_t<ViewConstructible<std::remove_all_extents_t<ArrayT>>::value && std::rank<ArrayT>::value == rank>/*, int> = 0*/>
        CXX14_CONSTEXPR array_view(ArrayT& arr) noexcept
            : array_view(arr, std::make_index_sequence<rank>{})
        {}

        CXX14_CONSTEXPR array_view(pointer ptr, bounds_type bounds) noexcept
            : ptr_ { ptr },
            bounds_ { bounds }
        {}

        template<typename U, typename = std::enable_if_t<ViewConstructible<U>::value>>
        CXX14_CONSTEXPR array_view(const array_view<U, rank>& rhs) noexcept
            :ptr_{rhs.ptr_},
            bounds_{rhs.bounds_}
        {}

        template<typename U, typename = std::enable_if_t<ViewConstructible<U>::value>>
        CXX14_CONSTEXPR array_view& operator=(const array_view<U, rank>& rhs) noexcept
        {
            ptr_ = rhs.ptr_;
            bounds_ = rhs.bounds_;
            return *this;
        }

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
			return Internal::ViewAccess(data(), idx, stride());
        }

        //[2][2][3] operator[1] -> [2][3]
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

        CXX14_CONSTEXPR strided_array_view<T, Rank>
            section(const index_type& origin, const bounds_type& sectionBnd) const noexcept;

        CXX14_CONSTEXPR strided_array_view<T, Rank>
            section(const index_type& origin) const noexcept;
       
    private:
        template<typename ArrayT, std::size_t... I>
        CXX14_CONSTEXPR array_view(ArrayT& arr, std::index_sequence<I...>) noexcept
            : ptr_ { std::addressof(Internal::GetFirstElement(arr)) },
            bounds_ { static_cast<std::ptrdiff_t>(std::extent_v<ArrayT, I>)... }
        {}

        pointer ptr_;
        bounds_type bounds_;
    };

    template<typename T, std::ptrdiff_t Rank>
    class strided_array_view
    {
    public:
        static constexpr std::ptrdiff_t rank = Rank;
        using index_type = index<Rank>;
        using bounds_type = bounds<Rank>;
        CONTAINER_DEFINE(T)
        using iterator = pointer;
        using const_iterator = const_pointer;

    private:
        template<typename U>
        using ViewConstructible = std::conjunction<std::is_convertible<std::add_pointer_t<U>, pointer>,
            std::is_same<std::remove_cv_t<U>, std::remove_cv_t<value_type>>>;

    public:
        CXX14_CONSTEXPR strided_array_view() noexcept
            : ptr_ {}
        {}

        template<typename U, typename = std::enable_if_t<ViewConstructible<U>::value>>
        CXX14_CONSTEXPR strided_array_view(const array_view<U, rank>& rhs) noexcept
            :ptr_{rhs.data()},
            stride_{rhs.stride()},
            bounds_{rhs.bounds()}
        {}

        template<typename U, typename = std::enable_if_t<ViewConstructible<U>::value>>
        CXX14_CONSTEXPR strided_array_view(const strided_array_view<U, rank>& rhs) noexcept
            :ptr_ { rhs.data() },
            stride_ { rhs.stride() },
            bounds_ { rhs.bounds() }
        {}

        CXX14_CONSTEXPR strided_array_view(pointer ptr, index_type start, bounds_type bound, index_type stride) noexcept
            :ptr_{ptr},
			size_ { (bound - start).size() },
            stride_{stride},
            bounds_{bound}
        {}

        template<typename U>
        CXX14_CONSTEXPR strided_array_view& operator=(const strided_array_view<U, rank>& rhs) noexcept
        {
            ptr_ = rhs.data();
            bounds_ = rhs.bounds();
            stride_ = rhs.stride();
            return *this;
        }

        constexpr bounds_type bounds() const noexcept
        {
            return bounds_;
        }

        constexpr size_type size() const noexcept
        {
			return size_;
        }

        constexpr pointer data() const noexcept
        {
            return ptr_;
        }

        constexpr index_type stride() const noexcept
        {
            return stride_;
        }

        iterator begin() noexcept
        {
            return data();
        }

        iterator end() noexcept
        {
            return data() + size();
        }

        const_iterator begin() const noexcept
        {
            return data();
        }

        const_iterator end() const noexcept
        {
            return data() + size();
        }

        CXX14_CONSTEXPR reference operator[](const index_type& idx) const noexcept
        {
			return Internal::ViewAccess(data(), idx, stride());
        }

        CXX14_CONSTEXPR strided_array_view<T, rank - 1> operator[](std::ptrdiff_t slice) const noexcept
        {
            using BoundsType = typename strided_array_view<T, rank - 1>::bounds_type;
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

        CXX14_CONSTEXPR strided_array_view<T, Rank>
            section(const index_type& origin, const bounds_type& sectionBnd) const
        {
            //assert(sectionBnd.contains(origin) && sectionBnd < bounds());
            return { std::addressof((*this)[origin]), origin, sectionBnd, stride() };
        }

        CXX14_CONSTEXPR strided_array_view<T, Rank>
            section(const index_type& origin) const noexcept
        {
            //assert(bounds().contains(origin));
            return { std::addressof((*this)[origin]), origin, bounds() - origin, stride() };
        }

    private:
        pointer ptr_;
		size_type size_;
        index_type stride_;
        bounds_type bounds_;     
    };

    template<typename T, std::ptrdiff_t Rank>
    CXX14_CONSTEXPR strided_array_view<T, Rank>
        array_view<T, Rank>::section(const index_type& origin, const bounds_type& sectionBnd) const noexcept
    {
        //assert(sectionBnd.contains(origin) && sectionBnd < bounds());
        return { std::addressof((*this)[origin]), origin, sectionBnd, stride() };
    }

    template<typename T, std::ptrdiff_t Rank>
    CXX14_CONSTEXPR strided_array_view<T, Rank>
        array_view<T, Rank>::section(const index_type& origin) const noexcept
    {
        //assert(bounds().contains(origin));
		//FIXME.
        return { std::addressof((*this)[origin]), origin, bounds() - origin, stride() };
    }

    using std::begin;
    using std::end;
    using std::cbegin;
    using std::cend;
    using std::rbegin;
    using std::rend;
    using std::size;
    using std::data;
}