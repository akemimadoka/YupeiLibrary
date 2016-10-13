#pragma once

#include "Meta.hpp"
#include <iterator>
#include <cstddef> //for std::ptrdiff_t

namespace Yupei
{
    //containers

	namespace Internal
	{
		template<typename ContainerT>
		using IteratorType = decltype(std::begin(std::declval<ContainerT>()));

		template<typename ContainerT>
		using SentinelType = decltype(std::end(std::declval<ContainerT>()));

		template<typename ContainerT>
		using SizeType = decltype(std::size(std::declval<ContainerT>()));

		template<typename ContainerT>
		using ContainerValueType = std::remove_reference_t<std::remove_cv_t<decltype(*std::declval<IteratorType<ContainerT>>())>>;
	}

    template<typename ContainerT>
	using iterator_t = Internal::IteratorType<ContainerT>;

	template<typename ContainerT>
	using sentinel_t = Internal::SentinelType<ContainerT>;

	template<typename T, typename = void>
	struct is_container : std::false_type {};

	template<typename T>
	struct is_container<T, std::void_t<Internal::IteratorType<T>, Internal::SentinelType<T>>> : std::true_type {};

    template<typename ContainerT>
	using size_type_t = Internal::SizeType<ContainerT>;

	template<typename ContainerT>
	using container_value_type_t = Internal::ContainerValueType<ContainerT>;

    //iterators

    template<typename IteratorT>
    using iterator_category_t = typename std::iterator_traits<IteratorT>::iterator_category;

    template<typename IteratorT>
	using iterator_value_type_t = typename std::iterator_traits<IteratorT>::value_type;

    template<typename IteratorT>
	using reference_t = typename std::iterator_traits<IteratorT>::reference;

    template<typename IteratorT>
	using difference_t = typename std::iterator_traits<IteratorT>::difference_type;

    template<typename IteratorT>
	using pointer_t = typename std::iterator_traits<IteratorT>::pointer;

	template<typename IteratorT>
	using const_pointer_t = typename std::iterator_traits<IteratorT>::const_pointer;

    template<typename IteratorT, typename = void>
    struct is_iterator : std::false_type {};

    template<typename IteratorT>
    struct is_iterator<IteratorT, std::void_t<iterator_value_type_t<IteratorT>, iterator_category_t<IteratorT>>> : std::true_type {};

	namespace Internal
	{
		/*template<typename T, typename Category>
		struct IsSomeIteratorCategory : lazy_and<is_iterator<T>::value, lazy<std::is_base_of, identity<Category>, lazy<iterator_category_t, identity<T>>>> {};*/
		template<typename T, typename Category, bool = is_iterator<T>::value>
		struct IsSomeIteratorCategory;

		template<typename T, typename Category>
		struct IsSomeIteratorCategory<T, Category, true> : std::is_base_of<Category, iterator_category_t<T>> {};

		template<typename T, typename Category>
		struct IsSomeIteratorCategory<T, Category, false> : std::false_type {};
	}

	template<typename T>
	using is_input_iterator = Internal::IsSomeIteratorCategory<T, std::input_iterator_tag>;

	template<typename T>
	using is_output_iterator = Internal::IsSomeIteratorCategory<T, std::output_iterator_tag>;

	template<typename T>
	using is_forward_iterator = Internal::IsSomeIteratorCategory<T, std::forward_iterator_tag>;

	template<typename T>
	using is_bidirectional_iterator = Internal::IsSomeIteratorCategory<T, std::bidirectional_iterator_tag>;

	template<typename T>
	using is_random_access_iterator = Internal::IsSomeIteratorCategory<T, std::random_access_iterator_tag>;

	//http://120.52.72.41/www.open-std.org/c3pr90ntcsf0/jtc1/sc22/wg21/docs/papers/2014/n4183.pdf

	template<typename T>
	constexpr T* do_pointer_from(T* p) noexcept { return p; }

	template<typename ContiguousIterator>
	constexpr auto pointer_from(ContiguousIterator i) noexcept(noexcept(do_pointer_from(i)))
		-> decltype(do_pointer_from(i))  // necessary for SFINAE     
	{
		return do_pointer_from(i);
	}

	template<typename IteratorT, typename = void>
	struct is_contiguous_iterator : std::false_type {};

	template<typename IteratorT>
	struct is_contiguous_iterator<IteratorT, std::void_t<decltype(Yupei::pointer_from(std::declval<IteratorT&>()))>> : std::true_type {};	
}