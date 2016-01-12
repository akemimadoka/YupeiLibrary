#pragma once

#include "SmartPointers\unique_ptr.h"
#include "MemoryReasource\memory_resource.h"
#include "HelperMacros.h"
#include "MemoryInternal.h"
#include "iterator.h"
#include "utility.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename T>
		struct list_node;

		template<typename T>
		struct list_node_base;

		template<typename T>
		struct NodeTraits
		{			
			using NodeBaseType = list_node_base<T>;
			using NodeType = list_node<T>;
			using LinkPointer = NodeBaseType*;
			using NodePointer = NodeType*;

			static auto AsNode(list_node_base<T>* p) noexcept
				-> NodePointer
			{
				return reinterpret_cast<list_node<T>*>(p);
			}

			static auto AsNode(const list_node_base<T>* p) noexcept
				-> NodePointer
			{
				return reinterpret_cast<list_node<T>*>(const_cast<list_node<T>*>(p));
			}

			static auto AsNode(list_node<T>* p) noexcept
				-> NodePointer
			{
				return p;
			}
		};

		template<typename T>
		struct list_node_base
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

			list_node_base() noexcept
				:prev_{ self() },
				next_{ self() }
			{}
		};

		template<typename T>
		struct list_node : public list_node_base<T>
		{
		private:
			using NodePointer = typename NodeTraits<T>::NodePointer;

		public:
			T item_;

			template<typename... Args>
			list_node(Args&&... args)
				:item_(Yupei::forward<Args>(args)...)
			{
			}

			NodePointer AsNode() noexcept
			{
				return NodeTraits<T>::AsNode(this);
			}
		};

	}
	
	template<typename T>
	class list_iterator : public iterator<bidirectional_iterator_tag,T>
	{
		using NodePointer = typename Internal::NodeTraits<T>::NodePointer;
		using BaseType = iterator<bidirectional_iterator_tag, T>;
		using LinkPointer = typename Internal::NodeTraits<T>::LinkPointer;

		LinkPointer cur_;

		template<typename U>
		friend class list;

		template<typename U>
		friend class list_const_iterator;

		explicit list_iterator(LinkPointer p) noexcept
			:cur_{p}
		{}

	public:
		constexpr list_iterator() noexcept
			:cur_{}
		{}

		reference_t<BaseType> operator*() const noexcept
		{ 
			return Internal::NodeTraits<T>::AsNode(cur_)->item_; 
		}

		pointer_t<BaseType> operator->() const noexcept
		{ 
			return Yupei::addressof(Internal::NodeTraits<T>::AsNode(cur_)->item_); 
		}

		list_iterator& operator++() noexcept
		{
			cur_ = cur_->next_;
			return *this;
		}

		list_iterator operator++(int) noexcept
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		list_iterator& operator--() noexcept
		{
			cur_ = cur_->prev_;
			return *this;
		}

		list_iterator operator--(int) noexcept
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		bool operator==(const list_iterator& rhs) const noexcept
		{
			return cur_ == rhs.cur_;
		}

		bool operator!=(const list_iterator& rhs) const noexcept
		{
			return cur_ != rhs.cur_;
		}
	};

	template<typename T>
	class list_const_iterator : public iterator<bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
	{
		using NodePointer = Internal::list_node<T>*;
		using BaseType = iterator<bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>;
		using LinkPointer = typename Internal::NodeTraits<T>::LinkPointer;
		LinkPointer cur_;

		template<typename T>
		friend class list;

		list_const_iterator(LinkPointer p) noexcept
			:cur_{ p }
		{}

		constexpr list_const_iterator() noexcept
			: cur_{}
		{}

		explicit list_const_iterator(list_iterator<T> it) noexcept
			:cur_{it.cur_}
		{}
	public:
		reference_t<BaseType> operator*() const noexcept
		{ 
			return Internal::NodeTraits<T>::AsNode(cur_)->item_; 
		}

		pointer_t<BaseType> operator->() const noexcept
		{ 
			return Yupei::addressof(Internal::NodeTraits<T>::AsNode(cur_)->item_);
		}

		list_const_iterator& operator++() noexcept
		{
			cur_ = cur_->next_;
			return *this;
		}

		list_const_iterator operator++(int) noexcept
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		list_const_iterator& operator--() noexcept
		{
			cur_ = cur_->prev_;
			return *this;
		}

		list_const_iterator operator--(int) noexcept
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		bool operator==(const list_const_iterator& rhs) const noexcept
		{
			return cur_ == rhs.cur_;
		}

		bool operator!=(const list_const_iterator& rhs) const noexcept
		{
			return cur_ != rhs.cur_;
		}
	};

	template<typename T>
	class list : private polymorphic_allocator<Internal::list_node<T>>
	{

	public:
		CONTAINER_DEFINE(T)
		using iterator = list_iterator<T>;
		using const_iterator = list_const_iterator<T>;
		using allocator_type = polymorphic_allocator<Internal::list_node<T>>;

	private:
		using NodeType = typename Internal::NodeTraits<T>::NodeType;
		using NodeBaseType = typename Internal::NodeTraits<T>::NodeBaseType;
		using LinkPointer = typename Internal::NodeTraits<T>::LinkPointer;
		using NodePointer = NodeType*;

	public:

		list() noexcept
			:count_{}
		{}

		list(memory_resource_ptr pmr)
			:count{},
			allocator_type{pmr}
		{}

		explicit list(size_type n)
		{
			for (; n != 0 ; --n)
				emplace_back();
		}

		list(const list& other)
			:list()
		{
			insert(cend(), other.begin(), other.end());
		}

		list(list&& other) noexcept
			:head_{ other.head_ },
			count_{ other.count_ }
		{
			other.head_ = {};
			other.count_ = {};
		}

		list& operator=(list&& other) noexcept
		{
			list(Yupei::move(other)).swap(*this);
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

		//TODO: if(count == 0) throw xxx;
		//CANNOT BE NOEXCEPT!
		reference front()
		{
			if (count_ != 0)
				return head_.next_->AsNode()->item_;
		}

		const_reference front() const
		{
			if (count_ != 0)
				return head_.next_->AsNode()->item_;
		}

		reference back()
		{
			if (count_ != 0)
				return head_.prev_->AsNode()->item_;
		}

		const_reference back() const
		{
			if (count_ != 0)
				return head_.prev_->AsNode()->item_;
		}

		void swap(list& other) noexcept
		{
			Yupei::swap(head_.next_, other.head_.next_);
			Yupei::swap(head_.prev_, other.head_.prev_);
			Yupei::swap(count_, other.count_);

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
			auto deleteFn = [this](NodePointer p) {this->deallocate(p, 1);};
			unique_ptr<NodeType, decltype(deleteFn)> holder{ this->allocate(1),deleteFn };
			auto node = holder.get();
			Yupei::construct(Yupei::addressof(node->item_), Yupei::forward<Args>(args)...);
			return holder;
		}

	public:
		void push_back(const value_type& v)
		{
			insert(cend(), v);			
		}

		void push_back(value_type&& v)
		{
			insert(cend(), Yupei::move(v));
		}

		void push_front(const value_type& v)
		{
			insert(cbegin(), v);
		}

		void push_front(value_type&& v)
		{
			insert(cbegin(), Yupei::move(v));
		}

		template<typename... Args>
		void emplace_back(Args&&... args)
		{
			auto node = MakeNode(Yupei::forward<Args>(args)...);
			InsertInterval(head_.prev_, node.get(), node.get());
			node.release();
			++count_;
		}

		void pop_back()
		{
			if (count_ != 0)
			{
				DeallocateNode(head_.prev_);
				--count_;
			}
		}

		void pop_front()
		{
			if (count_ != 0)
			{
				DeallocateNode(head_.next_);
				--count_;
			}
		}

		iterator insert(const_iterator position, const value_type& v)
		{
			return Insert(position, v);
		}

		iterator insert(const_iterator position, value_type&& v)
		{
			return Insert(position, Yupei::move(v));
		}

		template<typename InputItT>
		iterator insert(const_iterator position, InputItT first, InputItT last)
		{
			if (first == last) return iterator{ position.cur_ };
			auto firstNode = MakeNode(*first).release()->AsNode();
			InsertInterval(position.cur_, firstNode, firstNode);
			auto it = iterator{ firstNode };
			for (;first != last;++first)
				insert(position, *first);
			return it;
		}

		iterator erase(const_iterator position)
		{
			auto cur = position.cur_;
			UnlinkNode(cur,cur);
			auto ret = cur->next_;
			DeallocateNode(cur);
			--count_;
			return iterator{ ret };
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
				return iterator{ lastNode };
			}	
			return iterator{ position.cur_ };
		}

		void clear() noexcept
		{
			erase(cbegin(), cend());
		}

		iterator begin() noexcept
		{
			return iterator{ head_.next_ };
		}

		const_iterator begin() const noexcept
		{
			return const_iterator{ head_.next_ };
		}

		iterator end() noexcept
		{
			return iterator{ head_.self() };
		}

		const_iterator end() const noexcept
		{
			return const_iterator{ head_.self() };
		}

		const_iterator cbegin() const noexcept
		{
			return const_iterator{ head_.next_ };
		}

		const_iterator cend() const noexcept
		{
			return const_iterator{ head_.self() };
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
			auto n = Yupei::distance(first, last);
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
				Yupei::swap(cur->next_, cur->prev_);
				cur = next;
			}
			Yupei::swap(head_.prev_, head_.next_);
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
		Internal::list_node_base<T> head_;
		size_type count_;

		void DeallocateNode(NodePointer node)
		{
			Yupei::destroy(Yupei::addressof(node->item_));
			this->deallocate(node, 1);
		}

		//insert before pos
		template<typename ...Args>
		iterator Insert(const_iterator position, Args&&... args)
		{
			auto node = MakeNode(Yupei::forward<Args>(args)...);
			auto cur = position.cur_;
			auto prev = cur->prev_;
			node->next_ = cur;
			cur->prev_ = node.get();
			prev->next_ = node.get();
			node->prev_ = prev;
			++count_;
			return iterator{ node.release() };
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
}

