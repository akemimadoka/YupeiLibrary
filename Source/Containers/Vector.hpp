﻿#pragma once

#include "../Assert.hpp"
#include "../Scope.hpp"
#include "../Iterator.hpp"
#include "../Meta.hpp"
#include "../HelperMacros.hpp"
#include "../ConstructDestruct.hpp"
#include "../MemoryResource/MemoryResource.hpp"
#include <algorithm>
#include <stdexcept> //for std::out_of_range
#include <memory>

namespace Yupei
{
    template<typename ElementT>
    class vector;

    namespace Internal
    {
        template<typename T>
        class vector_iterator
        {
        public:
            using MyType = vector_iterator<T>;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::random_access_iterator_tag;

            template<typename U>
            friend class Yupei::vector;

            template<typename U>
            friend constexpr auto do_pointer_from(vector_iterator<U> it) noexcept -> U*
            {
                return it.current_;
            }

            template<typename U>
            friend class vector_const_iterator;

            using ContainerType = Yupei::vector<T>;

            T* current_;
            ContainerType* container_;

            vector_iterator(T* p, ContainerType* container)
                :current_ { p },
                container_ { container }
            {}

        public:
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;

            constexpr vector_iterator() noexcept
                :current_ {},
                container_ {}
            {}

            MyType& operator += (difference_type n) noexcept;
            MyType& operator -= (difference_type n) noexcept;

            MyType& operator --() noexcept
            {
                *this -= 1;
                return *this;
            }

            MyType& operator --(int) noexcept
            {
                auto tmp = *this;
                --*this;
                return tmp;
            }

            template<typename U>
            friend vector_iterator<U> operator + (vector_iterator<U> it, difference_type n) noexcept
            {
                it += n;
                return it;
            }

            template<typename U>
            friend vector_iterator<U> operator + (difference_type n, vector_iterator<U> it) noexcept
            {
                it += n;
                return it;
            }

            MyType& operator ++() noexcept
            {
                *this += 1;
                return *this;
            }

            MyType& operator ++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            template<typename U>
            friend vector_iterator<U> operator - (vector_iterator<U> it, difference_type n) noexcept
            {
                it -= n;
                return it;
            }

            template<typename U>
            friend difference_type operator - (const vector_iterator<U>& lhs, const vector_iterator<U>& rhs) noexcept
            {
                return lhs.current_ - rhs.current_;
            }

            value_type& operator[] (difference_type n) const noexcept
            {
                return current_[n];
            }

            value_type& operator* () const noexcept
            {
                return *current_;
            }

            bool operator < (const MyType& it) const noexcept
            {
                return current_ < it.current_;
            }

            bool operator > (const MyType& it) const noexcept
            {
                return current_ > it.current_;
            }

            bool operator >= (const MyType& it) const noexcept
            {
                return current_ >= it.current_;
            }

            bool operator <= (const MyType& it) const noexcept
            {
                return current_ <= it.current_;
            }

            bool operator == (const MyType& it) const noexcept
            {
                return current_ == it.current_;
            }

            bool operator != (const MyType& it) const noexcept
            {
                return current_ != it.current_;
            }

        };

        template<typename T>
        class vector_const_iterator
        {
        public:
            using MyType = vector_const_iterator<T>;

            template<typename U>
            friend class Yupei::vector;

            using ContainerType = Yupei::vector<T>;

            template<typename U>
            friend constexpr auto do_pointer_from(vector_const_iterator<U> it) noexcept -> U*
            {
                return it.current_;
            }

            T* current_;
            const ContainerType* container_;

            vector_const_iterator(T* p, const ContainerType* container)
                :current_ { p },
                container_ { container }
            {}

        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::random_access_iterator_tag;
            using pointer = const value_type*;
            using reference = const value_type&;

            constexpr vector_const_iterator() noexcept
                :current_ {},
                container_ {}
            {}

            vector_const_iterator(const vector_iterator<T>& it)
                :current_ { it.current_ },
                container_ { it.container_ }
            {}


