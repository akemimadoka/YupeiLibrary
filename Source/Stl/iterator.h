#pragma once

#include <cstddef>
#include <iosfwd>
#include <iterator> // to be compatible with tags in std
#include "traits_internal.h"
#include "__addressof.h"
#include "container_traits.h"

namespace Yupei
{
	using std::input_iterator_tag;
	using std::output_iterator_tag;
	using std::forward_iterator_tag;
	using std::random_access_iterator_tag;
	using std::bidirectional_iterator_tag;

	struct contiguous_iterator_tag : random_access_iterator_tag {};

	

	namespace Internal
	{
		template<typename Iterator,
			typename = void>
		struct value_type
		{
			using type = remove_reference_t<decltype(*declval<Iterator>())>; // no value_type
		};

		template<typename Iterator>
		struct value_type<Iterator,
			void_t<typename Iterator::value_type>
		>
		{
			using type = typename Iterator::value_type; //I have a value_type
		};
	}

	template<typename Iterator>
	using ValueType = typename Internal::value_type<Iterator>::type;

	namespace Internal
	{
		template<typename Iterator,
			typename = void>
		struct reference_type
		{
			using type = decltype(*declval<Iterator>()); // no reference, use operator*
		};

		template<typename Iterator>
		struct reference_type<Iterator,
			void_t<typename Iterator::reference>
		>
		{
			using type = typename Iterator::reference; //I have a value_type
		};
	}

	template<typename Iterator>
	using ReferenceType = typename Internal::reference_type<Iterator>::type;

	namespace Internal
	{
		template <class, class = void> 
		struct difference_type { };
		template <class T> 
		struct difference_type<T*>
		{
			using type = ptrdiff_t;
		};
		template <>
		struct difference_type<nullptr_t>
		{
			using type = ptrdiff_t;
		};
		template <class T>
		struct difference_type<T[]>
		{
			using type = ptrdiff_t;
		};
		template <class T, std::size_t N>
		struct difference_type<T[N]>
		{
			using type = ptrdiff_t;
		};
		template <class T>
		struct difference_type<T, void_t<typename T::difference_type>>
		{
			using type = typename T::difference_type;
		};
		/*[Editor¡¯s note : REVIEW : The difference_type of unsigned Integral types
		is not large enough to cover
		the entire range.*/
		//yupei's note:why do we need this.......
		/*template <class T>
		struct difference_type<T, enable_if_t<is_integral<T>::value>>
		: make_signed< decltype(declval<T>() - declval<T>()) >
		{
		};*/
	}

	template<typename Iterator>
	using DifferenceType = typename Internal::difference_type<Iterator>::type;

	namespace Internal
	{
		template <typename, typename = void> 
		struct iterator_category { };

		template <typename T> 
		struct iterator_category<T*,void> 
		{
			using type = random_access_iterator_tag;
		};

		template <typename T>
		struct iterator_category<T,void_t<typename T::iterator_category>> 
		{
			using type = typename T::iterator_category;
		};

	}

	template <class T>
	using IteratorCategory = typename Internal::iterator_category<T>::type;

	namespace Internal
	{
		template <class I, class = void> 
		struct pointer_type_impl
		{
			using type = add_pointer_t<ReferenceType<I>>;
		};


		
		template <class I>
		struct pointer_type_impl<I, 
			void_t</*decltype(Yupei::declval<I>().operator->())*/typename I::pointer>
		>
		{
			using type = typename I::pointer;
		};

	}

	template <class T>
	using PointerType = typename Internal::pointer_type_impl<T>::type;
	

	namespace Internal
	{
		template<
			typename Iterator,
			bool IsIterator>
		struct iterator_traits_impl;

		template<
			typename Iterator
		>
		struct iterator_traits_impl<Iterator, true>
		{
			using difference_type = DifferenceType<Iterator>;
			using value_type = ValueType<Iterator>;
			using pointer = PointerType<Iterator>;
			using reference = ReferenceType<Iterator>;
			using iterator_category = IteratorCategory<Iterator>;
		};

