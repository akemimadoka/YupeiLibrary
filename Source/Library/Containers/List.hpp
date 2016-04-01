#pragma once

#include "..\SmartPointers\UniquePtr.hpp"
#include "..\MemoryResource\MemoryResource.hpp"
#include "..\HelperMacros.hpp"
#include "..\ConstructDestruct.hpp"
#include "..\Iterator.hpp"
#include "..\Utility.hpp"
#include "..\Assert.hpp"
#include <algorithm>
#include <initializer_list>

namespace Yupei
{
    template<typename>
    class list;

    namespace Internal
    {
        template<typename T>
        struct ListNode;

        template<typename T>
        struct ListNodeBase;

        template<typename T>
        struct NodeTraits
        {
            using NodeBaseType = ListNodeBase<T>;
            using NodeType = ListNode<T>;
            using LinkPointer = NodeBaseType*;
            using NodePointer = NodeType*;

            static auto AsNode(ListNodeBase<T>* p) noexcept
                -> NodePointer
            {
                return reinterpret_cast<ListNode<T>*>(p);
            }

            static auto AsNode(ListNode<T>* p) noexcept
                -> NodePointer
            {
                return p;
            }
        };

        template<typename T>
        struct ListNodeBase
        {
        private:
            using LinkPointer = typename NodeTraits<T>::LinkPointer;
            using NodePointer = typename NodeTraits<T>::NodePointer;
            using NodeBaseType = typename NodeTraits<T>::NodeBaseType;

        public:
            LinkPointer prev_;
            LinkPointer next_;

            LinkPointer self() noexcept
            {
                return this;
            }

            LinkPointer self() const noexcept
            {
                return const_cast<LinkPointer>(this);
            }

            NodePointer AsNode() const noexcept
            {
                return NodeTraits<T>::AsNode(this);
            }

            NodePointer AsNode() noexcept
            {
                return NodeTraits<T>::AsNode(this);
            }

            ListNodeBase() noexcept
                :prev_{self()},
                next_{self()}
            {}
        };

        template<typename T>
        struct ListNode : public ListNodeBase<T>
        {
        private:
            using NodePointer = typename NodeTraits<T>::NodePointer;

        public:
            T item_;

            NodePointer AsNode() noexcept
            {
                return NodeTraits<T>::AsNode(this);
            }
        };

        template<typename T>
        class ListIterator
        {
            using NodePointer = typename Internal::NodeTraits<T>::NodePointer;
            using LinkPointer = typename Internal::NodeTraits<T>::LinkPointer;


            LinkPointer cur_;

            template<typename>
            friend class Yupei::list;

            template<typename>
            friend class ListConstIterator;

            explicit ListIterator(LinkPointer p) noexcept
                :cur_{p}
            {}

        public:
            using value_type = T;
            using reference = value_type&;
            using pointer = value_type*;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::bidirectional_iterator_tag;

            constexpr ListIterator() noexcept
                : cur_{}
            {}

            reference operator*() const noexcept
            {
                return NodeTraits<T>::AsNode(cur_)->item_;
            }

            pointer operator->() const noexcept
            {
                return std::addressof(Internal::NodeTraits<T>::AsNode(cur_)->item_);
            }

            ListIterator& operator++() noexcept
            {
                cur_ = cur_->next_;
                return *this;
            }

            ListIterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            ListIterator& operator--() noexcept
            {
                cur_ = cur_->prev_;
                return *this;
            }

            ListIterator operator--(int) noexcept
            {
                auto tmp = *this;
                --*this;
                return tmp;
            }

            bool operator==(const ListIterator& rhs) const noexcept
            {
                return cur_ == rhs.cur_;
            }

            bool operator!=(const ListIterator& rhs) const noexcept
            {
                return cur_ != rhs.cur_;
            }
        };

        template<typename T>
        class ListConstIterator
        {
            using NodePointer = Internal::ListNode<T>*;
            using LinkPointer = typename Internal::NodeTraits<T>::LinkPointer;
            LinkPointer cur_;

            template<typename>
            friend class Yupei::list;

            ListConstIterator(LinkPointer p) noexcept
                :cur_{p}
            {}

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using reference = const value_type&;
            using pointer = const value_type*;
            using iterator_category = std::bidirectional_iterator_tag;

            constexpr ListConstIterator() noexcept
                : cur_{}
            {}

            ListConstIterator(const ListIterator<T>& it) noexcept
                : cur_{it.cur_}
            {}

