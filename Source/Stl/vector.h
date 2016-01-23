#pragma once

#include "type_traits.h"
#include "MemoryReasource\memory_resource.h"
#include "HelperMacros.h"
#include "..\CLib\raw_memory.h"
#include "MemoryInternal.h"
#include "algorithm.h"

namespace Yupei
{
    template<typename ElementT>
    class vector;

    namespace Internal
    {
        template<typename T>
        class vector_iterator
        { 
        private:
            using MyType = vector_iterator<T>;
            using difference_type = std::ptrdiff_t;
            using iterator_category = random_access_iterator_tag;

            template<typename U>
            friend class Yupei::vector;

            template<typename T>
            friend constexpr auto do_pointer_from(vector_iterator<T> it) noexcept -> T*
            {
                return it.current_;
            }

            template<typename U>
            friend class vector_const_iterator;

            using ContainerType = Yupei::vector<T>;

            T* current_;
            ContainerType* container_;

            vector_iterator(T* p, ContainerType* container)
                :current_{p},
                container_{container}
            {}

        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using iterator_category = random_access_iterator_tag;
            using pointer = value_type*;
            using reference = value_type&;
           

            constexpr vector_iterator() noexcept
                :current_{},
                container_{}
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
            friend vector_iterator<U> operator + (difference_type n,vector_iterator<U> it) noexcept
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
        private:
            using MyType = vector_const_iterator<T>;
            
            template<typename U>
            friend class Yupei::vector;

            using ContainerType = Yupei::vector<T>;

            template<typename T>
            friend constexpr auto do_pointer_from(vector_const_iterator<T> it) noexcept -> T*
            {
                return it.current_;
            }

            const T* current_;
            const ContainerType* container_;

            vector_const_iterator(const T* p, const ContainerType* container)
                :current_{p},
                container_{container}
            {}

            
        public:
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using iterator_category = random_access_iterator_tag;
            using pointer = const value_type*;
            using reference = const value_type&;
            
            constexpr vector_const_iterator() noexcept
                :current_{},
                container_{}
            {}

            vector_const_iterator(const vector_iterator<T>& it)
                :current_{it.current_},
                container_{it.container_}
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
	class vector : private polymorphic_allocator<ElementT>
	{
	public:

		CONTAINER_DEFINE(ElementT);
		using allocator_type = polymorphic_allocator<value_type>;

#ifdef _DEBUG
        using iterator = Internal::vector_iterator<ElementT>;
        using const_iterator = Internal::vector_const_iterator<ElementT>;
#else
        using iterator = pointer;
        using const_iterator = const value_type*;
#endif // DEBUG

		vector() noexcept
			:storage_{}, size_{}, capacity_{}
		{}

        vector(size_type n)
            :vector()
        {
            Reserve(n);
            for (size_type i{}; i < n; ++i)
                AddElementAtLast();
            size_ = n;
        }

        vector(memory_resource_ptr resource)
            :allocator_type{resource}, storage_ {}, size_{}, capacity_{}
        {}

        vector(const vector& other)            
        {            
            append(other.begin(), other.end());
        }

        vector& operator = (const vector& other)
        {
            if (this != &other)
                vector(other).swap(*this);
            return *this;
        }

        vector& operator = (vector&& other) noexcept
        {
            if (this != &other)
                vector(Yupei::move(other)).swap(*this);
            return *this;
        }

        vector(vector&& v) noexcept
            :storage_{v.storage_},
            size_{v.size_},
            capacity_{v.capacity_}
        {
            v.storage_ = {};
            v.size_ = {};
            v.capacity_ = {};
        }

        ~vector()
        {
            Yupei::destroy_n(storage_, size());
            this->deallocate(storage_, capacity());
            capacity_ = size_ = {};
            storage_ = {};
        }

        void swap(vector& other) noexcept
        {
            Yupei::swap(storage_, other.storage_);
            Yupei::swap(size_, other.size_);
            Yupei::swap(capacity_, other.capacity_); 
        }

        reference back() noexcept
        {
            return storage[size() - 1];
        }

        const_reference back() const noexcept
        {
            return storage[size() - 1];
        }

        reference front() noexcept
        {
            return storage[0];
        }