		template<
			typename Iterator,
			typename = void
		>
		struct is_iterator : false_type
		{

		};

		template<typename Iterator>
		struct is_iterator<Iterator, 
			void_t<
			//ValueType<Iterator>,// a value_type, a workaround
			IteratorCategory<Iterator> // a iterator category
			>
		> : true_type
		{

		};
	}

	template<class Iterator> 
	using iterator_traits = Internal::iterator_traits_impl<Iterator,
		Internal::is_iterator<Iterator>::value>;

	template<class Category, class T, class Distance = std::ptrdiff_t,
	class Pointer = T*, class Reference = T&>
	struct iterator 
	{
		typedef T value_type;
		typedef Distance difference_type;
		typedef Pointer pointer;
		typedef Reference reference;
		typedef Category iterator_category;
	};

	template<typename IteratorT>
	struct is_contiguous_iterator : is_same<IteratorCategory<IteratorT>, Yupei::contiguous_iterator_tag> {};

	template<typename IteratorT>
	struct realtype_traits
	{
		using type = IteratorT;
	};

	template<typename PtrT>
	struct is_pod_ptr : false_type {};

	template<typename ObjectT>
	struct is_pod_ptr<ObjectT*> : is_pod<ObjectT> {};

	template<bool IsContiguous, typename ItT>
	struct is_pod_iterator_impl : false_type {};
	
	template<typename ItT>
	struct is_pod_iterator_impl<true, ItT> : Yupei::is_pod<ValueType<ItT>> {};

	template<typename ItT>
	using is_pod_iterator = is_pod_iterator_impl<is_contiguous_iterator<ItT>::value, ItT>;


	namespace Internal
	{
		template <class InputIterator, class Distance>
		void advance_impl(InputIterator& i, Distance n,random_access_iterator_tag)
		{
			i += n;
		}

		template <class InputIterator, class Distance>
		void advance_impl(InputIterator& i, Distance n, input_iterator_tag)
		{
			for (;n > 0;--n)
			{
				++i;
			}
		}

		template <class InputIterator, class Distance>
		void advance_impl(InputIterator& i, Distance n, bidirectional_iterator_tag)
		{
			if (n > 0)
			{
				for (;n > 0;--n)
				{
					++i;
				}
			}
			else
			{
				for (;n != 0; ++n)
				{
					--i;
				}
			}
		}
	}

	template <class InputIterator, class Distance>
	void advance(InputIterator& i, Distance n)
	{
		return Internal::advance_impl(i, n, IteratorCategory<InputIterator>{});
	}

	namespace Internal
	{
		template <class InputIterator>
		DifferenceType<InputIterator> distance_impl(InputIterator first, InputIterator last, input_iterator_tag)
		{
			DifferenceType<Iterator> res{};
			for (;first != last;++first)
				++res;
			return res;
		}

		template <class InputIterator>
		DifferenceType<InputIterator> distance_impl(InputIterator first, InputIterator last, random_access_iterator_tag)
		{
			return last - first;
		}
	}
	
	template <class InputIterator>
	DifferenceType<InputIterator> distance(InputIterator first, InputIterator last)
	{
		return Internal::distance_impl(first, last, IteratorCategory<InputIterator>{});
	}
	template <class ForwardIterator>
	ForwardIterator next(ForwardIterator x,
		DifferenceType<ForwardIterator> n = 1)
	{
		Yupei::advance(x, n);
		return x;
	}
	 template <class BidirectionalIterator>
	 BidirectionalIterator prev(BidirectionalIterator x,
		 DifferenceType<BidirectionalIterator> n = 1)
	 {
		 Yupei::advance(x, -n);
		 return x;
	 }