            reference operator*() const noexcept
            {
                return NodeTraits<T>::AsNode(cur_)->item_;
            }

            pointer operator->() const noexcept
            {
                return std::addressof(NodeTraits<T>::AsNode(cur_)->item_);
            }

            ListConstIterator& operator++() noexcept
            {
                cur_ = cur_->next_;
                return *this;
            }

            ListConstIterator operator++(int) noexcept
            {
                auto tmp = *this;
                ++*this;
                return tmp;
            }

            ListConstIterator& operator--() noexcept
            {
                cur_ = cur_->prev_;
                return *this;
            }

            ListConstIterator operator--(int) noexcept
            {
                auto tmp = *this;
                --*this;
                return tmp;
            }

            bool operator==(const ListConstIterator& rhs) const noexcept
            {
                return cur_ == rhs.cur_;
            }

            bool operator!=(const ListConstIterator& rhs) const noexcept
            {
                return cur_ != rhs.cur_;
            }
        };
    }


    template<typename T>
    class list
    {
    public:
        CONTAINER_DEFINE(T)
            using iterator = Internal::ListIterator<T>;
        using const_iterator = Internal::ListConstIterator<T>;
        using allocator_type = polymorphic_allocator<T>;

    private:
        using NodeTraits = Internal::NodeTraits<value_type>;
        using NodeType = typename NodeTraits::NodeType;
        using NodeBaseType = typename NodeTraits::NodeBaseType;
        using LinkPointer = typename NodeTraits::LinkPointer;
        using NodePointer = NodeType*;

    public:
        list() noexcept
            : count_{}
        {}

        list(size_type count, const value_type& v, memory_resource_ptr pmr = {})
            :allocator_{pmr}, count_{}
        {
            for (size_type i{}; i < count; ++i)
                push_back(v);
        }

        explicit list(memory_resource_ptr pmr)
            :count_{},
            allocator_{pmr}
        {}

        explicit list(size_type n, memory_resource_ptr pmr = {})
            :allocator_{pmr}, count_{}
        {
            for (size_type i{}; i < n; ++i)
                emplace_back();
        }

        template<typename InputItT, std::enable_if_t<is_input_iterator<InputItT>::value, int> = 0>
        list(InputItT first, InputItT last, memory_resource_ptr pmr = {})
            :allocator_{pmr}, count_{}
        {
            std::for_each(first, last, [&](const value_type_t<InputItT>& v) {
                push_back(v);
            });
        }

        list(std::initializer_list<value_type> il)
            :list{il.begin(), il.end()}
        {}

        list(const list& other)
            :list{other.allocator_.resource()}
        {
            insert(cend(), other.begin(), other.end());
        }

        list(list&& other) noexcept
            : head_{other.head_},
            count_{other.count_}
        {
            other.head_ = {};
            other.count_ = {};
        }

        list& operator=(list&& other) noexcept
        {
            list(std::move(other)).swap(*this);
            return *this;
        }

        list& operator=(const list& other)
        {
            list(other).swap(*this);
            return *this;
        }

        ~list()
        {
            clear();
        }

        reference front()
        {
            YPASSERT(!empty(), "Call front on an empty list!");
            return head_.next_->AsNode()->item_;
        }

        const_reference front() const
        {
            YPASSERT(!empty(), "Call front on an empty list!");
            return head_.next_->AsNode()->item_;
        }

        reference back()
        {
            YPASSERT(!empty(), "Call back on an empty list!");
            return head_.prev_->AsNode()->item_;
        }

        const_reference back() const
        {
            YPASSERT(!empty(), "Call back on an empty list!");
            return head_.prev_->AsNode()->item_;
        }

        void swap(list& other) noexcept
        {
            using std::swap;
            swap(head_.next_, other.head_.next_);
            swap(head_.prev_, other.head_.prev_);
            swap(count_, other.count_);

            if (empty())
                head_.next_ = head_.prev_ = head_.self();
            else
                head_.prev_->next_ = head_.next_->prev_ = head_.self();

            if (other.empty())
                other.head_.next_ = other.head_.prev_ = other.head_.self();
            else
                other.head_.prev_->next_ = other.head_.next_->prev_ = other.head_.self();
        }

    private:
        template<typename ...Args>
        auto MakeNode(Args&&... args)
        {
            const auto deleteFn = [this](NodePointer p) {allocator_.deallocate(p, 1);};
            unique_ptr<NodeType, decltype(deleteFn)> holder{allocator_.allocate(1),deleteFn};
            Yupei::construct(std::addressof(holder->item_), std::forward<Args>(args)...);
            return holder;
        }

