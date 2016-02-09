#pragma once

#include "TypeTraits.h"
#include <iterator>
#include <cstddef> //for std::ptrdiff_t

namespace Yupei
{
    //containers

    template<typename ContainerT>
    using iterator_t = typename ContainerT::iterator;

    template<typename ContainerT>
    using const_iterator_t = typename ContainerT::const_iterator;

    //iterators

    namespace Internal
    {
        template<typename IteratorT, bool IsPointer = std::is_pointer<IteratorT>::value>
        struct ValueTypeImp;

        template<typename IteratorT>
        struct ValueTypeImp<IteratorT, false>
        {
            using type = typename IteratorT::value_type;
        };

        template<typename IteratorT>
        struct ValueTypeImp<IteratorT, true>
        {
            using type = std::remove_reference_t<decltype(*std::declval<IteratorT>())>;
        };

        template<typename T>
        struct IteratorCategoryImp
        {
            using type = typename T::iterator_category;
        };

        template<typename T>
        struct IteratorCategoryImp<T*>
            : std::enable_if<std::is_object<T>::value, std::random_access_iterator_tag> {};

        template<typename T>
        struct DifferentImp
        {
            using type = typename T::difference_type;
        };

        template<typename T>
        struct DifferentImp<T*>
            :std::enable_if<std::is_object<T>::value, std::ptrdiff_t> {};

        template<typename T>
        using ReferenceOp = typename T::reference;

        template<typename T>
        using PointerOp = typename T::pointer;

    }
    template<typename T>
    using iterator_category_t = typename Internal::IteratorCategoryImp<std::remove_cv_t<T>>::type;

    template<typename IteratorT>
    using value_type_t = typename Internal::ValueTypeImp<IteratorT>::type;

    template<typename T>
    using reference_t = deteced_or_t<value_type_t<T>&, Internal::ReferenceOp, T>;

    template<typename T>
    using difference_t = typename Internal::DifferentImp<std::decay_t<T>>::type;

    template<typename T>
    using pointer_t = deteced_or_t<value_type_t<T>*, Internal::PointerOp, T>;

    template<typename IteratorT>
    struct is_input_iterator : bool_constant<std::is_base_of<std::input_iterator_tag, iterator_category_t<IteratorT>>> {};
}

