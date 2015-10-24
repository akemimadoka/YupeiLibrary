#pragma once

#include "utility_internal.h"
#include "__swap.h"
#include "compress_pair.h"
#include "compare_funtor.h"
#include "iterator.h"
#include "pointer_traits.h"
#include "SmartPointers\unique_ptr.h"
#include "SmartPointers\shared_ptr.h"
#include <numeric>
#include <cinttypes>

namespace Yupei
{
	namespace Internal
	{
		template<typename Type,
			typename Ptr,
			typename _Value,
			typename = void>
		struct get_const_pointer
		{
			using type = typename pointer_traits<Ptr>::template rebind<const _Value>;
		};

		template<typename Type,
			typename Ptr,
			typename _Value
		>
		struct get_const_pointer<Type, Ptr, _Value, void_t<typename Type::const_pointer>>
		{
			using type = typename Type::const_pointer;
		};

		template<typename Type,
			typename Ptr,
			typename = void>
		struct get_void_pointer
		{
			using type = typename pointer_traits<Ptr>::template rebind<void>;
		};

		template<typename Type,
			typename Ptr
		>
		struct get_void_pointer<Type, Ptr, void_t<typename Type::void_pointer>>
		{
			using type = typename Type::void_pointer;
		};

		template<typename Type,
			typename Ptr,
			typename = void>
		struct get_const_void_pointer
		{
			using type = typename pointer_traits<Ptr>::template rebind<const void>;
		};

		template<typename Type,
			typename Ptr
		>
		struct get_const_void_pointer<Type, Ptr, void_t<typename Type::const_void_pointer>>
		{
			using type = typename Type::const_void_pointer;
		};

		template<typename Type,
			typename Ptr,
			typename = void>
		struct get_allocator_diff
		{
			using type = typename pointer_traits<Ptr>::difference_type;
		};

		template<typename Type,
			typename Ptr
		>
		struct get_allocator_diff<Type, Ptr, void_t<typename Type::difference_type>>
		{
			using type = typename Type::difference_type;
		};

		template<typename Type,
			typename DiffType,
			typename = void>
		struct get_allocator_size_type
		{
			using type = make_unsigned_t<DiffType>;
		};

		template<typename Type,
			typename DiffType
		>
		struct get_allocator_size_type<Type, DiffType, void_t<typename Type::size_type>>
		{
			using type = typename Type::size_type;
		};

		template<typename Alloc,
			typename = void>
		struct is_always_equal_traits
		{
			using type = typename is_empty<Alloc>::type;
		};

		template<typename Alloc>
		struct is_always_equal_traits<Alloc, void_t<typename Alloc::is_always_equal>>
		{
			using type = typename Alloc::is_always_equal;
		};


		template<typename Alloc,
			typename = void>
		struct propagate_on_container_copy_assignment_traits
		{
			using type = false_type;
		};

		template<typename Alloc>
		struct propagate_on_container_copy_assignment_traits<Alloc, void_t<typename Alloc::propagate_on_container_copy_assignment>>
		{
			using type = typename Alloc::propagate_on_container_copy_assignment;
		};

		template<typename Alloc,
			typename = void>
		struct propagate_on_container_move_assignment_traits
		{
			using type = false_type;
		};

		template<typename Alloc>
		struct propagate_on_container_move_assignment_traits<Alloc, void_t<typename Alloc::propagate_on_container_move_assignment>>
		{
			using type = typename Alloc::propagate_on_container_move_assignment;
		};

		template<typename Alloc,
			typename = void>
		struct propagate_on_container_swap_traits
		{
			using type = false_type;
		};

		template<typename Alloc>
		struct propagate_on_container_swap_traits<Alloc, void_t<typename Alloc::propagate_on_container_swap>>
		{
			using type = typename Alloc::propagate_on_container_swap;
		};
	}
	

	namespace Internal
	{
		
		template<typename Alloc,
			typename SizeType,
			typename HintType>
			static inline auto allocate_impl(Alloc& a, SizeType n, HintType hint, int)
			->decltype(a.allocate(n, hint))
		{
			return a.allocate(n, hint);
		}

		template<typename Alloc,
			typename SizeType,
			typename HintType>
			static inline auto allocate_impl(Alloc& a, SizeType n, HintType hint, Internal::WrapInt)
			->decltype(a.allocate(n))
		{
			return a.allocate(n);
		}

