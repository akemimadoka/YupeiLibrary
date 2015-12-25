#pragma once

#include "SmartPointers\unique_ptr.h"
#include "MemoryReasource\memory_resource.h"
#include "HelperMacros.h"
#include "MemoryInternal.h"
#include "iterator.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename U>
		struct list_node;

		template<typename T>
		struct list_node_base
		{
		private:
			using T = int;
			using pointer = list_node<T>*;
			using const_pointer = const list_node<T>*;

		public:
			pointer prev_;
			pointer next_;

			//TODO: fix the reinterpret_cast
			pointer self() noexcept
			{
				return reinterpret_cast<pointer>(this);
			}

			pointer self() const noexcept
			{
				return reinterpret_cast<pointer>(const_cast<list_node_base*>(this));
			}

			list_node_base() noexcept
				:prev_{ self() },
				next_{ self() }
			{}
		};

		template<typename U>
		struct list_node : public list_node_base<U>
		{
		public:
			using U = int;
			U item_;

			template<typename... Args>
			list_node(Args&&... args)
				:item_(Yupei::forward<Args>(args)...)
			{
			}
		};

	}
	
	template<typename T>
	class list_iterator : public iterator<bidirectional_iterator_tag,T>
	{
		using node_pointer = Internal::list_node<T>*;
		using base_type = iterator<bidirectional_iterator_tag, T>;
		node_pointer cur_;

		template<typename U>
		friend class list;

		template<typename U>
		friend class list_const_iterator;

		explicit list_iterator(node_pointer p) noexcept
			:cur_{p}
		{}

		constexpr list_iterator() noexcept
			:cur_{}
		{}
public:
		reference_t<base_type> operator*() const { return cur_->item_; }

		pointer_t<base_type> operator->() const { return &cur_->item_; }

		list_iterator& operator++()
		{
			cur_ = cur_->next_;
			return *this;
		}

		list_iterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		list_iterator& operator--()
		{
			cur_ = cur_->prev_;
			return *this;
		}

		list_iterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		bool operator==(const list_iterator& rhs) noexcept
		{
			return cur_ == rhs.cur_;
		}

		bool operator!=(const list_iterator& rhs) noexcept
		{
			return cur_ != rhs.cur_;
		}
	};

	template<typename T>
	class list_const_iterator : public iterator<bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>
	{
		using node_pointer = Internal::list_node<T>*;
		using base_type = iterator<bidirectional_iterator_tag, T, std::ptrdiff_t, const T*, const T&>;
		node_pointer cur_;

		template<typename T>
		friend class list;

		explicit list_const_iterator(node_pointer p) noexcept
			:cur_{ p }
		{}

		constexpr list_const_iterator() noexcept
			: cur_{}
		{}

		list_const_iterator(list_iterator<T> it)
			:cur_{it.cur_}
		{}
	public:
		reference_t<base_type> operator*() const { return cur_->item_; }

		pointer_t<base_type> operator->() const { return &cur_->item_; }

		list_const_iterator& operator++()
		{
			cur_ = cur_->next_;
			return *this;
		}

		list_const_iterator operator++(int)
		{
			auto tmp = *this;
			++*this;
			return tmp;
		}

		list_const_iterator& operator--()
		{
			cur_ = cur_->prev_;
			return *this;
		}

		list_const_iterator operator--(int)
		{
			auto tmp = *this;
			--*this;
			return tmp;
		}

		bool operator==(const list_const_iterator& rhs) noexcept
		{
			return cur_ == rhs.cur_;
		}

		bool operator!=(const list_const_iterator& rhs) noexcept
		{
			return cur_ != rhs.cur_;
		}
	};

	template<typename T>
	class list : private polymorphic_allocator<Internal::list_node<T>>
	{

	public:
		using T = int;
		CONTAINER_DEFINE(T)
		using iterator = list_iterator<T>;
		using const_iterator = list_const_iterator<T>;
		using allocator_type = polymorphic_allocator<Internal::list_node<T>>;

	private:
		using node_type = Internal::list_node<T>;
		using node_base_type = Internal::list_node_base<T>;
		using node_pointer = node_type*;

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
		{
			insert(end(), other.begin(), other.end());
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

		~list()
		{
			clear();
		}

		//TODO: if(count == 0) throw xxx;
		//CANNOT BE NOEXCEPT!
		reference front()
		{
			if (count_ != 0)
				return head_.next_->item_;
		}

		const_reference front() const
		{
			if (count_ != 0)
				return head_.next_->item_;
		}

		reference back()
		{
			if (count_ != 0)
				return head_.prev_->item_;
		}

		const_reference back() const
		{
			if (count_ != 0)
				return head_.prev_->item_;
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
			auto deleteFn = [this](node_pointer p) {this->deallocate(p, 1);};
			unique_ptr<node_type, decltype(deleteFn)> holder{ this->allocate(1),deleteFn };
			auto node = holder.get();
			Yupei::construct(Yupei::addressof(node->item_), Yupei::forward<Args>(args)...);
			return holder;
		}

	public:
		void push_back(const value_type& v)
		{
			insert(end(), v);			
		}

		void push_back(value_type&& v)
		{
			insert(end(), Yupei::move(v));
		}

		void push_front(const value_type& v)
		{
			insert(begin(), v);
		}

		void push_front(value_type&& v)
		{
			insert(begin(), Yupei::move(v));
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
			for (;first != last;++first)
				insert(position, *first);
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
				auto start = position.cur_;
				auto lastNode = last.cur_;
				UnlinkNode(start, lastNode->prev_);
				size_type n = {};
				while (start != lastNode)
				{
					auto next = start->next_;
					DeallocateNode(start);
					start = next;
					++n;
				}
				count_ -= n;
			}			
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
				auto cur = i.cur_;
				other.UnlinkNode(cur, cur);
				auto pos = position.cur_;
				InsertInterval(pos, cur, cur);
				other.count_ -= 1;
				count_ += 1;
			}			
		}

		void splice(const_iterator position, list& other, const_iterator first, const_iterator last)
		{
			auto firstNode = first.cur_;
			auto lastNode = last.cur_;
			auto n = Yupei::distance(first, last);
			auto prev = lastNode->prev_;
			other.UnlinkNode(firstNode, prev);
			auto pos = position.cur_;
			InsertInterval(pos, firstNode, prev);			
			other.count_ -= n;
			count_ += n;
		}

		void reverse() noexcept
		{
			node_pointer cur = head_.next_;
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
			auto first1 = begin();
			auto last1 = end();
			auto first2 = x.begin();
			auto last2 = x.end();

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

		void DeallocateNode(node_pointer node)
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
		void InsertInterval(node_type* position, node_type* first, node_type* last)
		{
			auto prev = position->prev_;
			position->prev_ = last;
			last->next_ = position;
			prev->next_ = first;
			first->prev_ = prev;
		}

		void UnlinkNode(const node_base_type* first, const node_base_type* last) noexcept
		{			
			auto prev = first->prev_;
			first->prev_->next_ = last->next_;
			last->next_->prev_ = prev;
		}

		
	};
}

