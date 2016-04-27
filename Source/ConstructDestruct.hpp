#pragma once

#include "Ranges\Xrange.hpp"
#include <utility>  //for std::forward
#include <type_traits>

namespace Yupei
{
    template<typename ObjectT, typename... ParamsT>
    void construct(ObjectT* ptr, ParamsT&&... params) noexcept(std::is_nothrow_constructible<ObjectT, ParamsT&&...>::value)
    {
        ::new (static_cast<void*>(ptr)) ObjectT(std::forward<ParamsT>(params)...);
    }

    template<typename ObjectT>
    void destroy(const ObjectT* ptr) noexcept
    {
        ptr->~ObjectT();
    }

    namespace Internal
    {
        template<typename ObjectT>
        void DestroyNImp(ObjectT*, std::size_t, std::true_type) noexcept
        {
            ;//no-op
        }

        template<typename ObjectT>
        void DestroyNImp(ObjectT* ptr, std::size_t count, std::false_type) noexcept
        {
            for (std::size_t i {};i < count;++i)
                Yupei::destroy(ptr + i);
        }
    }

    template<typename ObjectT>
    void destroy_n(ObjectT* ptr, std::size_t count) noexcept
    {
        Internal::DestroyNImp(ptr, count, std::is_trivially_destructible<ObjectT>());
    }

    template<typename ObjectT, typename... ParamsT>
    ObjectT* construct_n(ObjectT* ptr, std::size_t count, ParamsT&&... param) noexcept(std::is_nothrow_constructible<ObjectT, ParamsT&&...>::value)
    {
        for (auto i : xrange(count))
            (void)Yupei::construct(ptr + i, std::forward<ParamsT>(param)...);
        return ptr + count;
    }
}