#pragma once

#include "StringView.hpp"
#include "MemoryResource\MemoryResource.hpp"
#include "Ranges\Xrange.hpp"
#include "Limits.hpp"
#include "Containers\Vector.hpp"
#include "Searchers.hpp"
#include <algorithm>
#include <utility>

namespace Yupei
{
    template<string_type StringT>
    class basic_string
    {
    public:
        using view_type = basic_string_view<StringT>;
        using value_type = typename view_type::value_type;
        using size_type = std::size_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;

        basic_string(memory_resource_ptr pmr = {}) noexcept
            :allocator_ {pmr}
        {
            SetSmallSize(0);
        }

        basic_string(size_type n, memory_resource_ptr pmr = {})
            :basic_string(pmr)
        {
            append(value_type {}, n);
        }

        basic_string(const view_type& view, memory_resource_ptr pmr = {})
            :basic_string(pmr)
        {
            append(view);
        }

        basic_string(const_pointer value, size_type startIndex, size_type count, memory_resource_ptr pmr = {})
            :basic_string(view_type {value + startIndex, count}, pmr)
        {}

        basic_string(const basic_string& other)
        {
            append(other);
        }

        basic_string& operator=(const basic_string& other)
        {
            basic_string(other).swap(*this);
            return *this;
        }

        basic_string(basic_string&& other) noexcept
        {
            storage_ = other.storage_;
            other.SetSmallSize(0);
        }

        basic_string& operator=(basic_string&& other) noexcept
        {
            basic_string(std::move(other)).swap(*this);
            return *this;
        }

        ~basic_string()
        {
            if (IsBig())
                allocator_.deallocate(storage_.big_.ptr_, GetBigSize());
            SetSmallSize(0);
        }

        void swap(basic_string& other)
        {
            std::swap(storage_, other.storage_);
        }

        size_type size() const noexcept
        {
            return GetSize();
        }

        view_type to_string_view() const noexcept
        {
            if (IsBig())
                return {storage_.big_.ptr_, GetBigSize()};
            else
                return {storage_.small_.data_, GetSmallSize()};
        }

        size_type capacity() const noexcept
        {
            return GetCapacity();
        }

        pointer data() & noexcept
        {
            return c_str();
        }

        const_pointer data() const & noexcept
        {
            return c_str();
        }

        pointer data() && noexcept = delete;
        const_pointer data() const && noexcept = delete;

        pointer c_str() & noexcept
        {
            if (IsBig())
            {
                const auto str = storage_.big_.ptr_;
                str[storage_.big_.size_] = {};
                return str;
            }
            else
            {
                const auto str = storage_.small_.data_;
                str[storage_.small_.size_] = {};
                return str;
            }
        }

        const_pointer c_str() const & noexcept
        {
            if (IsBig())
            {
                const auto str = storage_.big_.ptr_;
                const_cast<reference>(str[storage_.big_.size_]) = {};
                return str;
            }
            else
            {
                const auto str = storage_.small_.data_;
                const_cast<reference>(str[storage_.small_.size_]) = {};
                return str;
            }
        }

        pointer c_str() && noexcept = delete;
        const_pointer c_str() const && noexcept = delete;

        reference operator[](size_type n) noexcept
        {
            return GetBegin()[n];
        }

        const_reference operator[](size_type n) const noexcept
        {
            return GetBegin()[n];
        }

        reference at(size_type n)
        {
            if (n >= size()) throw std::out_of_range("Out of range!");
            return (*this)[n];
        }

        const_reference at(size_type n) const
        {
            if (n >= size()) throw std::out_of_range("Out of range!");
            return (*this)[n];
        }

        reference front() noexcept
        {
            return *GetBegin();
        }

        const_reference front() const noexcept
        {
            return *GetBegin();
        }

        reference back() noexcept
        {
            return GetEnd()[-1];
        }

        const_reference back() const noexcept
        {
            return GetEnd()[-1];
        }

        iterator begin() noexcept
        {
            return {GetBegin()};
        }

        const_iterator begin() const noexcept
        {
            return {GetBegin()};
        }

        iterator end() noexcept
        {
            return {GetEnd()};
        }

        const_iterator end() const noexcept
        {
            return {GetEnd()};
        }

        void push_back(value_type c)
        {
            ReserveMore(1);
            UncheckedPush(c);
        }

