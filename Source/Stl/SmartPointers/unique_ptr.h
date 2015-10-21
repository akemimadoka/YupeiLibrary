#pragma once

#include "default_delete.h"
#include "..\utility_internal.h"
#include "..\compress_pair.h"
#include "..\type_traits.h"

namespace Yupei
{
	namespace Internal
	{
		template<typename E, typename D,
			typename = void>
		struct GetPtrForUniquePtr
		{
			using type = E*;
		};

		template<typename E,
			typename D>
		struct GetPtrForUniquePtr < E, D,
			Yupei::void_t<typename Yupei::remove_reference_t<D>::pointer >>
		{
			using type = typename Yupei::remove_reference_t<D>::pointer;
		};

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

	template<
		typename T,
		typename DeleterT
	>
	struct unique_ptr_base
	{
	public:
		using pointer = typename Internal::GetPtrForUniquePtr<T, DeleterT>::type;
		template<typename U>
		unique_ptr_base(U&& u)
			:internalData(Yupei::compress_value_initialize_first_arg,
				Yupei::forward<U>(u))
		{

		}
		template<typename U,
			typename D>
			unique_ptr_base(U&& u, D&& d)
			:internalData(Yupei::compress_value_initialize_both_args,
				Yupei::forward<D>(d),
				Yupei::forward<U>(u))
		{

		}

		DeleterT& deleter() noexcept
		{
			return internalData.first();
		}

		const DeleterT& deleter() const noexcept
		{
			return internalData.first();
		}

		pointer& raw_pointer() noexcept
		{
			return internalData.second();
		}

		const pointer& raw_pointer() const noexcept
		{
			return internalData.second();
		}

	private:
		compress_pair<DeleterT, pointer> internalData;
	};

	template <class T,
	class DeleterT = Yupei::default_delete<T >>
	class unique_ptr :
		public unique_ptr_base<T, DeleterT>
	{
	public:

		typedef T element_type;
		typedef DeleterT deleter_type;
		using BaseType = unique_ptr_base<T, DeleterT>;
		using pointer = typename BaseType::pointer;
		// 20.8.1.2.1, constructors
		constexpr unique_ptr() noexcept
			:BaseType(pointer())
		{

		}
		explicit unique_ptr(pointer p) noexcept
			: BaseType(p)
		{

		}

		//if D is a ref(& or const &),just add &&
		//else const & & &&

		using _lref_type_for_unique_ptr = conditional_t
			<
			is_reference<deleter_type>::value,
			deleter_type,
			const deleter_type&
			>;
		unique_ptr(pointer p,
			_lref_type_for_unique_ptr d1) noexcept
			:BaseType(p, d1)
		{

		}
		unique_ptr(pointer p, Yupei::add_rvalue_reference_t <
			Yupei::remove_reference_t<DeleterT >> d2) noexcept
			: BaseType(p,
				Yupei::forward<DeleterT>(u.get_deleter()))
		{

		}
		unique_ptr(unique_ptr&& u) noexcept
			: BaseType(u.release(),
				Yupei::forward<DeleterT>(u.get_deleter()))
		{

		}

		constexpr unique_ptr(nullptr_t) noexcept
			: unique_ptr()
		{
		}

		//Remarks: This constructor shall not participate in overload resolution unless:
		//(19.1) ！ unique_ptr<U, E>::pointer is implicitly convertible to pointer,
		//(19.2) ！ U is not an array type, and
		//(19.3) ！ either D is a reference type and E is the same type as D, or D is not a reference type and E is
		//implicitly convertible to D.
		//now we start
		template < class U,
		class E,
			typename = enable_if_t <
			is_convertible<typename unique_ptr<U, E>::pointer, pointer>::value
			&& !is_array<U>::value
			&& conditional_t <
			is_reference<D>::value,
			is_same<E, D>,
			is_convertible<E, D>
			>::value >
		>
			unique_ptr(unique_ptr<U, E>&& u) noexcept
			:BaseType(u.release(), Yupei::forward<E>(u.get_deleter()))
		{

		}
		// 20.8.1.2.2, destructor
		~unique_ptr()
		{
			if (get() != pointer{})
				this->deleter()(this->get());
		}
		// 20.8.1.2.3, assignment

		unique_ptr& operator=(unique_ptr&& u) noexcept
		{
			if (this != &u)
			{
				this->get_deleter() = Yupei::forward<E>(u.get_deleter());
				reset(u.release());
			}
			return *this;
		}
		//(5.1) ！ unique_ptr<U, E>::pointer is implicitly convertible to pointer, and
		//(5.2) ！ U is not an array type, and
		//(5.3) ！ is_assignable<D&, E&&>::value is true.
		template <class U, class E,
			typename =
			enable_if_t
			<
			is_convertible<
			typename unique_ptr<U, E>::pointer,
			pointer
			>::value
			&& !is_array<U>::value
			&& is_assignable
			<
			deleter_type&,
			E&&
			>::value
			>
		>
			unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept
		{
			this->get_deleter() = Yupei::forward<E>(u.get_deleter());
			reset(u.release());
			return *this;
		}
		unique_ptr& operator=(nullptr_t) noexcept
		{
			reset();
		}
		add_lvalue_reference_t<T> operator*() const
		{
			return *get();
		}
		pointer operator->() const noexcept
		{
			return get();
		}
		pointer get() const noexcept
		{
			return this->raw_pointer();
		}
		deleter_type& get_deleter() noexcept
		{
			return this->deleter();
		}
		const deleter_type& get_deleter() const noexcept
		{
			return this->deleter();
		}
		explicit operator bool() const noexcept
		{
			return get() != pointer{};
		}
		// 20.8.1.2.5 modifiers
		pointer release() noexcept
		{
			auto old_p = this->get();
			this->raw_pointer() = pointer{};
			return old_p;
		}
		void reset(pointer p = pointer{}) noexcept
		{
			auto old_p = this->raw_pointer();
			this->raw_pointer() = p;
			if (old_p != pointer{})
			{
				this->get_deleter()(old_p);
			}
		}
		void swap(unique_ptr& u) noexcept
		{
			Yupei::swap(this->raw_pointer(), u.raw_pointer());
			Yupei::swap(this->get_deleter(), u.get_deleter());
		}
		// disable copy from lvalue
		unique_ptr(const unique_ptr&) = delete;
		unique_ptr& operator=(const unique_ptr&) = delete;

	};