		template <class Alloc, class T, class... Args>
		static inline auto construct_impl(int, Alloc& a, T* p, Args&&... args)->decltype(a.construct(a.construct(p, Yupei::forward<Args>(args)...)))
		{
			return a.construct(a.construct(p, Yupei::forward<Args>(args)...));
		}

		template <class Alloc, class T, class... Args>
		static inline void construct_impl(Internal::WrapInt, Alloc& a, T* p, Args&&... args)
		{
			::new (static_cast<void*>(p)) T(Yupei::forward<Args>(args)...);
		}

		template <class Alloc, class T>
		static auto destroy_impl(Alloc& a, T* p, int)->decltype(a.destroy(p))
		{
			return a.destroy(p);
		}

		template <class Alloc, class T>
		static void destroy_impl(Alloc& a, T* p, Internal::WrapInt)
		{
			p->~T();
		}

		template<class Alloc>
		static auto max_size_impl(const Alloc& a, int) noexcept
			->decltype(a.max_size())
		{
			return a.max_size();
		}

		template<class SizeType, class Alloc>
		static SizeType max_size_impl(const Alloc& a, Internal::WrapInt) noexcept
		{
			return std::numeric_limits<SizeType>::max();
		}

		template<class Alloc>
		static auto select_on_container_copy_construction_impl(const Alloc& rhs, int)
			->decltype(rhs.select_on_container_copy_construction())
		{
			return rhs.select_on_container_copy_construction();
		}

		template<class Alloc>
		static Alloc select_on_container_copy_construction_impl(const Alloc& rhs, Internal::WrapInt)
		{
			return rhs;
		}
	}



	//minimal allocator_traits support
	template<typename Alloc>
	struct allocator_traits
	{
		using allocator_type = Alloc;
		using value_type = typename allocator_type::value_type;
		using pointer = get_pointer_t<allocator_type>;
		using const_pointer = typename Internal::get_const_pointer<allocator_type, pointer, value_type>::type;
		using void_pointer = typename Internal::get_void_pointer<allocator_type, pointer>::type;
		using const_void_pointer = typename Internal::get_const_void_pointer<allocator_type, pointer>::type;
		using difference_type = typename Internal::get_allocator_diff<allocator_type, pointer>::type;
		using size_type = typename Internal::get_allocator_size_type<allocator_type, difference_type>::type;
		using is_always_equal = typename Internal::is_always_equal_traits<Alloc>::type;
		using propagate_on_container_copy_assignment = typename Internal::propagate_on_container_copy_assignment_traits<allocator_type>::type;
		using propagate_on_container_move_assignment = typename Internal::propagate_on_container_move_assignment_traits<allocator_type>::type;
		using propagate_on_container_swap = typename Internal::propagate_on_container_swap_traits<allocator_type>::type;
		template<class T>
		struct rebind_alloc
		{
			using other = typename Internal::get_rebind<allocator_type, T>::type;
		};
		template <class T>
		struct rebind_traits
		{
			using other = allocator_traits<typename rebind_alloc<T>::other >;
		};

		static pointer allocate(Alloc& a, size_type n)
		{
			return a.allocate(n);
		}
		static pointer allocate(Alloc& a, size_type n, const_void_pointer hint)
		{
			return Internal::allocate_impl(a, n, hint, 0);
		}

		static void deallocate(Alloc& a, pointer p, size_type n)
		{
			a.deallocate(p, n);
		}
		template <class T, class... Args>
		static void construct(Alloc& a, T* p, Args&&... args)
		{
			Internal::construct_impl(0, a, p, Yupei::forward<Args>(args)...);
		}
		template <class T>
		static void destroy(Alloc& a, T* p)
		{
			Internal::destroy_impl(a, p, 0);
		}
		static size_type max_size(const Alloc& a) noexcept
		{
			return Internal::max_size_impl<size_type>(a, 0);
		}
		static Alloc select_on_container_copy_construction(const Alloc& rhs)
		{
			return Internal::select_on_container_copy_construction_impl(rhs, 0);
		}

	};