        void pop_back(size_type n = 1) noexcept
        {
            IncrementSize(limits_max_v<size_type> +n - 1);
        }

        void append(value_type c)
        {
            push_back(c);
        }

        void append(const_pointer str)
        {
            append(view_type {str});
        }

        void append(const_pointer str, size_type startIndex, size_type charCount)
        {
            append(view_type {str + startIndex, charCount});
        }

        //这里会警告有未使用的变量 i，等 [[maybe_unused]]。
        void append(value_type value, size_type repeatCount)
        {
            const auto prevSize = size();
            ReserveMore(repeatCount);
            for (auto i : xrange(repeatCount))
                UncheckedPush(value);
            SetSize(prevSize + repeatCount);
        }

        void append(const view_type& view)
        {
            using Yupei::cbegin;
            using Yupei::cend;
            const auto prevSize = size();
            const auto viewSize = view.size();
            const auto newSpace = ReserveMore(viewSize);
            std::copy(cbegin(view), cend(view), newSpace);
            SetSize(prevSize + viewSize);
        }

        void append(const basic_string& value)
        {
            append(value.to_string_view());
        }

        size_type max_size() const noexcept
        {
            return allocator_.max_size();
        }

        void append(const basic_string& value, size_type startIndex, size_type count)
        {
            append(value.to_string_view().substr(startIndex, count));
        }

        //将 count 个 value 插入到 index 之前。
        void insert(size_type index, const view_type& value, size_type count = 1)
        {
            using Yupei::cbegin;
            using Yupei::cend;
            if (count == 0) return;
            const auto prevSize = size();
            const auto charsToInsert = value.size() * count;
            auto insertPlace = MakeRoom(charsToInsert, index, index);
            for (;;)
            {
                insertPlace = std::copy(cbegin(value), cend(value), insertPlace);
                --count;
                if (count == 0)
                    break;
            }
            SetSize(prevSize + charsToInsert);
        }

        void insert(size_type index, const_pointer value, size_type startIndex, size_type charCount)
        {
            insert(index, view_type {value + startIndex, charCount});
        }

        void remove(size_type startIndex, size_type length)
        {
            //TODO: 这里可以更高效。
            assert(startIndex + length < size());
            const auto strBegin = GetBegin();
            const auto strEnd = GetEnd();
            const auto curSize = size();
            const auto removeBegin = strBegin + startIndex;
            std::move(removeBegin + length, strEnd, removeBegin);
            SetSize(curSize - length);
        }

        void replace(const view_type& oldString, const view_type& newString)
        {
            using Yupei::cbegin;
            using Yupei::cend;
            using Yupei::begin;
            using Yupei::end;
            vector<iterator> replacements;
            //TODO: 小字符串应该使用 default searcher。
            const auto searcher = make_boyer_moore_searcher(begin(oldString), end(oldString));
            pair<iterator, iterator> result;            
            const auto strBegin = begin(*this);
            const auto strEnd = end(*this);
            auto searchBegin = strBegin;
            for (;;)
            {
                result = searcher(searchBegin, strEnd);
                if (result.first != strEnd)
                    replacements.push_back(result.first);
                else break;
                searchBegin = result.second;
            }
            if (!replacements.empty())
            {
                const auto oldSize = oldString.size();
                const auto newSize = newString.size();
                const auto prevEntireStringSize = size();

                //TODO: sizeToEnsure 的溢出问题。
                auto sizeToEnsure = prevEntireStringSize;
                if (oldSize > newSize)
                    sizeToEnsure -= (oldSize - newSize) * replacements.size();
                else
                    sizeToEnsure += (newSize - oldSize) * replacements.size();

                //脏。                
                auto oldStorage = strBegin;
                size_type curReplace = {};
                const auto wasBig = IsBig();
                if ((!wasBig && sizeToEnsure < SmallMaxLength - 1) 
                    || sizeToEnsure < capacity() //注意这里，必须严格小于 capacity，因为要保留空字符的位置。
                    )
                {
                    //从 elementsToAllocate 这个位置开始向后移。
                    size_type cur = replacements.size() - 1;
                    auto curProgress = strBegin + sizeToEnsure;
                    auto oldEnd = strEnd;
                    for (; cur != size_type(-1); --cur)
                    {
                        auto replacement = replacements[cur];
                        const auto moveStart = replacement + oldSize;
                        curProgress = std::move_backward(moveStart, oldEnd, curProgress);
                        curProgress = std::copy_backward(cbegin(newString), cend(newString), curProgress);
                        oldEnd = replacement;
                    }
                    SetSize(sizeToEnsure);
                }
                else
                {
                    const auto elementsToAllocate = ((sizeToEnsure + 1 + 0x0Fu) & size_type(-0x10));
                    if (elementsToAllocate < sizeToEnsure) throw std::bad_array_new_length();

                    const auto newStorage = allocator_.allocate(elementsToAllocate);
                    auto curProgress = newStorage;
                    for (; curReplace != replacements.size();)
                    {
                        const auto oldEnd = replacements[curReplace];
                        curProgress = std::copy(oldStorage, oldEnd, curProgress);
                        curProgress = std::copy(cbegin(newString), cend(newString), curProgress);
                        oldStorage = oldEnd + oldString.size();
                        ++curReplace;
                    }
                    std::copy(oldStorage, strEnd, curProgress);
                    SetBigCapacity(elementsToAllocate);
                    if (wasBig) allocator_.deallocate(storage_.big_.ptr_, GetBigSize());
                    storage_.big_.ptr_ = newStorage;
                    storage_.big_.size_ = sizeToEnsure;
                }                                                                   
            }
        }