	template <class T,
	class DeleterT>
	class unique_ptr<T[], DeleterT> :
		public unique_ptr_base<T, DeleterT>
	{
	public:

		typedef T element_type;
		typedef DeleterT deleter_type;
		using BaseType = unique_ptr_base<T, DeleterT>;
		using pointer = typename BaseType::pointer;
		// 20.8.1.3.1, constructors
		constexpr unique_ptr() noexcept
			:BaseType(pointer())
		{

		}
		template<typename U>
		using _can_be_contructed =
			enable_if
			<
			is_same<U, pointer>::value
			|| (
				is_same<element_type*, pointer>::value
				&& is_convertible<typename Yupei::pointer_traits<U>::element_type(*)[],element_type(*)[]>::value
				)
			>;
		template <typename U,
			typename =typename _can_be_contructed<U>::type>
			explicit unique_ptr(U p) noexcept
			:BaseType(p)
		{

		}


		//if D is a ref(& or const &),just add &&
		//else const & & &&

		using _lref_type_for_unique_ptr =
			conditional_t
			<
			is_reference<deleter_type>::value,
			deleter_type,
			const deleter_type&
			>;
		template <class U,
			typename = typename _can_be_contructed<U >::type>
			unique_ptr(U p, _lref_type_for_unique_ptr d) noexcept
		{

		}
		template <class U,
			typename = typename _can_be_contructed<U >::type>
			unique_ptr(U p, add_rvalue_reference_t <
				remove_reference_t<DeleterT >> d2) noexcept
			: BaseType(p,
				Yupei::forward<DeleterT>(u.get_deleter()))
		{

		}
		unique_ptr(unique_ptr&& u) noexcept
			: BaseType(u.release(),
				Yupei::forward<DeleterT>(u.get_deleter()))
		{

		}

		constexpr unique_ptr(nullptr_t) noexcept
			: unique_ptr()
		{
		}

		//(2.1) ！ U is an array type, and
		//(2.2) ！ pointer is the same type as element_type*, and
		//(2.3) ！ UP::pointer is the same type as UP::element_type*, and
		//(2.4) ！ UP::element_type(*)[] is convertible to element_type(*)[], and
		//(2.5) ！ either D is a reference type and E is the same type as D, or D is not a reference type and E is
		//implicitly convertible to D.
		//now we start
		template <class U, class E>
		using _temp_copy_type = bool_constant
			<
			is_array<U>::value
			&&	is_same
			<
			pointer,
			element_type*
			>::value
			&&	is_same
			<
			typename unique_ptr<U, E>::pointer,
			typename unique_ptr<U, E>::element_type*
			>::value
			&& is_convertible
			<
			typename unique_ptr<U, E>::element_type(*)[],
			element_type(*)[]
			>::value
			>;
		template
			<
			typename U,
			typename E,
			typename = enable_if_t
			<
			_temp_copy_type<U, E>::value
			&&	conditional_t
			<
			is_reference<D>::value,
			is_same<D, E>,
			is_convertible<E, D>
			>::value
			>
			>
			unique_ptr(unique_ptr<U, E>&& u) noexcept
			:BaseType(u.release(),
				Yupei::forward<E>(u.get_deleter()))
		{

		}
		// 20.8.1.2.2, destructor
		~unique_ptr()
		{
			if (get() != pointer{})
				this->deleter()(this->get());
		}
		// 20.8.1.2.3, assignment

		unique_ptr& operator=(unique_ptr&& u) noexcept
		{
			this->get_deleter() = Yupei::forward<E>(u.get_deleter());
			reset(u.release());
		}
		//(5.1) ！ unique_ptr<U, E>::pointer is implicitly convertible to pointer, and
		//(5.2) ！ U is not an array type, and
		//(5.3) ！ is_assignable<D&, E&&>::value is true.
		template
			<
			typename U,
			typename E,
			typename = enable_if_t
			<
			_temp_copy_type<U, E>::value
			&&	is_assignable<DeleterT&, E&&>::value
			>
			>
			unique_ptr& operator=(unique_ptr<U, E>&& u) noexcept
		{
			this->get_deleter() = Yupei::forward<E>(u.get_deleter());
			reset(u.release());
		}
		unique_ptr& operator=(nullptr_t) noexcept
		{
			reset(pointer{});
		}
		pointer get() const noexcept
		{
			return this->raw_pointer();
		}
		deleter_type& get_deleter() noexcept
		{
			return this->deleter();
		}
		const deleter_type& get_deleter() const noexcept
		{
			return this->deleter();
		}
		explicit operator bool() const noexcept
		{
			return get() != pointer{};
		}
		// 20.8.1.2.5 modifiers
		pointer release() noexcept
		{
			auto old_p = this->get();
			this->raw_pointer() = pointer{};
			return old_p;
		}
		template
			<
				class U,
					typename = _can_be_contructed<U>
			>
					void reset(U p) noexcept
				{
					auto old_p = this->raw_pointer();
					this->raw_pointer() = p;
					if (old_p != pointer{})
					{
						this->get_deleter()(old_p);
					}
				}
				void reset(std::nullptr_t = nullptr)
				{
					reset(pointer{});
				}
				T& operator[](size_t i) const
				{
					return get()[i];
				}
				void swap(unique_ptr& u) noexcept
				{
					swap(this->raw_pointer(), u.raw_pointer());
					swap(this->get_deleter(), u.get_deleter());
				}
				// disable copy from lvalue
				unique_ptr(const unique_ptr&) = delete;
				unique_ptr& operator=(const unique_ptr&) = delete;

	};

