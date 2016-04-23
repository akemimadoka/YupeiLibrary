#pragma once

#include "Extensions.hpp"
#include <utility>
#include <cassert>

namespace Yupei
{
    template<typename Closer>
    class HandleWrapper
    {
    public:
        using HandleType = typename Closer::HandleType;

        HandleWrapper() noexcept
            :handle_ { InvalidHandle() }
        {}

        explicit HandleWrapper(HandleType handle) noexcept
            : handle_ { handle }
        {}

        DISABLECOPY(HandleWrapper)

        HandleWrapper(HandleWrapper&& other) noexcept
            : handle_ { other.handle_ }
        {
            other.handle_ = InvalidHandle();
        }

        HandleWrapper& operator=(HandleWrapper&& other) noexcept
        {
            HandleWrapper { std::move(other) }.swap(*this);
            return *this;
        }

        void swap(HandleWrapper& other) noexcept
        {
            using std::swap;
            swap(handle_, other.handle_);
        }

        constexpr explicit operator bool() const noexcept
        {
            return handle_ != InvalidHandle();
        }

        HandleType Get() const noexcept
        {
            return handle_;
        }

        HandleType* AddressOf() noexcept
        {
            assert(handle_ == InvalidHandle());
            return &handle_;
        }

        void Reset(HandleType other) noexcept
        {
            const auto old = Get();
            handle_ = other;
            if (old != InvalidHandle()) Closer()(old);
        }

        HandleType Release() noexcept
        {
            const auto oldHandle = Get();
            handle_ = InvalidHandle();
            return oldHandle;
        }

        ~HandleWrapper()
        {
            if (handle_ != InvalidHandle())
                Closer()(handle_);
        }

        static /*constexpr */HandleType InvalidHandle() noexcept
        {
            return Closer::InvalidHandle;
        }

    private:
        HandleType handle_;
    };

    template<typename T>
    void swap(HandleWrapper<T>& lhs, HandleWrapper<T>& rhs) noexcept
    {
        lhs.swap(rhs);
    }
}