            MyType& operator += (difference_type n) noexcept;
            MyType& operator -= (difference_type n) noexcept;

            MyType& operator ++() noexcept
            {
                *this += 1;
                return *this;
            }

            MyType& operator ++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            MyType& operator --() noexcept
            {
                *this -= 1;
                return *this;
            }

            MyType& operator --(int) noexcept
            {
                auto tmp = *this;
                --*this;
                return tmp;
            }

            template<typename U>
            friend vector_const_iterator<U> operator + (vector_const_iterator<U> it, difference_type n) noexcept
            {
                it += n;
                return it;
            }

            template<typename U>
            friend vector_const_iterator<U> operator + (difference_type n, vector_const_iterator<U> it) noexcept
            {
                it += n;
                return it;
            }

            template<typename U>
            friend vector_const_iterator<U> operator - (vector_const_iterator<U> it, difference_type n) noexcept
            {
                it -= n;
                return it;
            }

            template<typename U>
            friend difference_type operator - (const vector_const_iterator<U>& lhs, const vector_const_iterator<U>& rhs) noexcept
            {
                return lhs.current_ - rhs.current_;
            }

            const value_type& operator[] (difference_type n) const noexcept
            {
                return current_[n];
            }

            const value_type& operator*() const noexcept
            {
                return *current_;
            }

            bool operator < (const MyType& it) const noexcept
            {
                return current_ < it.current_;
            }

            bool operator > (const MyType& it) const noexcept
            {
                return current_ > it.current_;
            }

            bool operator >= (const MyType& it) const noexcept
            {
                return current_ >= it.current_;
            }

            bool operator <= (const MyType& it) const noexcept
            {
                return current_ <= it.current_;
            }

            bool operator == (const MyType& it) const noexcept
            {
                return current_ == it.current_;
            }

            bool operator != (const MyType& it) const noexcept
            {
                return current_ != it.current_;
            }
        };
    }

    template<typename ElementT>
    class vector
    {
    public:
        CONTAINER_DEFINE(ElementT)
        using allocator_type = polymorphic_allocator<value_type>;

#ifdef _DEBUG
        using iterator = Internal::vector_iterator<ElementT>;
        using const_iterator = Internal::vector_const_iterator<ElementT>;
#else
        using iterator = pointer;
        using const_iterator = const value_type*;
#endif // DEBUG

        vector() noexcept
            :storage_ {}, size_ {}, capacity_ {}
        {}

        vector(size_type n, memory_resource_ptr resource = {})
            :vector { n, {}, resource }
        {}

        vector(size_type n, const value_type& v, memory_resource_ptr resource = {})
            :vector { resource }
        {
            Reserve(n);
            Yupei::construct_n(storage_, n, v);
            size_ = n;
        }

        explicit vector(memory_resource_ptr resource)
            :storage_ {}, size_ {}, capacity_ {}, allocator_ { resource }
        {}

        template<typename InputItT, typename = std::enable_if_t < is_input_iterator<InputItT>{} >>
            vector(InputItT first, InputItT last, memory_resource_ptr pmr = {})
            : vector { pmr }
        {
            std::for_each(first, last, [&](const iterator_value_type_t<InputItT>& v) {
                push_back(v);
            });
        }

        vector(std::initializer_list<value_type> il)
            :vector { il.begin(), il.end() }
        {}

        vector(const vector& other)
            :vector()
        {
            append(other.begin(), other.end());
        }

        vector& operator=(const vector& other)
        {
            if (this != &other)
                vector(other).swap(*this);
            return *this;
        }

        vector& operator=(vector&& other) noexcept
        {
            if (this != &other)
                vector(std::move(other)).swap(*this);
            return *this;
        }

        vector(vector&& v) noexcept
            :storage_ { v.storage_ },
            size_ { v.size_ },
            capacity_ { v.capacity_ }
        {
            v.storage_ = {};
            v.size_ = {};
            v.capacity_ = {};
        }

        ~vector()
        {
            Yupei::destroy_n(storage_, size());
            allocator_.deallocate(storage_, capacity());
            capacity_ = size_ = {};
            storage_ = {};
        }