	 template <class Iterator>
	 class reverse_iterator 
	 {
	 public:
		 using iterator_type = Iterator;
		 using iterator_category = IteratorCategory<Iterator>;
		 using value_type = ValueType<Iterator>;
		 using difference_type = DifferenceType<Iterator>;
		 using pointer = PointerType<Iterator>;
		 using reference = ReferenceType<Iterator>;
		 reverse_iterator()
			 :current()
		 {

		 }
		 explicit reverse_iterator(Iterator x)
			 :current(x)
		 {

		 }
		 template <class U> 
		 reverse_iterator(const reverse_iterator<U>& u)
			 :current(u.current)
		 {

		 }
		 template <class U> 
		 reverse_iterator& operator=(const reverse_iterator<U>& u)
		 {
			 current = u.base();
			 return *this;
		 }
		 Iterator base() const // explicit
		 {
			 return current;
		 }
		 reference operator*() const
		 {
			 auto tmp = current;
			 return *--tmp;
		 }
		 pointer operator->() const
		 {
			 return Yupei::addressof(operator*());
		 }
		 reverse_iterator& operator++()
		 {
			 --current;
			 return *this;
		 }
		 reverse_iterator operator++(int)
		 {
			 auto tmp = current;
			 --current;
			 return *tmp;
		 }
		 reverse_iterator& operator--()
		 {
			 ++current;
			 return *this;
		 }
		 reverse_iterator operator--(int)
		 {
			 auto tmp = *this;
			 ++current;
			 return tmp;
		 }
		 reverse_iterator operator+ (difference_type n) const
		 {
			 return reverse_iterator(current - n);
		 }
		 reverse_iterator operator- (difference_type n) const
		 {
			 return reverse_iterator(current + n);
		 }
		 reverse_iterator& operator-=(difference_type n)
		 {
			 current += n;
			 return *this;
		 }
		 reference operator[](difference_type n) const
		 {
			 return current[-n - 1];
		 }
	 protected:
		 Iterator current;
	 };
	 template <class Iterator1, class Iterator2>
	 bool operator==(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y)
	 {
		 return x.current == y.current;
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator<(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y)
	 {
		 return x.current > y.current;
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator!=(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y)
	 {
		 x.current != y.current;
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator>(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y)
	 {
		 return x.current < y.current;
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator>=(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y)
	 {
		 return x.current <= y.current;
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator<=(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y)
	 {
		 x.current >= y.current;
	 }
	 template <class Iterator1, class Iterator2>
	 auto operator-(
		 const reverse_iterator<Iterator1>& x,
		 const reverse_iterator<Iterator2>& y) -> decltype(y.base() - x.base())
	 {
		 return y.current - x.current;
	 }
	 template <class Iterator>
	 reverse_iterator<Iterator> operator+(
		 DifferenceType<Iterator> n,
		 const reverse_iterator<Iterator>& x)
	 {
		 return reverse_iterator<Iterator>(x.current - n);
	 }
	 template <class Iterator>
	 reverse_iterator<Iterator> make_reverse_iterator(Iterator i)
	 {
		 return reverse_iterator<Iterator>(i);
	 }

	 template <class Container>
	 class back_insert_iterator 
	 {
	 protected:
		 Container* container;
	 public:
		 using iterator_category = output_iterator_tag;
		 typedef void value_type;
		 typedef void difference_type;
		 typedef void pointer;
		 typedef void reference;
		 typedef Container container_type;
		 explicit back_insert_iterator(Container& x)
			 :container(Yupei::addressof(x))
		 {

		 }
		 back_insert_iterator& operator=(const container_value_type<Container>& value)
		 {
			 container->push_back(value);
			 return *this;
		 }
		 back_insert_iterator& operator=(container_value_type<Container>&& value)
		 {
			 container->push_back(Yupei::move(value));
			 return *this;
		 }
		 back_insert_iterator& operator*()
		 {
			 return *this;
		 }
		 back_insert_iterator& operator++()
		 {
			 return *this;
		 }
		 back_insert_iterator operator++(int)
		 {
			 return *this;
		 }
	 };
	 template <class Container>
	 back_insert_iterator<Container> back_inserter(Container& x)
	 {
		 return back_insert_iterator<Container>(x);
	 }

	 template <class Container>
	 class front_insert_iterator {
	 protected:
		 Container* container;
	 public:
		 typedef output_iterator_tag iterator_category;
		 typedef void value_type;
		 typedef void difference_type;
		 typedef void pointer;
		 typedef void reference;
		 typedef Container container_type;
		 explicit front_insert_iterator(Container& x)
			 :container(Yupei::addressof(x))
		 {

		 }
		 front_insert_iterator& operator=(const container_value_type<Container>& value)
		 {
			 container->push_front(value);
		 }
		 front_insert_iterator& operator=(container_value_type<Container>&& value)
		 {
			 container->push_front(Yupei::move(value));
		 }
		 front_insert_iterator& operator*()
		 {
			 return *this;
		 }
		 front_insert_iterator& operator++()
		 {
			 return *this;
		 }
		 front_insert_iterator operator++(int)
		 {
			 return *this;
		 }
	 };

	 template <class Container>
	 class insert_iterator {
	 protected:
		 Container* container;
		 typename Container::iterator iter;
	 public:
		 using iterator_category = output_iterator_tag;
		 using value_type = void;
		 using difference_type = void;
		 using pointer = void;
		 using reference = void;
		 using container_type = Container;
		 insert_iterator(Container& x, container_iterator<Container> i)
			 :container(Yupei::addressof(x)),
			 iter(i)
		 {

		 }
		 insert_iterator& operator=(const container_value_type<Container>& value)
		 {
			 iter = container->insert(iter, value);
			 ++iter;
			 return *this;
		 }
		 insert_iterator& operator=(container_value_type<Container>&& value)
		 {
			 iter = container->insert(iter, Yupei::move(value));
			 ++iter;
			 return *this;
		 }
		 insert_iterator& operator*()
		 {
			 return *this;
		 }
		 insert_iterator& operator++()
		 {
			 return *this;
		 }
		 insert_iterator& operator++(int)
		 {
			 return *this;
		 }
	 };
	 template <class Container>
	 insert_iterator<Container> inserter(Container& x, container_iterator<Container> i)
	 {
		 return insert_iterator<Container>(x, i);
	 }

	 template <class Iterator>
	 class move_iterator {
	 public:
		 using iterator_type = Iterator;
		 using difference_type = DifferenceType<Iterator>;
		 using value_type = ValueType<Iterator>;
		 using iterator_category = IteratorCategory<Iterator>;
		 using reference = ValueType<Iterator>&&;

		 move_iterator()
			 :current()
		 {

		 }
		 explicit move_iterator(Iterator i)
			 :current(i)
		 {

		 }
		 template <class U> 
		 move_iterator(const move_iterator<U>& u)
			 :current(u.base())
		 {

		 }
		 template <class U> 
		 move_iterator& operator=(const move_iterator<U>& u)
		 {
			 current = u.base();
			 return *this;
		 }
		 iterator_type base() const
		 {
			 return current;
		 }
		 reference operator*() const
		 {
			 return static_cast<reference>(*current);
		 }
		 Iterator operator->() const
		 {
			 return current;
		 }
		 move_iterator& operator++()
		 {
			 ++current;
			 return *this;
		 }
		 move_iterator operator++(int)
		 {
			 auto tmp = *this;
			 ++current;
			 return tmp;
		 }
		 move_iterator& operator--()
		 {
			 --current;
			 return *this;
		 }
		 move_iterator operator--(int)
		 {
			 auto tmp = *this;
			 --current;
			 return tmp;
		 }
		 move_iterator operator+(difference_type n) const
		 {
			 return move_iterator(current + n);
		 }
		 move_iterator& operator+=(difference_type n)
		 {
			 current += n;
			 return *this;
		 }
		 move_iterator operator-(difference_type n) const
		 {
			 return move_iterator(current - n);
		 }
		 move_iterator& operator-=(difference_type n)
		 {
			 current -= n;
			 return *this;
		 }
		 reference operator[](difference_type n) const
		 {
			 return Yupei::move(current[n]);
		 }
	 private:
		 Iterator current; // exposition only
	 };
	 template <class Iterator1, class Iterator2>
	 bool operator==(
		 const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
	 {
		 return x.base() == y.base();
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator!=(
		 const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
	 {
		 !(x == y);
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator<(
		 const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
	 {
		 return x.base() < y.base();
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator<=(
		 const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
	 {
		 return !(y < x);
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator>(
		 const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
	 {
		 return y < x;
	 }
	 template <class Iterator1, class Iterator2>
	 bool operator>=(
		 const move_iterator<Iterator1>& x, const move_iterator<Iterator2>& y)
	 {
		 return !(x < y);
	 }
	 template <class Iterator1, class Iterator2>
	 auto operator-(
		 const move_iterator<Iterator1>& x,
		 const move_iterator<Iterator2>& y) -> decltype(x.base() - y.base())
	 {
		 return x.base() - y.base();
	 }
	 template <class Iterator>
	 move_iterator<Iterator> operator+(
		 typename move_iterator<Iterator>::difference_type n, const move_iterator<Iterator>& x)
	 {
		 return x + n;
	 }
	 template <class Iterator>
	 move_iterator<Iterator> make_move_iterator(Iterator i)
	 {
		 return move_iterator<Iterator>(i);
	 }

	 template <class T, class charT = char, class traits = std::char_traits<charT>,
	 class Distance = std::ptrdiff_t>
	 class istream_iterator : public iterator<
		 input_iterator_tag,
		 T,
		 Distance,
		 const T*,
		 const T&>
	 {
	 public:
		 typedef charT char_type;
		 typedef traits traits_type;
		 typedef std::basic_istream<charT, traits> istream_type;
		 constexpr istream_iterator()
			 :in_stream(nullptr),
			 value()
		 {

		 }
		 istream_iterator(istream_type& s)
			 :in_stream(&s)
		 {
			 if (!(*in_stream >> value))
				 in_stream = 0;
		 }
		 istream_iterator(const istream_iterator& x) = default;
		 ~istream_iterator() = default;
		 const T& operator*() const
		 {
			 return value;
		 }
		 const T* operator->() const
		 {
			 return &(operator*());
		 }
		 istream_iterator& operator++()
		 {
			 if (!(*in_stream >> value))
				 in_stream = 0;
			 return *this;
		 }
		 istream_iterator operator++(int)
		 {
			 auto tmp = *this;
			 ++*this;
			 return tmp;
		 }
		 template <class T, class charT, class traits, class Distance>
		 friend bool operator==(const istream_iterator<T, charT, traits, Distance>& x,
			 const istream_iterator<T, charT, traits, Distance>& y)
		 {
			 return x.in_stream == y.in_stream;
		 }
		 template <class T, class charT, class traits, class Distance>
		 friend  bool operator!=(const istream_iterator<T, charT, traits, Distance>& x,
			 const istream_iterator<T, charT, traits, Distance>& y)
		 {
			 return !(x == y);
		 }
	 private:
		 std::basic_istream<charT, traits>* in_stream; // exposition only
		 T value; // exposition only
	 };

	 template <class T, class charT = char, class traits = std::char_traits<charT> >
	 class ostream_iterator {
	 public:
		 typedef output_iterator_tag iterator_category;
		 typedef void value_type;
		 typedef void difference_type;
		 typedef void pointer;
		 typedef void reference;
		 typedef charT char_type;
		 typedef traits traits_type;
		 typedef std::basic_ostream<charT, traits> ostream_type;
		 ostream_iterator(ostream_type& s)
			 :out_stream(&s),
			 delim(nullptr)
		 {

		 }
		 ostream_iterator(ostream_type& s, const charT* delimiter)
			 :out_stream(&s),
			 delim(delimiter)
		 {

		 }
		 ostream_iterator(const ostream_iterator& x) = default;
		 ~ostream_iterator() = default;
		 ostream_iterator& operator=(const T& value)
		 {
			 *out_stream << value;
			 if (delim != 0)
				 *out_stream << delim;
			 return *this;
		 }
		 ostream_iterator& operator*()
		 {
			 return *this;
		 }
		 ostream_iterator& operator++()
		 {
			 return *this;
		 }
		 ostream_iterator& operator++(int)
		 {
			 return *this;
		 }
	 private:
		 std::basic_ostream<charT, traits>* out_stream; // exposition only
		 const charT* delim; // exposition only
	 };


	 template<typename ObjectT>
	 class temp_iterator : public Yupei::iterator<output_iterator_tag, void, void, void, void>
	 {
		 using MyType = temp_iterator<ObjectT>;
	 public:
		 using pointer = ObjectT*;

		 temp_iterator(std::ptrdiff_t count = 0) noexcept
		 {
			 buf_.begin_ = {};
			 buf_.cur_ = {};
			 buf_.bufSize_ = count;
			 buf_.unintializePos_ = {};
			 pbuf_ = &buf_;
		 }

		 ~temp_iterator() noexcept
		 {
			 if (buf_.begin_ != pointer{})
			 {
				 for (auto i = buf_.begin_;i < buf_.unintializePos_;++i)
					 i->~ObjectT();
				 ::operator delete(buf_.begin_);
			 }
		 }

		 temp_iterator(const MyType& rhs) noexcept
		 {
			 buf_ = {};
			 pbuf_ = rhs.pbuf_;
		 }

		 MyType& operator = (const MyType& rhs) noexcept
		 {
			 pbuf_ = rhs.pbuf_;
			 return *this;
		 }

		 MyType& operator=(const ObjectT& v)
		 {
			 assert(pbuf_->cur_ - pbuf_->begin_ <= pbuf_->bufSize_);
			 if (pbuf_->cur_ < pbuf_->unintializePos_)
				 *pbuf_->cur_++ = v;
			 else
			 {
				 new (pbuf_->cur_) ObjectT(v);
				 pbuf_->unintializePos_ = ++pbuf_->cur_;
			 }
			 return *this;
		 }

		 MyType& operator=(ObjectT&& v) noexcept
		 {
			 assert(pbuf_->cur_ - pbuf_->begin_ <= pbuf_->bufSize_);
			 if (pbuf_->cur_ < pbuf_->unintializePos_)
				 *pbuf_->cur_++ = Yupei::move(v);
			 else
			 {
				 new (pbuf_->cur_) ObjectT(Yupei::move(v));
				 pbuf_->unintializePos_ = ++pbuf_->cur_;
			 }
			 return *this;
		 }

		 MyType& operator*() noexcept
		 {
			 return *this;
		 }

		 MyType& operator++() noexcept
		 {
			 return *this;
		 }

		 MyType& operator++(int) noexcept
		 {
			 return *this;
		 }

		 MyType& init() noexcept
		 {
			 pbuf_->cur_ = pbuf_->begin_;
			 return *this;
		 }

		 pointer begin() const noexcept
		 {
			 return  pbuf_->begin_;
		 }

		 pointer end() const noexcept
		 {
			 return pbuf_->cur_;
		 }

		 void allocate()
		 {
			 if (pbuf_->begin_ == pointer{} && pbuf_->bufSize_ > 0)
			 {
				 auto ptr = static_cast<pointer>(::operator new(pbuf_->bufSize_ * sizeof(ObjectT)));
				 pbuf_->begin_ = ptr;
				 pbuf_->cur_ = ptr;
				 pbuf_->unintializePos_ = ptr;
			 }
		 }

		 std::ptrdiff_t size() const
		 {
			 return pbuf_->bufSize_;
		 }

	 private:
		 struct BufType
		 {
			 pointer begin_;
			 std::ptrdiff_t bufSize_;
			 pointer unintializePos_;
			 pointer cur_;
		 };
		
		 BufType buf_;
		 BufType* pbuf_;
		
	 };
	 
}