        void replace(value_type oldChar, value_type newChar) noexcept
        {
            for (auto& c : *this)
                if (c == oldChar)
                    c = newChar;
        }

    private:
        struct Big
        {
            size_type capacity_;
            size_type size_;
            pointer ptr_;
        };

        static constexpr size_type SmallMaxLength = max(2, sizeof(Big) / sizeof(value_type));
        using UnsignedCharT = std::make_unsigned_t<value_type>;

        //padding?
        struct Small
        {
            UnsignedCharT size_;
            value_type data_[SmallMaxLength - 1];
        };

        union Storage
        {
            Big big_;
            Small small_;
        }storage_;

        polymorphic_allocator<value_type> allocator_;

        static constexpr size_type BigMask = 1;
        static constexpr UnsignedCharT SmallMask = 1;

        pointer ReserveMore(size_type delta)
        {
            const auto curSize = size();
            const auto sizeToEnsure = curSize + delta;
            if (sizeToEnsure < curSize) throw std::bad_array_new_length();
            return MakeRoom(delta, curSize, curSize);
        }

        void Reserve(size_type newCap)
        {
            const auto oldCap = capacity();
            if (newCap <= oldCap) return;
            (void)ReserveMore(newCap - oldCap);
        }

        //例如 Insert、Replace 这些函数都会有类似的要求：
        //先预留好空间。拷贝前半部分，这时在新区域加入或替换，再拷贝后半部分。
        //这个函数的作用：在 copyEndIndex1 与 copyStartIndex2 之间留出 count 的空隙。
        pointer MakeRoom(size_type count, size_type copyEndIndex1, size_type copyStartIndex2)
        {
            assert(copyEndIndex1 <= copyStartIndex2);
            const auto str = GetBegin();
            const auto strEnd = GetEnd();
            const auto copyPrevEnd = str + copyEndIndex1;
            const auto copyLastStart = str + copyStartIndex2;
            const auto prevSize = size();
            const auto elementsRequired = prevSize + count;
            const auto isBig = IsBig();

            //这里已经处理了 Small 的情况。
            const auto curCap = capacity();
            if (elementsRequired <= curCap)
            {
                std::move_backward(copyLastStart, strEnd, strEnd + count);
                return str + copyEndIndex1;
            }

            const auto elementsToAllocate = ((elementsRequired + 1 + 0x0Fu) & size_type(-0x10));
            if (elementsToAllocate < elementsRequired || elementsToAllocate >= max_size()) throw std::bad_array_new_length();

            const auto newStorage = allocator_.allocate(elementsToAllocate);
            if (copyPrevEnd == strEnd && copyLastStart == strEnd)
                std::copy(str, strEnd, newStorage);//在末尾留 count，直接全部拷贝过去。
            else
            {
                const auto des = std::copy(str, copyPrevEnd, newStorage);
                std::copy(copyLastStart, strEnd, des + count);
            }

            SetBigCapacity(elementsToAllocate);
            if (isBig) allocator_.deallocate(storage_.big_.ptr_, prevSize);
            storage_.big_.ptr_ = newStorage;
            return newStorage + copyEndIndex1;
        }

