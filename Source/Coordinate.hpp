#pragma once

#include "HelperMacros.hpp"
#include "Config.hpp"
#include "TypeTraits.hpp"
#include "Assert.hpp"
#include <array>
#include <cstddef>
#include <initializer_list>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>

namespace Yupei
{
    template<std::ptrdiff_t Rank>
    class index
    {
    public:
        static constexpr std::ptrdiff_t rank = Rank;
        CONTAINER_DEFINE(std::ptrdiff_t)

        constexpr index() noexcept
            :indexes_ {}
        {}

        template<std::size_t R = rank, std::enable_if_t<R == 1, int> = 0>
        constexpr index(value_type v) noexcept
            : indexes_ { v }
        {}

        constexpr index(std::initializer_list<value_type> il) noexcept
        {
            std::copy(il.begin(), il.end(), indexes_.begin());
        }

        template<std::ptrdiff_t R>
        friend bool operator==(const index<R>& lhs, const index<R>& rhs) noexcept;

        CXX14_CONSTEXPR reference operator[](size_type n) noexcept
        {
            assert(n < rank);
            return indexes_[n];
        }

        CXX14_CONSTEXPR const_reference operator[](size_type n) const noexcept
        {
            assert(n < rank);
            return indexes_[n];
        }

        CXX14_CONSTEXPR index& operator+=(const index& rhs) noexcept
        {
            for (std::size_t i = 0; i < rank; ++i)
                (*this)[i] += rhs[i];
            return *this;
        }

        CXX14_CONSTEXPR index& operator-=(const index& rhs) noexcept
        {
            for (std::size_t i = 0; i < rank; ++i)
                (*this)[i] -= rhs[i];
            return *this;
        }

        constexpr index operator+(const index& rhs) const noexcept
        {
            return index<rank>{*this} += rhs;
        }

        constexpr index operator-(const index& rhs) const noexcept
        {
            return index<rank>{*this} -= rhs;
        }

        template<std::size_t R = rank, std::enable_if_t<R == 1, int> = 0>
        CXX14_CONSTEXPR index& operator++() noexcept
        {
            ++(*this)[0];
            return *this;
        }

        template<std::size_t R = rank, std::enable_if_t<R == 1, int> = 0>
        CXX14_CONSTEXPR index& operator--() noexcept
        {
            --(*this)[0];
            return *this;
        }

        template<std::size_t R = rank, std::enable_if_t<R == 1, int> = 0>
        CXX14_CONSTEXPR index operator++(int) noexcept
        {
            return { ++(*this)[0] };
        }

        template<std::size_t R = rank, std::enable_if_t<R == 1, int> = 0>
        CXX14_CONSTEXPR index operator--(int) noexcept
        {
            return { --(*this)[0] };
        }

        CXX14_CONSTEXPR index& operator*=(value_type v) noexcept
        {
            std::for_each(indexes_.begin(), indexes_.end(), [v](auto& x)
            {
                x *= v;
            });
            return *this;
        }

        CXX14_CONSTEXPR index& operator/=(value_type v) noexcept
        {
            std::for_each(indexes_.begin(), indexes_.end(), [v](auto& x)
            {
                x /= v;
            });
            return *this;
        }

        CXX14_CONSTEXPR index operator*(value_type v) const noexcept
        {
            return index<rank>{*this} *= v;
        }

        CXX14_CONSTEXPR index operator/(value_type v) const noexcept
        {
            return index<rank>{*this} /= v;
        }

        template<std::size_t R>
        friend CXX14_CONSTEXPR index<R> operator*(std::ptrdiff_t v, const index<R>& rhs) noexcept;

    private:
        std::array<value_type, rank> indexes_;
    };

    template<std::ptrdiff_t R>
    bool operator==(const index<R>& lhs, const index<R>& rhs) noexcept
    {
        const auto& lArr = lhs.indexes_;
        const auto& rArr = rhs.indexes_;
        return std::equal(lArr.cbegin(), lArr.cend(), rArr.cbegin(), rArr.cend());
    }