        const_reference front() const noexcept
        {
            return storage[0];
        }

		size_type max_size() const noexcept
		{
			return size_type(-1) / sizeof(value_type);
		}

        value_type* data() noexcept
        {
            return storage_;
        }

        const value_type* data() const noexcept
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

        void resize(size_type count)
        {
            const auto nowSize = size();
            if (count < nowSize)
                Yupei::destroy_n(storage_ + count, nowSize - count);
            else if (count > nowSize)
            {
                Reserve(count);
                const auto delta = count - nowSize;
                for (size_type i{};i < delta;++i)
                    AddElementAtLast();
            }
            size_ = count;
        }

		reference operator[](size_type n) noexcept
		{
			return storage_[n];
		}

        reference at(size_type pos)
        {
            if (pos < size()) throw std::out_of_range("Out of range!");
            return storage_[pos];
        }

        const_reference at(size_type pos) const
        {
            if (pos < size()) throw std::out_of_range("Out of range!");
            return storage_[pos];
        }

        iterator begin() noexcept
        {
#ifdef _DEBUG
            return {storage_,this};
#else
            return {storage_};
#endif
        }

        const_iterator begin() const noexcept
        {
#ifdef _DEBUG
            return {storage_,this};
#else
            return {storage_};
#endif 
        }

        iterator end() noexcept
        {
#ifdef _DEBUG
            return {storage_ + size_,this};
#else
            return {storage_ + size_};
#endif
        }

        const_iterator end() const noexcept
        {
#ifdef _DEBUG
            return {storage_ + size_,this};
#else
            return {storage_ + size_};
#endif
        }

        const_iterator cend() const noexcept
        {
#ifdef _DEBUG
            return {storage_ + size_,this};
#else
            return {storage_ + size_};
#endif
        }

        const_iterator cbegin() const noexcept
        {
#ifdef _DEBUG
            return {storage_,this};
#else
            return {storage_};
#endif 
        }

        size_type size() const noexcept
        {
            return size_;
        }

        const_reference operator[](size_type n) const
        {
            return storage_[n];
        }

		void push_back(const value_type& v)
		{	
            ReserveMore(1);
			AddElementAtLast(v);
		}

		void push_back(value_type&& v)
		{			
            ReserveMore(1);
			AddElementAtLast(Yupei::move(v));
		}

        void pop_back(size_type n = 1) noexcept
        {
            for (size_type i = 0;i < n;++i)
                Yupei::destroy(storage_ + size_ - i);
            size_ -= n;
        }

        size_type capacity() const noexcept
        {
            return capacity_;
        }

        iterator insert(const_iterator pos, const value_type& value)
        {
            return Insert(1, pos, value);
        }

        iterator insert(const_iterator pos, value_type&& value)
        {
            return Insert(1, pos, Yupei::move(value));
        }

        template<typename IteratorT>
        void append(IteratorT first, IteratorT last)
        {
            const auto couldGetDistance = is_base_of<forward_iterator_tag, IteratorT>::value;
            if (couldGetDistance)
            {
                const auto dist = Yupei::distance(first, last);
                ReserveMore(dist);
                Yupei::for_each(first, last, [this](auto&& v) {
                    this->AddElementAtLast(Yupei::forward<decltype(v)&&>(v));
                });
            }
            else
            {
                Yupei::for_each(first, last, [this](auto&& v) {
                    this->push_back(Yupei::forward<decltype(v) && >(v));
                });
            }
        }

