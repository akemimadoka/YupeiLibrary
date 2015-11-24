#pragma once

#include "default_delete.h"
#include "..\compare_funtor.h"
#include "..\tuple.h"
#include "..\type_traits.h"
#include "..\pointer_traits.h"
#include <cstddef>

namespace Yupei
{
	namespace Internal
	{
		template<typename D>
		using DeleterHasPointerOp = typename remove_reference_t<D>::pointer;

		template<typename D>
		struct SignatureForUniquePtr
		{
			using type1 = const D&;
			using type2 = D&&;
		};

		template<typename D>
		struct SignatureForUniquePtr<D&>
		{
			using type1 = D&;
			using type2 = D&&;
		};

		template<typename D>
		struct SignatureForUniquePtr<const D&>
		{
			using type1 = const D&;
			using type2 = const D&&;
		};
	}


	template <typename ObjectT,typename DeleterT = default_delete<ObjectT>>
	class unique_ptr 
	{
	public:

		using element_type = ObjectT;
		using deleter_type = DeleterT;

		using pointer = deteced_or_t<element_type*, Internal::DeleterHasPointerOp, element_type>;

		constexpr unique_ptr() noexcept = default;

		explicit unique_ptr(pointer p) noexcept
			: data_(p,deleter_type())
		{
		}

		//if D is a ref(& or const &),just add &&
		//else const & & &&

		using RefType = conditional_t<is_reference<deleter_type>::value,deleter_type,const deleter_type&>;
		unique_ptr(pointer p,RefType d) noexcept
			:data_(p, d)
		{
		}

		unique_ptr(pointer p, add_rvalue_reference_t<remove_reference_t<deleter_type>> d) noexcept
			: data_(p,Yupei::move(d))
		{
		}

		unique_ptr(unique_ptr&& u) noexcept
			: data_(u.release(),Yupei::forward<deleter_type>(u.get_deleter()))
		{
		}

		constexpr unique_ptr(std::nullptr_t) noexcept
			: unique_ptr()
		{
		}

		//Remarks: This constructor shall not participate in overload resolution unless:
		//(19.1) ！ unique_ptr<U, E>::pointer is implicitly convertible to pointer,
		//(19.2) ！ U is not an array type, and
		//(19.3) ！ either D is a reference type and E is the same type as D, or D is not a reference type and E is
		//implicitly convertible to D.
		//now we start
		template<class U,class E,
			typename = enable_if_t<is_convertible<typename unique_ptr<U, E>::pointer, pointer>::value && 
			!is_array<U>::value &&
			conditional_t<is_reference<deleter_type>::value,is_same<E, deleter_type>,is_convertible<E, deleter_type>>::value>>
		unique_ptr(unique_ptr<U, E>&& u) noexcept
			:data_(u.release(),Yupei::forward<E>(u.get_deleter()))
		{
		}

		~unique_ptr()
		{
			if (get() != pointer())
				get_deleter()(get());
		}

		unique_ptr& operator=(unique_ptr&& u) noexcept
		{
			if (this != &u)
			{
				reset(u.release());
				get_deleter() = Yupei::forward<deleter_type>(u.get_deleter());
			}
			return *this;
		}

		//(5.1) ！ unique_ptr<U, E>::pointer is implicitly convertible to pointer, and
		//(5.2) ！ U is not an array type, and
		//(5.3) ！ is_assignable<D&, E&&>::value is true.

		template <typename U, typename E,
			typename = enable_if_t<is_convertible<typename unique_ptr<U, E>::pointer,pointer>::value && 
			!is_array<U>::value && 
			is_assignable<deleter_type&,E&&>::value>>
		unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept
		{
			reset(u.release());
			get_deleter() = Yupei::forward<E>(u.get_deleter());
			return *this;
		}

		unique_ptr& operator=(std::nullptr_t) noexcept
		{
			unique_ptr().swap(*this);
			return *this;
		}

		add_lvalue_reference_t<element_type> operator*() const
		{
			return *get();
		}

		pointer operator->() const noexcept
		{
			return get();
		}

		pointer get() const noexcept
		{
			return Yupei::get<0>(data_);
		}