    template<std::ptrdiff_t R>
    bool operator!=(const index<R>& lhs, const index<R>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template<std::ptrdiff_t R>
    CXX14_CONSTEXPR index<R> operator*(std::ptrdiff_t v, const index<R>& rhs) noexcept
    {
        return index<R>{rhs} *= v;
    }

    template<std::ptrdiff_t Rank>
    class bounds_iterator;

    template<std::ptrdiff_t Rank>
    class bounds
    {
    public:
        static constexpr std::size_t rank = Rank;
        CONTAINER_DEFINE(std::ptrdiff_t)
        using iterator = bounds_iterator<rank>;
        using const_iterator = iterator;

        constexpr bounds() noexcept
            :bounds_ {}
        {}

        template<std::size_t R = rank, std::enable_if_t<R == 1, int> = 0>
        constexpr bounds(value_type v) noexcept
            : bounds_ { v }
        {}

        constexpr bounds(std::initializer_list<value_type> il) noexcept
        {
            std::copy(il.begin(), il.end(), bounds_.begin());
        }

        constexpr size_type size() const noexcept
        {
            return std::accumulate(bounds_.begin(), bounds_.end(), static_cast<size_type>(1), std::multiplies<>{});
        }

        CXX14_CONSTEXPR bool contains(const index<rank>& idx) const noexcept
        {
            for (std::ptrdiff_t i = 0; i < rank; ++i)
                if (idx[i] >= (*this)[i]) return false;
            return true;
        }

        const_iterator begin() const noexcept;
        const_iterator end() const noexcept;

        template<std::ptrdiff_t R>
        friend bool operator==(const bounds<R>& lhs, const bounds<R>& rhs) noexcept;

        CXX14_CONSTEXPR reference operator[](size_type n) noexcept
        {
            assert(n < rank);
            return bounds_[n];
        }

        CXX14_CONSTEXPR const_reference operator[](size_type n) const noexcept
        {
            assert(n < rank);
            return bounds_[n];
        }

        CXX14_CONSTEXPR bounds& operator+=(const index<Rank>& rhs) noexcept
        {
            for (std::size_t i = 0; i < rank; ++i)
                (*this)[i] += rhs[i];
            return *this;
        }

        CXX14_CONSTEXPR bounds& operator-=(const index<Rank>& rhs) noexcept
        {
            for (std::size_t i = 0; i < rank; ++i)
                (*this)[i] -= rhs[i];
            return *this;
        }

        constexpr bounds operator+(const bounds& rhs) const noexcept
        {
            return bounds<rank>{*this} += rhs;
        }

        constexpr bounds operator-(const bounds& rhs) const noexcept
        {
            return bounds<rank>{*this} -= rhs;
        }

        CXX14_CONSTEXPR bounds& operator*=(value_type v) noexcept
        {
            std::for_each(bounds_.begin(), bounds_.end(), [v](auto& x)
            {
                x *= v;
            });
            return *this;
        }

        CXX14_CONSTEXPR bounds& operator/=(value_type v) noexcept
        {
            std::for_each(bounds_.begin(), bounds_.end(), [v](auto& x)
            {
                x /= v;
            });
            return *this;
        }

        CXX14_CONSTEXPR bounds operator*(value_type v) const noexcept
        {
            return bounds<rank>{*this} *= v;
        }

        CXX14_CONSTEXPR bounds operator/(value_type v) const noexcept
        {
            return bounds<rank>{*this} /= v;
        }
    private:
        std::array<value_type, rank> bounds_;
    };

    template<std::ptrdiff_t R>
    bool operator==(const bounds<R>& lhs, const bounds<R>& rhs) noexcept
    {
        const auto& lArr = lhs.bounds_;
        const auto& rArr = rhs.bounds_;
        return std::equal(lArr.cbegin(), lArr.cend(), rArr.cbegin(), rArr.cend());
    }

    template<std::ptrdiff_t R>
    bool operator!=(const bounds<R>& lhs, const bounds<R>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template<std::ptrdiff_t R>
    CXX14_CONSTEXPR bounds<R> operator*(std::ptrdiff_t v, const bounds<R>& rhs) noexcept
    {
        bounds<R> ret { rhs };
        ret *= v;
        return ret;
    }

    template<std::ptrdiff_t Rank>
    class bounds_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = index<Rank>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type;

        friend bounds<Rank>;

        bounds_iterator& operator++()
        {
            assert(!IsEnd());
            for (auto i = Rank - 1; i >= 0; --i)
            {
                if (++indexes_[i] < bounds_[i])
                    return *this;
                indexes_[i] = 0;
            }
            //在到达尾部后，所有的 indexes_ 都为 0！
            //这里必须要设置为 off the end，否则无法与 end(bounds) 比较。
            //off the end 还要确保可以再次递减。
            SetOffTheEnd();
            return *this;
        }

        bounds_iterator operator++(int)
        {
            bounds_iterator tmp { *this };
            ++*this;
            return tmp;
        }


        bounds_iterator& operator--() noexcept
        {
            assert(!IsStart());
            for (auto i = Rank - 1; i >= 0; --i)
            {
                if (--indexes_[i] >= 0)
                    return *this;
                indexes_[i] = bounds_[i] - 1;
            }
            return *this;
        }

        bounds_iterator operator--(int)
        {
            bounds_iterator tmp { *this };
            ++*this;
            return tmp;
        }

        //TODO.
        bounds_iterator  operator+(difference_type n) const;
        bounds_iterator& operator+=(difference_type n);
        bounds_iterator  operator-(difference_type n) const;
        bounds_iterator& operator-=(difference_type n);

        difference_type operator-(const bounds_iterator& rhs) const noexcept
        {
            std::ptrdiff_t cur = 1;
            difference_type result = 0;
            const auto& ri = rhs.indexes_;
            for (auto i = Rank - 1; i >= 0; --i)
            {
                result += (indexes_[i] - ri[i]) * cur;
                cur *= bounds_[i];
            }
            return result;
        }

        reference operator*() const
        {
            return indexes_;
        }

        pointer operator->() const
        {
            return &indexes_;
        }

        reference operator[](difference_type n) const
        {
            bounds_iterator it { *this };
            return it += n;
        }

        template<std::ptrdiff_t Rank>
        friend bool operator==(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept;
        template<std::ptrdiff_t Rank>
        friend bool operator<(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept;

    private:
        bounds_iterator(const bounds<Rank>& bound, const index<Rank>& i = {}) noexcept
            : bounds_ { bound },
            indexes_ { i }
        {}

        void SetOffTheEnd() noexcept
        {
            for (std::ptrdiff_t i = 0; i < Rank - 1; ++i)
                indexes_[i] = bounds_[i] - 1;

            indexes_[Rank - 1] = bounds_[Rank - 1];
        }

        bool IsEnd() const noexcept
        {
            for (std::ptrdiff_t i = 0; i < Rank - 1; ++i)
                if (indexes_[i] != bounds_[i] - 1) return false;
            return indexes_[Rank - 1] == bounds_[Rank - 1];
        }

        bool IsStart() const noexcept
        {
            for (std::ptrdiff_t i = 0; i < Rank; ++i)
                if (indexes_[i] != 0) return false;
            return true;
        }

        bounds<Rank> bounds_;
        index<Rank> indexes_;
    };

    template<std::ptrdiff_t Rank>
    auto bounds<Rank>::begin() const noexcept -> const_iterator
    {
        return { *this };
    }

    template<std::ptrdiff_t Rank>
    auto bounds<Rank>::end() const noexcept -> const_iterator
    {
        //TODO: 这里可以优化。
        const_iterator it { *this };
        it.SetOffTheEnd();
        return it;
    }

    template<std::ptrdiff_t Rank>
    bool operator==(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept
    {
        /*DoWhenDebug(const auto& lb = lhs.bounds_; const auto& rb = rhs.bounds_; assert(std::equal(
            lb.begin(), lb.end(), rb.begin(), rb.end())));*/
        return lhs.indexes_ == rhs.indexes_;
    }

    template<std::ptrdiff_t Rank>
    bool operator!= (const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    template<std::ptrdiff_t Rank>
    bool operator<(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept
    {
        /*DoWhenDebug(const auto& lb = lhs.bounds_; const auto& rb = rhs.bounds_; assert(std::equal(
            lb.begin(), lb.end(), rb.begin(), rb.end())));*/
        const auto& li = lhs.indexes_;
        const auto& ri = rhs.indexes_;
        for (std::size_t i = 0; i < Rank; ++i)
            if (li[i] < ri[i]) return true;
        return false;
    }

    template<std::ptrdiff_t Rank>
    bool operator>(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept
    {
        return rhs < lhs;
    }

    template<std::ptrdiff_t Rank>
    bool operator<=(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept
    {
        return !(rhs < lhs);
    }

    template<std::ptrdiff_t Rank>
    bool operator>=(const bounds_iterator<Rank>& lhs, const bounds_iterator<Rank>& rhs) noexcept
    {
        return !(lhs < rhs);
    }
}