        iterator erase(const_iterator first, const_iterator last)
        {
            const auto eraseStartOffset = first - cbegin();
            const auto eraseCount = last - first;
            const auto eraseStart = storage_ + eraseStartOffset;
            Yupei::destroy_n(eraseStart, eraseCount);
            auto prevEnd = cend();
            Yupei::move(last, prevEnd, eraseStart);
            Yupei::destroy(prevEnd.current_ - 1);
            size_ -= eraseCount;
#ifdef _DEBUG
            return {storage_ + eraseStartOffset, this};
#else
            return {storage_ + eraseStartOffset};
#endif // _DEBUG           
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
            const auto insertionPoint = begin() + insertionOffset;
            pointer des{};
            if (pos == cend())
            {
                ReserveMore(n);
                const auto lastPos = storage_ + size();
                for (size_type i{};i < n;++i)
                    AddElementAtLast(Yupei::forward<ParamsT>(params)...);
                des = lastPos;
            }
            else if (newSize > capacity())
            {              
                const auto elementsToAlloc = CalcNewSize(newSize);
                const auto newStorage = this->allocate(elementsToAlloc);                
                des = Move(storage_, insertionOffset, newStorage);
                des = Yupei::construct_n(des, n, Yupei::forward<ParamsT>(params)...);
                Move(storage_ + insertionOffset, size() - insertionOffset, des);
                this->deallocate(storage_, capacity());
                capacity_ = elementsToAlloc;
                storage_ = newStorage;  
                size_ += n;
            }
            else
            {
                auto prevEnd = storage_ + size();                                               
                for (size_type i{}; i < n;++i)
                    AddElementAtLast(Yupei::forward<ParamsT>(params)...);  
                des = Yupei::rotate(storage_ + insertionOffset, prevEnd, prevEnd + n);                              
            }
            
#ifdef _DEBUG
            return{des,this};
#else
            return{des};
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
            const auto newMem = this->allocate(elementsToAlloc);
            this->Move(storage_, size_, newMem);
            this->deallocate(storage_, capacity_);
            capacity_ = elementsToAlloc;
            storage_ = newMem;
		}

        size_type CalcNewSize(size_type newCapacity) const noexcept
        {
            const auto oldCapacity = capacity_;

            if (newCapacity < oldCapacity) return;
            auto elementsToAlloc = oldCapacity + 1;
            elementsToAlloc += (elementsToAlloc >> 1);
            elementsToAlloc = (elementsToAlloc + 0x0Fu) & (size_type)-0x10;
          
            if (elementsToAlloc < newCapacity) elementsToAlloc = newCapacity;

            return elementsToAlloc;
        }

		template<typename... ParamsT>
		void AddElementAtLast(ParamsT&&... params)
		{			
			Yupei::construct(storage_ + size_, Yupei::forward<ParamsT>(params)...);
			size_ += 1;
		}

		static pointer MoveImp(pointer src, size_type size, pointer des, true_type)
		{
			return static_cast<pointer>(memcpy(static_cast<void*>(des), static_cast<const void*>(src), size * sizeof(value_type)));
		}

		static pointer MoveImp(pointer src, size_type size, pointer des, false_type)
		{
            for (size_type i = {}; i < size; ++i, ++des, ++src)
			{
				Yupei::construct(des, Yupei::move(*src));
				Yupei::destroy(src);
			}
			return des;
		}

		static pointer Move(pointer src, size_type size, pointer des)
		{
			return MoveImp(src, size, des, is_pod<value_type>());
		}

		pointer storage_;
		size_type size_;
		size_type capacity_;		
	};

    template<typename ElementT>
    inline decltype(auto) cbegin(const vector<ElementT>& v)
    {
        return v.begin();
    }

    template<typename ElementT>
    inline decltype(auto) cend(const vector<ElementT>& v)
    {
        return v.end();
    }

    namespace Internal
    {

        template<typename T>
        auto vector_iterator<T>::operator += (vector_iterator<T>::difference_type n) noexcept
            -> vector_iterator<T>&
        {
            current_ += n;
            assert(*this > container_->end());
            return *this;
        }

        template<typename T>
        auto vector_iterator<T>::operator -= (vector_iterator<T>::difference_type n) noexcept
            -> vector_iterator<T>&
        {
            current_ -= n;
            assert(*this < container_->begin());
            return *this;
        }

        template<typename T>
        auto vector_const_iterator<T>::operator += (vector_const_iterator<T>::difference_type n) noexcept
            -> vector_const_iterator<T>&
        {
            current_ += n;
            assert(*this > container_->end());
            return *this;
        }

        template<typename T>
        auto vector_const_iterator<T>::operator -= (vector_const_iterator<T>::difference_type n) noexcept
            -> vector_const_iterator<T>&
        {
            current_ -= n;
            assert(*this < container_->begin());
            return *this;
        }
    }
}