		deleter_type& get_deleter() noexcept
		{
			return Yupei::get<1>(data_);
		}

		const deleter_type& get_deleter() const noexcept
		{
			return Yupei::get<1>(data_);
		}

		explicit operator bool() const noexcept
		{
			return get() != pointer();
		}

		pointer release() noexcept
		{
			auto old_p = get();
			Yupei::get<0>(data_) = {};
			return old_p;
		}

		void reset(pointer p = pointer()) noexcept
		{
			auto old_p = get();
			Yupei::get<0>(data_) = p;
			if (old_p != pointer())
			{
				get_deleter()(old_p);
			}
		}

		void swap(unique_ptr& u) noexcept
		{
			Yupei::swap(data_, u.data_);
		}

		// disable copy from lvalue
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;

	private:
		tuple<pointer, DeleterT> data_;

	};

	template <class T,class DeleterT>
	class unique_ptr<T[], DeleterT>
	{
	public:

		using element_type = T;
		using deleter_type = DeleterT;

		using pointer = deteced_or_t<element_type*, Internal::DeleterHasPointerOp, element_type>;
		
		constexpr unique_ptr() noexcept = default;

		template<typename U>
		using CouldBeConstructed = enable_if<
				is_same<U, pointer>::value || 
				(is_same<element_type*, pointer>::value && 
				is_convertible<typename pointer_traits<U>::element_type(*)[],element_type(*)[]>::value)>;

		template <typename U,typename = typename CouldBeConstructed<U>::type>
		explicit unique_ptr(U p) noexcept
			:data_(p,deleter_type())
		{
		}

		//if D is a ref(& or const &),just add &&
		//else const & & &&
		using RefType = conditional_t<is_reference<deleter_type>::value,deleter_type,const deleter_type&>;

		template <typename U,typename = typename CouldBeConstructed<U>::type>
		unique_ptr(U p, RefType d) noexcept
			:data_(p,d)
		{
		}

		template <typename U,typename = typename CouldBeConstructed<U>::type>
		unique_ptr(U p, add_rvalue_reference_t<remove_reference_t<DeleterT>> d2) noexcept
			: data_(p,Yupei::move(d2))
		{
		}

		unique_ptr(unique_ptr&& u) noexcept
			: data_(u.release(),Yupei::forward<deleter_type>(u.get_deleter()))
		{
		}

		constexpr unique_ptr(std::nullptr_t) noexcept
			: unique_ptr()
		{
		}

		//(2.1) ！ U is an array type, and
		//(2.2) ！ pointer is the same type as element_type*, and
		//(2.3) ！ UP::pointer is the same type as UP::element_type*, and
		//(2.4) ！ UP::element_type(*)[] is convertible to element_type(*)[], and
		//(2.5) ！ either D is a reference type and E is the same type as D, or D is not a reference type and E is
		//implicitly convertible to D.

		template <class U, class E>
		struct CouldBeMoved : 
			bool_constant<is_array<U>::value &&
				is_same<pointer,element_type*>::value &&
				is_same<typename unique_ptr<U, E>::pointer,typename unique_ptr<U, E>::element_type*>::value &&
				is_convertible<typename unique_ptr<U, E>::element_type(*)[],element_type(*)[]>::value> {};

		template<typename U,typename E,typename = enable_if_t<
			CouldBeMoved<U, E>::value &&	
			conditional_t<is_reference<deleter_type>::value,is_same<E, deleter_type>,is_convertible<E, deleter_type>>::value>>
		unique_ptr(unique_ptr<U, E>&& u) noexcept
			:data_(u.release(),Yupei::forward<E>(u.get_deleter()))
		{
		}

		~unique_ptr()
		{
			if (get() != pointer())
				get_deleter()(get());
		}

		unique_ptr& operator=(unique_ptr&& u) noexcept
		{
			if (this != &u)
				unique_ptr(move(u)).swap(*this);	
			return *this;
		}

		//(5.1) ！ unique_ptr<U, E>::pointer is implicitly convertible to pointer, and
		//(5.2) ！ U is not an array type, and
		//(5.3) ！ is_assignable<D&, E&&>::value is true.
		template<typename U,typename E,
			typename = enable_if_t<CouldBeMoved<U, E>::value &&	is_assignable<DeleterT&, E&&>::value>>
		unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept
		{
			reset(u.release());
			get_deleter() = Yupei::forward<E>(u.get_deleter());
			return *this;
		}