    public:
        void push_back(const value_type& v)
        {
            insert(cend(), v);
        }

        void push_back(value_type&& v)
        {
            insert(cend(), std::move(v));
        }

        void push_front(const value_type& v)
        {
            insert(cbegin(), v);
        }

        void push_front(value_type&& v)
        {
            insert(cbegin(), std::move(v));
        }

        template<typename... Args>
        void emplace_front(Args&&... args)
        {
            Insert(cbegin(), std::forward<Args>(args)...);
        }

        template<typename... Args>
        void emplace_back(Args&&... args)
        {
            Insert(cend(), std::forward<Args>(args)...);
        }

        void pop_back() noexcept
        {
            if (count_ != 0)
            {
                const auto n = head_.prev_;
                UnlinkNode(n, n);
                DeallocateNode(NodeTraits::AsNode(n));
                --count_;
            }
        }

        void pop_front() noexcept
        {
            if (count_ != 0)
            {
                const auto n = head_.next_;
                UnlinkNode(n, n);
                DeallocateNode(NodeTraits::AsNode(n));
                --count_;
            }
        }

        iterator insert(const_iterator position, const value_type& v)
        {
            return Insert(position, v);
        }

        iterator insert(const_iterator position, value_type&& v)
        {
            return Insert(position, std::move(v));
        }

        template<typename InputItT>
        iterator insert(const_iterator position, InputItT first, InputItT last)
        {
            if (first == last) return iterator{position.cur_};
            auto it = insert(position, *first);
            std::for_each(++first, last, [&](const value_type_t<InputItT>& v) {
                insert(position, v);
            });
            return it;
        }

        iterator insert(const_iterator pos, std::initializer_list<T> ilist)
        {
            return insert(pos, ilist.begin(), ilist.end());
        }

        iterator erase(const_iterator position)
        {
            auto cur = position.cur_;
            UnlinkNode(cur, cur);
            auto ret = cur->next_;
            DeallocateNode(NodeTraits::AsNode(cur));
            --count_;
            return iterator{ret};
        }

        template<typename... Args>
        iterator emplace(const_iterator pos, Args&&... args)
        {
            return Insert(pos, std::forward<Args>(args)...);
        }

        iterator erase(const_iterator position, const_iterator last)
        {
            if (position != last)
            {
                auto start = position.cur_->AsNode();
                auto lastNode = last.cur_;
                UnlinkNode(start, lastNode->prev_->AsNode());
                size_type n = {};
                while (start != lastNode)
                {
                    auto next = start->next_->AsNode();
                    DeallocateNode(start);
                    start = next;
                    ++n;
                }
                count_ -= n;
                return iterator{lastNode};
            }
            return iterator{position.cur_};
        }

        void clear() noexcept
        {
            erase(cbegin(), cend());
        }

        iterator begin() noexcept
        {
            return iterator{head_.next_};
        }

        const_iterator begin() const noexcept
        {
            return const_iterator{head_.next_};
        }

        iterator end() noexcept
        {
            return iterator{head_.self()};
        }

        const_iterator end() const noexcept
        {
            return const_iterator{head_.self()};
        }

        const_iterator cbegin() const noexcept
        {
            return const_iterator{head_.next_};
        }

        const_iterator cend() const noexcept
        {
            return const_iterator{head_.self()};
        }

        bool empty() const noexcept
        {
            return count_ == 0;
        }

        size_type size() const noexcept
        {
            return count_;
        }

        void splice(const_iterator position, list& other)
        {
            if (!other.empty())
            {
                auto firstNode = other.cbegin().cur_;
                auto lastNode = other.cend().cur_;
                other.UnlinkNode(firstNode, lastNode->prev_);
                auto pos = position.cur_;
                InsertInterval(pos, firstNode, lastNode->prev_);
                count_ += other.count_;
                other.count_ = {};
            }
        }

        void splice(const_iterator position, list& other, const_iterator i)
        {
            if (other.count_ >= 1)
            {
                auto cur = i.cur_->AsNode();
                other.UnlinkNode(cur, cur);
                auto pos = position.cur_;
                InsertInterval(pos->AsNode(), cur, cur);
                other.count_ -= 1;
                count_ += 1;
            }
        }