	//Effects: If it is possible to fit size bytes of storage aligned by alignment into the buffer pointed to by
	//ptr with length space, the function updates ptr to point to the first possible address of such storage
	//and decreases space by the number of bytes used for alignment.Otherwise, the function does nothing.
	inline void* align(std::size_t alignment, std::size_t size,
		void*& ptr, std::size_t& space)
	{
		std::size_t offset = static_cast<std::size_t>(reinterpret_cast<std::uintptr_t>(ptr) & (alignment - 1));
		if (offset > 0) offset = alignment - offset;
		if (offset > space || space - offset > size) return nullptr;
		else
		{
			ptr = static_cast<char*>(ptr) - static_cast<std::ptrdiff_t>(offset);
			space -= offset;
			return ptr;
		}
	}

	template <class T>
	class allocator;
	// specialize for void:
	template <> class allocator<void> {
	public:
		typedef void* pointer;
		typedef const void* const_pointer;
		// reference-to-void members are impossible.
		typedef void value_type;
		template <class U> struct rebind { typedef allocator<U> other; };
	};
	template <class T>
	class allocator
	{
	public:
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;
		template <class U> struct rebind { typedef allocator<U> other; };
		typedef true_type propagate_on_container_move_assignment;
		typedef true_type is_always_equal;
		allocator() noexcept = default;
		allocator(const allocator&) noexcept = default;
		template <class U> allocator(const allocator<U>&) noexcept = default;
		~allocator() = default;
		pointer address(reference x) const noexcept
		{
			return Yupei::addressof(x);
		}
		const_pointer address(const_reference x) const noexcept
		{
			return Yupei::addressof(x);
		}
		pointer allocate(
			size_type sz, typename allocator<void>::const_pointer hint = 0)
		{
			return static_cast<pointer>(::operator new(sz * sizeof(value_type)));
		}
		void deallocate(pointer p, size_type n)
		{
			return ::operator delete(p, n);
		}
		size_type max_size() const noexcept
		{
			return static_cast<size_type>(-1) / sizeof(value_type);
		}
		//template<class U, class... Args>
		//void construct(U* p, Args&&... args);
		//template <class U>
		//void destroy(U* p);
	};

	template <class T1, class T2>
	bool operator==(const allocator<T1>&, const allocator<T2>&) noexcept
	{
		return true;
	}
	template <class T1, class T2>
	bool operator!=(const allocator<T1>&, const allocator<T2>&) noexcept
	{
		return false;
	}
	template <class OutputIterator, class T>
	class raw_storage_iterator {
	public:
		typedef output_iterator_tag iterator_category;
		typedef void value_type;
		typedef void difference_type;
		typedef void pointer;
		typedef void reference;
		explicit raw_storage_iterator(OutputIterator x)
			:iter(x)
		{

		}
		raw_storage_iterator& operator*()
		{
			return *this;
		}
		raw_storage_iterator& operator=(const T& element)
		{
			new (static_cast<void*>(&*iter)) T(element)
		}
		raw_storage_iterator& operator++()
		{
			++iter;
			return *this;
		}
		raw_storage_iterator operator++(int)
		{
			auto temp = iter;
			++iter;
			return temp;
		}
		OutputIterator base() const
		{
			return iter;
		}
	private:
		OutputIterator iter;
	};
	//no get_temporary_buffer/return support

	template <class InputIterator, class ForwardIterator>
	ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
		ForwardIterator result)
	{
		for (; first != last; ++result, ++first)
			::new (static_cast<void*>(addressof(*result)))
			typename iterator_traits<ForwardIterator>::value_type(*first);
		return result;
	}
	template <class InputIterator, class Size, class ForwardIterator>
	ForwardIterator uninitialized_copy_n(InputIterator first, Size n,
		ForwardIterator result)
	{
		for (; n > 0; ++result, ++first, --n) {
			::new (static_cast<void*>(addressof(*result)))
				typename iterator_traits<ForwardIterator>::value_type(*first);
		}
		return result;
	}

	template <class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last,
		const T& x)
	{
		for (; first != last; ++first)
			::new (static_cast<void*>(addressof(*first)))
			typename iterator_traits<ForwardIterator>::value_type(x);
	}

	template <class ForwardIterator, class Size, class T>
	ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
	{
		for (; n--; ++first)
			::new (static_cast<void*>(addressof(*first)))
			typename iterator_traits<ForwardIterator>::value_type(x);
		return first;
	}

	

	
}