        void UncheckedPush(value_type c) noexcept
        {
            assert(capacity() >= size() + 1);
            if (IsBig())
            {
                const auto prevSize = GetBigSize();
                storage_.big_.ptr_[prevSize] = c;
                SetBigSize(prevSize + 1);
            }
            else
            {
                const auto prevSize = GetSmallSize();
                storage_.small_.data_[prevSize] = c;
                SetSmallSize(prevSize + 1);
            }
        }

        bool IsBig() const noexcept
        {
            //如果正在使用 Small，那么 capacity 最低位为 0，否则，存储的 capacity 为奇数（最低位为 1）（实际为偶数）。
            return (storage_.big_.capacity_ & BigMask) != 0;
        }

        void SetSize(size_type s) noexcept
        {
            if (IsBig())
                SetBigSize(s);
            else
                SetSmallSize(s);
        }

        void IncrementSize(size_type delta) noexcept
        {
            if (IsBig())
                storage_.big_.size_ += delta;
            else
            {
                assert(GetSmallSize() + delta <= SmallMaxLength - 1);
                SetSmallSize(GetSmallSize() + delta);
            }
        }

        size_type GetSmallSize() const noexcept
        {
            return storage_.small_.size_ >> 1;
        }

        void SetSmallSize(size_type s) noexcept
        {
            assert(s < SmallMaxLength);
            storage_.small_.size_ = (s << 1);
        }

        size_type GetBigSize() const noexcept
        {
            assert(IsBig());
            return storage_.big_.size_;
        }

        void SetBigSize(size_type s) noexcept
        {
            assert(IsBig());
            storage_.big_.size_ = s;
        }

        //Capacity 只能是偶数，正是我们期望的。
        void SetBigCapacity(size_type s) noexcept
        {
            storage_.big_.capacity_ = s | BigMask;
        }

        size_type GetBigCapacity() const noexcept
        {
            return storage_.big_.capacity_ & ~(BigMask);
        }

        size_type GetSize() const noexcept
        {
            if (IsBig()) return GetBigSize();
            else return GetSmallSize();
        }

        pointer GetBegin() noexcept
        {
            if (IsBig()) return storage_.big_.ptr_;
            else return storage_.small_.data_;
        }

        const_pointer GetBegin() const noexcept
        {
            if (IsBig()) return storage_.big_.ptr_;
            else return storage_.small_.data_;
        }

        pointer GetEnd() noexcept
        {
            if (IsBig()) return storage_.big_.ptr_ + GetBigSize();
            else return storage_.small_.data_ + GetSmallSize();
        }

        const_pointer GetEnd() const noexcept
        {
            if (IsBig()) return storage_.big_.ptr_ + GetBigSize();
            else return storage_.small_.data_ + GetSmallSize();
        }

        size_type GetCapacity() const noexcept
        {
            return (IsBig() ? GetBigCapacity() : SmallMaxLength - 1);
        }
    };

    template<string_type StringT>
    decltype(auto) begin(basic_string<StringT>& str) noexcept
    {
        return str.begin();
    }

    template<string_type StringT>
    decltype(auto) begin(const basic_string<StringT>& str) noexcept
    {
        return str.begin();
    }

    template<string_type StringT>
    decltype(auto) cbegin(const basic_string<StringT>& str) noexcept
    {
        return begin(str);
    }

    template<string_type StringT>
    decltype(auto) end(basic_string<StringT>& str) noexcept
    {
        return str.end();
    }

    template<string_type StringT>
    decltype(auto) end(const basic_string<StringT>& str) noexcept
    {
        return str.end();
    }

    template<string_type StringT>
    decltype(auto) cend(const basic_string<StringT>& str) noexcept
    {
        return end(str);
    }

    template<string_type StringT>
    decltype(auto) size(const basic_string<StringT>& str) noexcept
    {
        return str.size();
    }

    extern template class basic_string<string_type::wide>;
    extern template class basic_string<string_type::utf8>;
    extern template class basic_string<string_type::utf16>;
    extern template class basic_string<string_type::utf32>;

    using wide_string = basic_string<string_type::wide>;
    using utf8_string = basic_string<string_type::utf8>;
    using utf16_string = basic_string<string_type::utf16>;
    using utf32_string = basic_string<string_type::utf32>;
}