#pragma once

#include <type_traits>
#include <utility>
#include <iterator>

namespace Yupei
{
    namespace Internal
    {      
        template<typename T>
        class RangeIterator
        {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using reference = const value_type&;
            using pointer = const value_type*;
            using iterator_category = std::random_access_iterator_tag;

        private:
            template<typename>
            friend class Range;

            constexpr RangeIterator(T cur) noexcept
                :cur_{std::move(cur)}
            {}

        public:
            RangeIterator& operator++() 
            {
                ++cur_;
                return *this;
            }

            RangeIterator operator++(int)
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            RangeIterator& operator--()
            {
                --cur_;
                return *this;
            }

            RangeIterator operator--(int)
            {
                auto tmp = *this;
                --*this;
                return tmp;
            }

            reference operator*() const noexcept
            {
                return cur_;
            }

            pointer operator->() noexcept
            {
                return std::addressof(cur_);
            }

            bool operator==(const RangeIterator& rhs) const noexcept(noexcept(std::declval<T>() == std::declval<T>()))
            {
                return cur_ == rhs.cur_;
            }

            bool operator!=(const RangeIterator& rhs) const noexcept(noexcept(std::declval<T>() == std::declval<T>()))
            {
                return !(*this == rhs);
            }

            
        private:
            T cur_;
        };


        template<typename T>
        class Range
        {
            static_assert(std::is_move_constructible<T>::value, "Not nothrow constructible.");

        public:
            using value_type = T;
            using size_type = std::size_t;
            using iterator = RangeIterator<T>;
            using const_iterator = iterator;

            constexpr Range(T from, T to) noexcept
                :from_{std::move(from)}, to_{std::move(to)}
            {}

            const_iterator begin() const noexcept
            {
                return {from_};
            }

            const_iterator end() const noexcept
            {
                return {to_};
            }
            
        private:          
            T from_;
            T to_;
        };

        template<typename T>
        decltype(auto) begin(const Range<T>& rng) noexcept
        {
            return rng.begin();
        }

        template<typename T>
        decltype(auto) end(const Range<T>& rng) noexcept
        {
            return rng.end();
        }

        template<typename T>
        decltype(auto) cbegin(const Range<T>& rng) noexcept
        {
            return rng.begin();
        }

        template<typename T>
        decltype(auto) cend(const Range<T>& rng) noexcept
        {
            return rng.end();
        }
    }

    template<typename T>
    Internal::Range<T> range(T from, T to) noexcept
    {
        return {std::move(from), std::move(to)};
    }

    template<typename T>
    Internal::Range<T> range(T to) noexcept
    {
        return range<T>({}, std::move(to));
    }
}