		unique_ptr& operator=(std::nullptr_t) noexcept
		{
			unique_ptr().swap(*this);
			return *this;
		}

		pointer get() const noexcept
		{
			return Yupei::get<0>(data_);
		}

		deleter_type& get_deleter() noexcept
		{
			return Yupei::get<1>(data_);
		}

		const deleter_type& get_deleter() const noexcept
		{
			return Yupei::get<1>(data_);
		}

		explicit operator bool() const noexcept
		{
			return get() != pointer();
		}

		// 20.8.1.2.5 modifiers
		pointer release() noexcept
		{
			auto old_p = get();
			Yupei::get<0>(data_) = pointer();
			return old_p;
		}

		template<class U,typename = typename CouldBeConstructed<U>::type>
		void reset(U p) noexcept
		{
			auto old_p = get();
			Yupei::get<0>(data_) = p;
			if (old_p != pointer())
			{
				get_deleter()(old_p);
			}
		}

		void reset(std::nullptr_t = nullptr)
		{
			reset(pointer());
		}

		T& operator[](size_t i) const
		{
			return get()[i];
		}

		void swap(unique_ptr& u) noexcept
		{
			Yupei::swap(data_, u.data_);
		}

		// disable copy from lvalue
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;

	private:
		tuple<pointer, DeleterT> data_;
	};

	namespace Internal
	{
		template<typename T>
		struct IsIncompleteArrayType : false_type {};

		template<typename T>
		struct IsIncompleteArrayType<T[]> : true_type {};
	}

	template<typename T,typename... ParamsT,typename = enable_if_t<!is_array<T>::value>>
	inline unique_ptr<T> make_unique(ParamsT&&... params)
	{
		return unique_ptr<T>(new T(Yupei::forward<ParamsT>(params)...));
	}

	template<typename T,typename = enable_if_t<Internal::IsIncompleteArrayType<T>::value>>
	inline unique_ptr<T> make_unique(std::size_t n)
	{
		return unique_ptr<T>(new remove_extent_t<T>[n]());
	}

	template <typename T, typename... Args>
	enable_if_t<extent<T>::value != 0>
		make_unique(Args&&...) = delete;

	template<class T, class D>
	inline void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) noexcept
	{
		x.swap(y);
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return x.get() == y.get();
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return x.get() != y.get();
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return less<common_type_t<typename unique_ptr<T1, D1>::pointer,typename unique_ptr<T2, D2>::pointer>>()(x, y);
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return !(y < x);
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return y < x;
	}

	template<class T1, class D1, class T2, class D2>
	inline bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return !(x < y);
	}

	template <class T, class D>
	inline bool operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept
	{
		return !x;
	}

	template <class T, class D>
	inline bool operator==(std::nullptr_t, const unique_ptr<T, D>& y) noexcept
	{
		return !y;
	}

	template <class T, class D>
	inline bool operator!=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept
	{
		return static_cast<bool>(x);
	}

	template <class T, class D>
	inline bool operator!=(std::nullptr_t, const unique_ptr<T, D>& y) noexcept
	{
		return static_cast<bool>(y);
	}

	template <class T, class D>
	inline bool operator<(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return less<typename unique_ptr<T, D>::pointer>()(x.get(), nullptr);
	}

	template <class T, class D>
	inline bool operator<(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return less<typename unique_ptr<T, D>::pointer>()(nullptr, y.get());
	}

	template <class T, class D>
	inline bool operator<=(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return !(nullptr < x);
	}

	template <class T, class D>
	inline bool operator<=(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return !(y < nullptr);
	}

	template <class T, class D>
	inline bool operator>(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return nullptr < x;
	}

	template <class T, class D>
	inline bool operator>(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return y < nullptr;
	}

	template <class T, class D>
	inline bool operator>=(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return !(x < nullptr);
	}

	template <class T, class D>
	inline bool operator>=(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return !(nullptr < y);
	}
}