        void splice(const_iterator position, list& other, const_iterator first, const_iterator last)
        {
            auto firstNode = first.cur_->AsNode();
            auto lastNode = last.cur_->AsNode();
            auto n = std::distance(first, last);
            auto prev = lastNode->prev_->AsNode();
            other.UnlinkNode(firstNode, prev);
            auto pos = position.cur_->AsNode();
            InsertInterval(pos, firstNode, prev);
            other.count_ -= n;
            count_ += n;
        }

        void reverse() noexcept
        {
            auto cur = head_.next_;
            auto head = head_.self();
            while (cur != head)
            {
                auto next = cur->next_;
                std::swap(cur->next_, cur->prev_);
                cur = next;
            }
            std::swap(head_.prev_, head_.next_);
        }

        template<typename CompareT>
        void merge(list& x, CompareT comp)
        {
            auto first1 = cbegin();
            auto last1 = cend();
            auto first2 = x.cbegin();
            auto last2 = x.cend();

            while (first1 != last1 && first2 != last2)
            {
                if (comp(*first2, *first1))
                {
                    auto next = first2;
                    ++next;
                    splice(first1, x, first2);
                    first2 = next;
                }
                else ++first1;
            }

            if (first2 != last2)
                splice(last1, x, first2, last2);
        }

        void merge(list& x)
        {
            merge(x, less<>());
        }

        template<typename CompareT>
        void sort(CompareT compare)
        {
            if (size() > 1)
            {
                list buckets[64];
                list carry;
                list* fill = &buckets[0];
                list* counter;

                do
                {
                    carry.splice(carry.begin(), *this, begin());

                    for (counter = &buckets[0];counter != fill && !counter->empty();++counter)
                    {
                        counter->merge(carry);
                        carry.swap(*counter);
                    }
                    carry.swap(*counter);
                    if (counter == fill) ++fill;
                } while (!empty());

                for (counter = &buckets[1]; counter != fill;++counter)
                    counter->merge(counter[-1]);
                this->swap(fill[-1]);
            }
        }

        void sort()
        {
            sort(less<>());
        }

    private:
        Internal::ListNodeBase<T> head_;
        size_type count_;
        polymorphic_allocator<Internal::ListNode<T>> allocator_;

        void DeallocateNode(NodePointer node)
        {
            Yupei::destroy(std::addressof(node->item_));
            allocator_.deallocate(node, 1);
        }

        //insert before pos
        template<typename ...Args>
        iterator Insert(const_iterator position, Args&&... args)
        {
            auto holder = MakeNode(std::forward<Args>(args)...);
            auto node = holder.release();
            auto cur = position.cur_;
            auto prev = cur->prev_;
            node->next_ = cur;
            cur->prev_ = node;
            prev->next_ = node;
            node->prev_ = prev;
            ++count_;
            return iterator{node};
        }

        //[first,last]
        void InsertInterval(LinkPointer position, LinkPointer first, LinkPointer last)
        {
            auto prev = position->prev_;
            position->prev_ = last;
            last->next_ = position;
            prev->next_ = first;
            first->prev_ = prev;
        }

        void UnlinkNode(const NodeBaseType* first, const NodeBaseType* last) noexcept
        {
            auto prev = first->prev_;
            first->prev_->next_ = last->next_;
            last->next_->prev_ = prev;
        }
    };

    template<typename T>
    decltype(auto) begin(const list<T>& l) noexcept
    {
        return l.begin();
    }

    template<typename T>
    decltype(auto) begin(list<T>& l) noexcept
    {
        return l.begin();
    }

    template<typename T>
    decltype(auto) end(const list<T>& l) noexcept
    {
        return l.end();
    }

    template<typename T>
    decltype(auto) end(list<T>& l) noexcept
    {
        return l.end();
    }

    template<typename T>
    decltype(auto) cbegin(const list<T>& l) noexcept
    {
        return begin(l);
    }

    template<typename T>
    decltype(auto) cend(const list<T>& l) noexcept
    {
        return end(l);
    }

    template<typename T>
    auto size(const list<T>& l) noexcept -> typename list<T>::size_type
    {
        return l.size();
    }

    template<typename T>
    bool operator == (const list<T>& lhs, const list<T>& rhs)
    {
        if (size(lhs) != size(rhs)) return false;
        return std::mismatch(begin(lhs), end(lhs), begin(rhs), end(rhs)) == std::make_pair(end(lhs), end(rhs));
    }

    template<typename T>
    bool operator != (const list<T>& lhs, const list<T>& rhs)
    {
        return !(lhs == rhs);
    }
}