        void swap(vector& other) noexcept
        {
            using std::swap;
            swap(storage_, other.storage_);
            swap(size_, other.size_);
            swap(capacity_, other.capacity_);
        }

        reference back() noexcept
        {
            return storage_[size() - 1];
        }

        const_reference back() const noexcept
        {
            return storage_[size() - 1];
        }

        reference front() noexcept
        {
            return storage_[0];
        }

        const_reference front() const noexcept
        {
            return storage_[0];
        }

        size_type max_size() const noexcept
        {
            return size_type(-1) / sizeof(value_type);
        }

        pointer data() noexcept
        {
            return storage_;
        }

        const_pointer data() const noexcept
        {
            return storage_;
        }

        bool empty() const noexcept
        {
            return size() == 0;
        }

        void clear() noexcept
        {
            Yupei::destroy_n(storage_, size());
            size_ = {};
        }

        allocator_type get_allocator() const
        {
            return allocator_;
        }

        void resize(size_type count)
        {
            const auto nowSize = size();
            if (count < nowSize)
                Yupei::destroy_n(storage_ + count, nowSize - count);
            else if (count > nowSize)
            {
                Reserve(count);
                const auto prevEnd = storage_ + nowSize;
                Yupei::construct_n(prevEnd, count - nowSize);
            }
            size_ = count;
        }

        reference operator[](size_type n) noexcept
        {
            YPASSERT(n < size(), "Out of Range!");
            return storage_[n];
        }

        const_reference operator[](size_type n) const
        {
            YPASSERT(n < size(), "Out of Range!");
            return storage_[n];
        }

        reference at(size_type pos)
        {
            if (pos >= size()) throw std::out_of_range("Out of range!");
            return storage_[pos];
        }

        const_reference at(size_type pos) const
        {
            if (pos >= size()) throw std::out_of_range("Out of range!");
            return storage_[pos];
        }

        iterator begin() noexcept
        {
            return MakeIterator(storage_);
        }

        const_iterator begin() const noexcept
        {
            return MakeIterator(storage_);
        }

        iterator end() noexcept
        {
            return MakeIterator(storage_ + size_);
        }

        const_iterator end() const noexcept
        {
            return MakeIterator(storage_ + size_);
        }

        const_iterator cend() const noexcept
        {
            return end();
        }

        const_iterator cbegin() const noexcept
        {
            return begin();
        }

        size_type size() const noexcept
        {
            return size_;
        }

        void push_back(const value_type& v)
        {
            ReserveMore(1);
            AddElementAtLast(v);
        }

        void push_back(value_type&& v)
        {
            ReserveMore(1);
            AddElementAtLast(std::move(v));
        }

        void pop_back(size_type n = 1) noexcept
        {
            Yupei::destroy_n(storage_ + size_ - n, n);
            size_ -= n;
        }

        size_type capacity() const noexcept
        {
            return capacity_;
        }

        void reserve(size_type cap)
        {
            Reserve(cap);
        }

        iterator insert(const_iterator pos, const value_type& value)
        {
            return Insert(1, pos, value);
        }

        iterator insert(const_iterator pos, value_type&& value)
        {
            return Insert(1, pos, std::move(value));
        }

        template<typename IteratorT>
        void append(IteratorT first, IteratorT last)
        {
            if (std::is_base_of<std::forward_iterator_tag, IteratorT>::value)
            {
                const auto dist = std::distance(first, last);
                ReserveMore(dist);
                std::for_each(first, last, [this](const iterator_value_type_t<IteratorT>& v) {
                    AddElementAtLast(v);
                });
            }
            else
            {
                std::for_each(first, last, [this](const iterator_value_type_t<IteratorT>& v) {
                    push_back(v);
                });
            }
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            const auto numToErase = last - first;
            Yupei::destroy_n(std::addressof(*first), numToErase);
            std::move(last, end(), first);
            size_ -= numToErase;
            return last;
        }