	namespace Internal
	{
		template<typename T>
		struct is_incomplete_array_type : false_type {};

		template<typename T>
		struct is_incomplete_array_type<T[]> : true_type {};
	}

	template<typename T,
		typename... Args,
		typename = enable_if_t<!is_array<T>::value >>
		unique_ptr<T> make_unique(Args&&... args)
	{
		return unique_ptr<T>(new T(Yupei::forward<Args>(args)...));
	}

	template<typename T,
		typename... Args,
		typename = enable_if_t<Internal::is_incomplete_array_type<T>::value >>
		unique_ptr<T> make_unique(std::size_t n)
	{
		return unique_ptr<T>(new remove_extent_t<T>[n]());
	}

	template <typename T, typename... Args>
	enable_if_t<extent<T>::value != 0>
		make_unique(Args&&...) = delete;

	template<class T, class D>
	void swap(unique_ptr<T, D>& x, unique_ptr<T, D>& y) noexcept
	{
		x.swap(y);
	}
	template<class T1, class D1, class T2, class D2>
	bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return x.get() == y.get();
	}
	template<class T1, class D1, class T2, class D2>
	bool operator!=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return x.get() != y.get();
	}
	template<class T1, class D1, class T2, class D2>
	bool operator<(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return less < common_type_t<
			typename unique_ptr<T1, D1>::pointer,
			typename unique_ptr<T2, D2>::pointer
			>> {}(x, y);
	}
	template<class T1, class D1, class T2, class D2>
	bool operator<=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return !(y < x);
	}
	template<class T1, class D1, class T2, class D2>
	bool operator>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return y < x;
	}
	template<class T1, class D1, class T2, class D2>
	bool operator>=(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
	{
		return !(x < y);
	}
	template <class T, class D>
	bool operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept
	{
		return !x;
	}
	template <class T, class D>
	bool operator==(std::nullptr_t, const unique_ptr<T, D>& y) noexcept
	{
		return !y;
	}
	template <class T, class D>
	bool operator!=(const unique_ptr<T, D>& x, std::nullptr_t) noexcept
	{
		return static_cast<bool>(x);
	}
	template <class T, class D>
	bool operator!=(std::nullptr_t, const unique_ptr<T, D>& y) noexcept
	{
		return static_cast<bool>(y);
	}
	template <class T, class D>
	bool operator<(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return less<typename unique_ptr<T, D>::pointer>{}(x.get(), nullptr);
	}
	template <class T, class D>
	bool operator<(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return less<typename unique_ptr<T, D>::pointer>{}(nullptr, y.get());
	}
	template <class T, class D>
	bool operator<=(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return !(nullptr < x);
	}
	template <class T, class D>
	bool operator<=(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return !(y < nullptr);
	}
	template <class T, class D>
	bool operator>(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return nullptr < x;
	}
	template <class T, class D>
	bool operator>(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return y < nullptr;
	}
	template <class T, class D>
	bool operator>=(const unique_ptr<T, D>& x, std::nullptr_t)
	{
		return !(x < nullptr);
	}
	template <class T, class D>
	bool operator>=(std::nullptr_t, const unique_ptr<T, D>& y)
	{
		return !(nullptr < y);
	}
}
