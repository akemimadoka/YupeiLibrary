#pragma once


#include "traits_internal.h"
#include "__addressof.h"

#include <cstddef>
#include <iterator>
#include <cassert>

namespace Yupei
{
	using std::input_iterator_tag;
	using std::output_iterator_tag;
	using std::forward_iterator_tag;
	using std::random_access_iterator_tag;
	using std::bidirectional_iterator_tag;

	//TODO: SFINAE-friendly.

	

	namespace Internal
	{
		template<typename T,typename = void>
		struct IteratorTraitsImp
		{
		};//nothing

		template<typename T>
		struct IteratorTraitsImp<T,
			void_t<
				value_type_t<T>, 
				reference_t<T>, 
				pointer_t<T>, 
				difference_t<T>, 
				iterator_category_t<T>
			>>
		{
			using value_type = value_type_t<T>;
			using reference = reference_t<T>;
			using pointer = pointer_t<T>;
			using difference_type = difference_t<T>;
			using iterator_category = iterator_category_t<T>;
		};
	}

	template<class IteratorT> 
	struct iterator_traits : Internal::IteratorTraitsImp<IteratorT> {};

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


	template<typename PtrT>
	struct is_pod_ptr : false_type {};

	template<typename ObjectT>
	struct is_pod_ptr<ObjectT*> : is_pod<remove_cv_t<ObjectT>> {};

    //http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n4183.pdf
    template<typename T>
    constexpr auto do_pointer_from(T* ptr) noexcept -> T*
    {
        return ptr;
    }

    template<typename ContiguousIterator>
    constexpr auto pointer_from(ContiguousIterator i)
        noexcept(noexcept(do_pointer_from(i)))
        -> decltype(do_pointer_from(i)) // necessary for SFINAE
    {
        return do_pointer_from(i); //ADL
    }

    namespace Internal
    {
        template<typename IteratorT,typename = void>
        struct IsContiguousIterator : false_type {};

        template<typename IteratorT>
        struct IsContiguousIterator<IteratorT, void_t<decltype(Yupei::pointer_from(Yupei::declval<IteratorT>()))>> : true_type {};
    }

    template<typename IteratorT>
    using is_contiguous_iterator = Internal::IsContiguousIterator<IteratorT>;

	using std::distance;

	using std::advance;

	using std::next;

	using std::prev;

	using std::make_reverse_iterator;

	using std::reverse_iterator;

	using std::back_inserter;

	using std::back_insert_iterator;

	using std::front_insert_iterator;

	using std::front_inserter;

	using std::insert_iterator;

	using std::inserter;

	using std::move_iterator;

	using std::istream_iterator;

	using std::ostream_iterator;


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

		 std::ptrdiff_t size() const noexcept
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