        iterator erase(const_iterator pos)
        {
            return erase(pos, pos + 1);
        }

    private:
        template<typename... ParamsT>
        iterator Insert(size_type n, const_iterator pos, ParamsT&&... params)
        {
            const auto newSize = size() + n;
            const auto insertionOffset = pos - cbegin();
            pointer des {};
            ReserveMore(n);
            //因为 reserveMore 可能会重新分配内存，故要将依赖指针的操作放在后面。
            const auto prevEnd = storage_ + size();
            const auto insertionPoint = begin() + insertionOffset;
            if (pos == cend())
            {
                Yupei::construct_n(prevEnd, n, std::forward<ParamsT>(params)...);
                des = prevEnd;
            }
            else
            {
                std::move_backward(insertionPoint, end(), prevEnd + n);
                des = storage_ + insertionOffset;
                Yupei::construct_n(des, n, std::forward<ParamsT>(params)...);
            }
            size_ = newSize;
            return MakeIterator(des);
        }

        iterator MakeIterator(pointer p) noexcept
        {
#ifdef _DEBUG
            return { p, this };
#else
            return { p };
#endif // _DEBUG
        }

        const_iterator MakeIterator(pointer p) const noexcept
        {
#ifdef _DEBUG
            return { p, this };
#else
            return { p };
#endif // _DEBUG
        }

        void ReserveMore(size_type delta)
        {
            const auto oldSize = size_;
            const auto newCapacity = oldSize + delta;
            if (newCapacity < oldSize)
                throw std::bad_array_new_length();
            if (newCapacity <= capacity()) return;
            Reserve(newCapacity);
        }

        void Reserve(size_type newCapacity)
        {
            const auto elementsToAlloc = CalcNewSize(newCapacity);
            const auto newStorage = allocator_.allocate(elementsToAlloc);
            std::move(storage_, storage_ + size_, newStorage);
            allocator_.deallocate(storage_, capacity_);
            capacity_ = elementsToAlloc;
            storage_ = newStorage;
        }

        size_type CalcNewSize(size_type newCapacity) const noexcept
        {
            const auto oldCapacity = capacity_;
            auto elementsToAlloc = oldCapacity + 1;
            elementsToAlloc += (elementsToAlloc >> 1);
            elementsToAlloc = (elementsToAlloc + 0x0Fu) & (size_type)-0x10;

            if (elementsToAlloc < newCapacity) elementsToAlloc = newCapacity;

            return elementsToAlloc;
        }

        template<typename... ParamsT>
        void AddElementAtLast(ParamsT&&... params)
        {
            Yupei::construct(storage_ + size_, std::forward<ParamsT>(params)...);
            ++size_;
        }

        pointer storage_;
        size_type size_;
        size_type capacity_;
        polymorphic_allocator<ElementT> allocator_;
    };

    namespace Internal
    {
        template<typename T>
        auto vector_iterator<T>::operator += (typename vector_iterator<T>::difference_type n) noexcept
            -> vector_iterator<T>&
        {
            current_ += n;
            assert(*this <= container_->end());
            return *this;
        }

        template<typename T>
        auto vector_iterator<T>::operator -= (typename vector_iterator<T>::difference_type n) noexcept
            -> vector_iterator<T>&
        {
            current_ -= n;
            assert(*this >= container_->begin());
            return *this;
        }

        template<typename T>
        auto vector_const_iterator<T>::operator += (typename vector_const_iterator<T>::difference_type n) noexcept
            -> vector_const_iterator<T>&
        {
            current_ += n;
            assert(*this <= container_->end());
            return *this;
        }

        template<typename T>
        auto vector_const_iterator<T>::operator -= (typename vector_const_iterator<T>::difference_type n) noexcept
            -> vector_const_iterator<T>&
        {
            current_ -= n;
            assert(*this >= container_->begin());
            return *this;
        }
    }

    using std::begin;
    using std::end;
    using std::cbegin;
    using std::cend;
    using std::rbegin;
    using std::rend;
    using std::empty;
    using std::data;
    using std::size;
}
